#include <includes.hpp>
#include <tools/fastfile/linkers/linker_memory.hpp>

namespace fastfile::linker::memory {
	XBlockLinker::XBlockLinker(size_t numblocks, size_t virtualBlock) : numblocks(numblocks), virtualBlock(virtualBlock) {
		if (numblocks > LINKER_MAX_BLOCKS) {
			throw std::runtime_error(std::format("numblocks={} > LINKER_MAX_BLOCKS={}", numblocks, LINKER_MAX_BLOCKS));
		}
		SetBlockType(virtualBlock, BLOCKTYPE_VIRTUAL);
	}

	void XBlockLinker::SetMode(LinkerMode mode) {
		if (mode == linkerMode) {
			return; // ignore
		}
		if (blockStackIndex) {
			throw std::runtime_error("XBlockLinker::SetMode: Can't set linker mode with non empty chunks, missing PopStream()?");
		}
		linkerMode = mode;
	}

	void XBlockLinker::SetBlockType(size_t block, LinkerBlockType type) {
		AssertBlock(block);
		blocks[block].type = type;
	}

	LinkerDataChunk* XBlockLinker::AddChunk(LinkerDataChunkType type, size_t align, void* data, size_t size) {
		LinkerDataChunk* chunk{ CreateChunk(type, align, data, size) };
		LinkerBlock& block{ GetBlock() };
		if (!block.data[linkerMode]) {
			// first
			block.data[linkerMode] = chunk;
			block.last[linkerMode] = chunk;
		}
		else {
			block.last[linkerMode]->next = chunk;
			chunk->last = block.last[linkerMode];
			block.last[linkerMode] = chunk;
		}
		return chunk;
	}
	LinkerDataChunk* XBlockLinker::CreateChunk(LinkerDataChunkType type, size_t align, void* data, size_t size) {
		LinkerDataChunk* chunk{ alloc.New<LinkerDataChunk>() };
		chunk->type = type;
		chunk->align = align;
		chunk->data = data;
		chunk->size = size;
		return chunk;
	}

	void XBlockLinker::PushStream(size_t block) {
		AssertBlock(block);
		if (blockStackIndex + 1 == ACTS_ARRAYSIZE(blockStack)) {
			throw std::runtime_error("XBlockLinker::PushStreamPos: Stack overflow");
		}
		blockStack[++blockStackIndex] = block;
		AddChunk(LinkerDataChunkType::CHUNKTYPE_PUSH, 0, nullptr, 0);
	}

	void XBlockLinker::PopStream() {
		if (blockStackIndex == 0) {
			throw std::runtime_error("XBlockLinker::PopStreamPos: Stack underflow");
		}
		AddChunk(LinkerDataChunkType::CHUNKTYPE_POP, 0, nullptr, 0);
		blockStackIndex--;
	}

	LinkerBlock& XBlockLinker::GetBlock() {
		if (blockStackIndex == 0) {
			throw std::runtime_error("XBlockLinker::GetBlock: No block set");
		}
		return blocks[blockStack[blockStackIndex]];
	}

	LinkerDataChunk* XBlockLinker::Align(size_t align) {
		return AddChunk(LinkerDataChunkType::CHUNKTYPE_ALIGN, align, nullptr, 0);
	}

	LinkerDataChunk* XBlockLinker::WriteStream(const void* data, size_t size) {
		LinkerDataChunk* chunk{ AllocStream(size) };
		std::memcpy(chunk->data, data, size);
		return chunk;
	}

	LinkerDataChunk* XBlockLinker::AllocStream(size_t size) {
		if (GetBlock().type == BLOCKTYPE_RUNTIME) {
			throw std::runtime_error("XBlockLinker::AllocStream: Can't alloc data for a runtime xblock");
		}
		LinkerDataChunk* chunk{ AllocRuntime(size) };

		if (!zone[linkerMode]) {
			// first
			zone[linkerMode] = chunk;
			zoneLast[linkerMode] = chunk;
		}
		else {
			zoneLast[linkerMode]->nextZone = chunk;
			chunk->lastZone = zoneLast[linkerMode];
			zoneLast[linkerMode] = chunk;
		}

		chunk->data = alloc.Alloc(size);
		std::memset(chunk->data, 0, size);
		return chunk;
	}

	LinkerDataChunk* XBlockLinker::AllocRuntime(size_t size) {
		return AddChunk(LinkerDataChunkType::CHUNKTYPE_DATA, 0, nullptr, size);
	}

	uint32_t XBlockLinker::AddScrString(const char* value) {
		if (!value || !*value) {
			return 0;
		}

		for (size_t i = 0; i < scrStrings.size(); i++) {
			if (!std::strcmp(value, scrStrings[i])) {
				return (uint32_t)i + 1; // +1 for the null str
			}
		}
		scrStrings.push_back(alloc.CloneStr(value));
		return (uint32_t)scrStrings.size();
	}

	AssetData* XBlockLinker::AddAsset(size_t type, LinkerDataChunk* align) {
		LOG_DEBUG("XBlockLinker::AddAsset({})", type);
		AssetData* asset{ alloc.New<AssetData>() };
		asset->header = (void*)POINTER_NEXT;
		asset->type = type;
		asset->align = align;
		assets.emplace_back(asset);
		return asset;
	}

	void XBlockLinker::AddXString(const char*& value, const char* val) {
		// todo: add relative string
		value = (const char*)POINTER_NEXT;
		this->WriteStream(val);
	}

	void XBlockLinker::AssertBlock(size_t block) {
		if (block >= numblocks) {
			throw std::runtime_error(std::format("XBlockLinker::AssertBlock: Invalid linker block {}: Too high", block));
		}
	}

	static void ApplyRefs(LinkerDataChunk& chunk, size_t blockIndex, size_t size) {
		if (!chunk.refs.empty()) {
			// +1 because 0|0 != nullptr
			void* loc{ (void*)((size | ((uint64_t)blockIndex << 60)) + 1) };

			for (void** ref : chunk.refs) {
				*ref = loc;
			}
		}
	}

	static size_t LoadChunkSize(size_t blockIndex, LinkerDataChunk* chunk, size_t startSize) {
		size_t size{ startSize };
		for (; chunk; chunk = chunk->next) {
			ApplyRefs(*chunk, blockIndex, size);

			switch (chunk->type) {
			case CHUNKTYPE_DATA:
				size += chunk->size;
				break;
			case CHUNKTYPE_ALIGN:
				size = utils::Aligned<size_t>(size, chunk->align);
				break;
			case CHUNKTYPE_POP:
			case CHUNKTYPE_PUSH:
				break;
			default:
				throw std::runtime_error(std::format("Invalid chunk type: {}", (int)chunk->type));
			}
		}

		return size;
	}
	static size_t LoadChunkSizeTemp(size_t blockIndex, LinkerDataChunk* chunk, size_t maxSize) {
		size_t size{};

		std::stack<size_t> lens{};

		for (; chunk; chunk = chunk->next) {
			if (!chunk->refs.empty()) {
				LOG_WARNING("LoadChunkSizeTemp found a ref in TEMP block {}, it is probably an error.", blockIndex);
			}
			ApplyRefs(*chunk, blockIndex, size);

			switch (chunk->type) {
			case CHUNKTYPE_DATA:
				size += chunk->size;
				break;
			case CHUNKTYPE_ALIGN:
				size = utils::Aligned<size_t>(size, chunk->align);
				break;
			case CHUNKTYPE_POP:
				if (size > maxSize) {
					maxSize = size;
				}
				size = lens.top();
				lens.pop();
				break;
			case CHUNKTYPE_PUSH:
				lens.push(size);
				break;
			default:
				throw std::runtime_error(std::format("Invalid chunk type: {}", (int)chunk->type));
			}
		}

		if (size > maxSize) {
			maxSize = size;
		}
		return maxSize;
	}

	void XBlockLinker::Link(size_t* blocks) {
		if (blockStackIndex) {
			throw std::runtime_error(std::format("XBlockLinker::Link: block stack not empty {}", blockStackIndex));
		}

		for (size_t i = 0; i < numblocks; i++) {
			LinkerBlock& block{ this->blocks[i] };

			switch (block.type) {
			case BLOCKTYPE_TEMP:
				block.finalSize = LoadChunkSizeTemp(i, block.data[LM_DATA], LoadChunkSizeTemp(i, block.data[LM_HEADER], 0));
				break;
			case BLOCKTYPE_RUNTIME:
			case BLOCKTYPE_VIRTUAL:
				block.finalSize = LoadChunkSize(i, block.data[LM_DATA], LoadChunkSize(i, block.data[LM_HEADER], 0));
				break;
			case BLOCKTYPE_UNKNOWN:
				if (block.data[LM_HEADER] || block.data[LM_DATA]) {
					throw std::runtime_error(std::format("XBlockLinker::Link: block 0x{:x} was used, but the type isn't known", i));
				}
				break;
			default:
				throw std::runtime_error(std::format("XBlockLinker::Link: unimplemented block {} type {}", i, (int)block.type));
				break;
			}
			LOG_TRACE("Block size {}: 0x{:x}", i, block.finalSize);
			blocks[i] = block.finalSize;
		}
	}

	void XBlockLinker::WriteLinkedData(std::vector<byte>& data) {
		for (size_t i = 0; i < LM_COUNT; i++) {
			for (LinkerDataChunk* chunk = zone[i]; chunk; chunk = chunk->nextZone) {
				if (chunk->data && chunk->size) {
					utils::WriteValue(data, chunk->data, chunk->size);
				}
			}
		}
	}
}