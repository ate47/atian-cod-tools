#pragma once
#include <core/memory_allocator.hpp>

namespace fastfile::linker::memory {
	constexpr size_t POINTER_NEXT = static_cast<size_t>(-1ll);
	constexpr size_t POINTER_NEXT_POINTER = static_cast<size_t>(-2ll);

	constexpr size_t LINKER_MAX_BLOCKS = 20;
	constexpr size_t LINKER_STACK_SIZE = 64;

	enum LinkerBlockType {
		BLOCKTYPE_UNKNOWN = 0,
		BLOCKTYPE_TEMP = 1,
		BLOCKTYPE_RUNTIME = 2,
		BLOCKTYPE_VIRTUAL = 3,
	};

	enum LinkerDataChunkType {
		CHUNKTYPE_DATA = 1,
		CHUNKTYPE_ALIGN = 2,
		CHUNKTYPE_POP = 3,
		CHUNKTYPE_PUSH = 4,
	};

	enum LinkerMode {
		LM_HEADER = 0,
		LM_DATA = 1,
		LM_COUNT
	};

	struct LinkerDataChunk {
		LinkerDataChunkType type;
		size_t align{};
		void* data{};
		size_t size{};
		std::vector<void**> refs{};

		template<typename T>
		T* As() {
			return (T*)data;
		}
	};


	struct LinkerBlock {
		std::vector<LinkerDataChunk*> data[LM_COUNT];
		LinkerBlockType type{};
		size_t finalSize{};
	};

	struct AssetData {
		size_t type;
		void* header;
		LinkerDataChunk* chunk{};
	};

	class XBlockLinker {
		core::memory_allocator::MemoryAllocator alloc{};
	public:
		LinkerBlock blocks[LINKER_MAX_BLOCKS]{};
		std::vector<LinkerDataChunk*> zone[LM_COUNT]{};
		LinkerMode linkerMode{};
		size_t blockStack[LINKER_STACK_SIZE]{};
		size_t blockStackIndex{};
		std::vector<AssetData*> assets{};
		std::vector<const char*> scrStrings{};
		size_t numblocks;

		// a xblock linker
		XBlockLinker(size_t numblocks);
		XBlockLinker(const XBlockLinker& other) = delete;
		XBlockLinker(XBlockLinker&& other) = delete;

		// set linker mode
		void SetMode(LinkerMode mode);
		// configure a block type
		void SetBlockType(size_t block, LinkerBlockType type);
		// push a block
		void PushStream(size_t block);
		// pop the block
		void PopStream();
		// align the stream
		void Align(size_t align);
		// create a chunk
		LinkerDataChunk* CreateChunk(LinkerDataChunkType type, size_t align, void* data, size_t size);
		// add chunk to current block
		LinkerDataChunk* AddChunk(LinkerDataChunkType type, size_t align, void* data, size_t size);
		// alloc and write a stream
		LinkerDataChunk* WriteStream(const void* data, size_t size);
		// alloc a stream
		LinkerDataChunk* AllocStream(size_t size);
		// alloc runtime data
		LinkerDataChunk* AllocRuntime(size_t size);
		// link the blocks
		void Link(size_t* blocks);
		// write the data
		void WriteLinkedData(std::vector<byte>& data);
		// add a scr string
		uint32_t AddScrString(const char* value);
		// add an asset
		AssetData* AddAsset(size_t type);

		template<typename Type>
		LinkerDataChunk* WriteStream(const Type& data) {
			return WriteStream(&data, sizeof(data));
		}
		template<typename Type>
		inline LinkerDataChunk* WriteStream(const std::vector<Type>& data) {
			return WriteStream(data.data(), data.size() * sizeof(Type));
		}
		template<typename Type>
		inline LinkerDataChunk* AllocStream() {
			return AllocStream(sizeof(Type));
		}
		template<typename Type>
		inline Type* AllocStreamPtr(size_t count = 1, fastfile::linker::memory::LinkerDataChunk** out = nullptr) {
			fastfile::linker::memory::LinkerDataChunk* stream{ AllocStream(sizeof(Type) * count) };
			if (out) *out = stream;
			return stream->As<Type>();
		}
		template<typename Type>
		inline Type& AllocStreamRef(fastfile::linker::memory::LinkerDataChunk** out = nullptr) {
			return *AllocStreamPtr<Type>(1, out);
		}
		LinkerDataChunk* WriteStream(const char* str) {
			return WriteStream(str, std::strlen(str) + 1);
		}
		LinkerDataChunk* WriteStream(const std::string& str) {
			return WriteStream(str.data(), str.size() + 1);
		}

		template<typename Type = void>
		void Align() {
			Align(sizeof(Type));
		}

		inline uint32_t AddScrString(const std::string& value) {
			return AddScrString(value.data());
		}
	private:
		// get the current last pushed block
		LinkerBlock& GetBlock();
		// assert that a block is valid
		void AssertBlock(size_t block);
	};

}