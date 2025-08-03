#include <includes.hpp>
#include "linker_data.hpp"

namespace fastfile::linker::data {

	size_t LinkerData::GetCurrentStream() const {
		if (blockTypes.empty()) {
			throw std::runtime_error("No linker stream set");
		}
		return blockTypes.top();
	}

	void LinkerData::PushStream(size_t blockType) {
		if (blockType >= numBlocks) {
			throw std::runtime_error(std::format("Block type above limit: {} >= {}", numBlocks, blockType));
		}
		blockTypes.push(blockType);
		chunks[headerMode][blockType].emplace_back(LinkerDataChunk{ .type = LinkerDataChunkType::CHUNKTYPE_PUSH });
	}

	void LinkerData::PopStream() {
		if (blockTypes.empty()) throw std::runtime_error("Can't pop linker with no stream set");
		chunks[headerMode][GetCurrentStream()].emplace_back(LinkerDataChunk{.type = LinkerDataChunkType::CHUNKTYPE_POP});
		blockTypes.pop();
	}

	void LinkerData::Align(size_t alignment) {
		chunks[headerMode][GetCurrentStream()].emplace_back(LinkerDataChunk{ .type = LinkerDataChunkType::CHUNKTYPE_ALIGN, .align = alignment });
	}

	template<>
	void* LinkerData::GetData(size_t offset) {
		return &fileData[headerMode][offset];
	}

	size_t LinkerData::AllocData(size_t size) {
		chunks[headerMode][GetCurrentStream()].emplace_back(LinkerDataChunk{ .type = LinkerDataChunkType::CHUNKTYPE_DATA, .start = fileData[headerMode].size(), .size = size });
		return utils::Allocate(fileData[headerMode], size);
	}

	void LinkerData::AllocRuntimeData(size_t size) {
		chunks[headerMode][GetCurrentStream()].emplace_back(LinkerDataChunk{ .type = LinkerDataChunkType::CHUNKTYPE_DATA, .start = fileData[headerMode].size(), .size = size });
	}

	size_t LinkerData::WriteData(const void* data, size_t size) {
		chunks[headerMode][GetCurrentStream()].emplace_back(LinkerDataChunk{ .type = LinkerDataChunkType::CHUNKTYPE_DATA, .start = fileData[headerMode].size(), .size = size });
		return utils::WriteValue(fileData[headerMode], data, size);
	}

	void LinkerData::SetMode(LinkerMode mode) {
		if (mode == headerMode) return; // ignore
		if (!blockTypes.empty()) {
			throw std::runtime_error("Can't set linker mode with non empty chunks");
		}
		headerMode = mode;
	}

	size_t LinkerData::AddString(const char* str) {
		strings.push_back(alloc.CloneStr(str));

		return strings.size();
	}

	void LinkerData::AddAsset(size_t type, size_t header) {
		LOG_TRACE("AddAsset({},{:x})", type, header);
		assets.emplace_back(type, header);
	}

	namespace {
		size_t ComputeChunkSize(std::vector<LinkerDataChunk>& chunks, size_t startSize) {
			size_t size{ startSize };
			for (size_t i = 0; i < chunks.size(); i++) {
				LinkerDataChunk& chunk{ chunks[i] };

				switch (chunk.type) {
				case CHUNKTYPE_DATA:
					size += chunk.size;
					break;
				case CHUNKTYPE_ALIGN:
					//size = utils::Aligned<size_t>(size, chunk.align);
					size += chunk.align - 1; // lazy
					break;
				case CHUNKTYPE_POP:
				case CHUNKTYPE_PUSH:
					break;
				default:
					throw std::runtime_error(std::format("Invalid chunk type: {}", (int)chunk.type));
				}
			}

			return size;
		}
		size_t ComputeChunkSizeTemp(std::vector<LinkerDataChunk>& chunks, size_t startSize, size_t maxSize) {
			size_t size{ startSize };

			std::stack<size_t> lens{};

			for (size_t i = 0; i < chunks.size(); i++) {
				LinkerDataChunk& chunk{ chunks[i] };

				switch (chunk.type) {
				case CHUNKTYPE_DATA:
					size += chunk.size;
					break;
				case CHUNKTYPE_ALIGN:
					size = utils::Aligned<size_t>(size, chunk.align);
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
					throw std::runtime_error(std::format("Invalid chunk type: {}", (int)chunk.type));
				}
			}

			if (size > maxSize) {
				maxSize = size;
			}
			return maxSize;
		}
	}

	void LinkerData::Link(std::vector<byte>& data, size_t* blocks) {
		if (!blockTypes.empty()) {
			throw std::runtime_error("Can't link sizes with non empty chunks");
		}

		// compute block sizes, later this part should do the linking
		for (size_t i = 0; i < numBlocks; i++) {
			bool isTemp{ tempBlocks.contains(i) };

			if (isTemp) {
				size_t size{ ComputeChunkSizeTemp(chunks[LM_HEADER][i], 0, 0) };
				size = ComputeChunkSizeTemp(chunks[LM_DATA][i], 0, size);
				blocks[i] = size;
			}
			else {
				size_t size{ ComputeChunkSize(chunks[LM_HEADER][i], 0) };
				size = ComputeChunkSize(chunks[LM_DATA][i], size);
				blocks[i] = size;
			}

		}

		// load data
		utils::WriteValue(data, fileData[LM_HEADER].data(), fileData[LM_HEADER].size());
		utils::WriteValue(data, fileData[LM_DATA].data(), fileData[LM_DATA].size());
	}
}