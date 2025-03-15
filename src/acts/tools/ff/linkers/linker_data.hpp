#pragma once
#include <core/memory_allocator.hpp>

namespace fastfile::linker::data {

	enum LinkerDataChunkType {
		CHUNKTYPE_DATA = 1,
		CHUNKTYPE_ALIGN = 2,
		CHUNKTYPE_POP = 3,
		CHUNKTYPE_PUSH = 4,
	};
	struct LinkerDataChunk {
		LinkerDataChunkType type;
		size_t align;
		size_t start;
		size_t size;
	};

	struct AssetData {
		size_t type;
		size_t header;
	};

	constexpr size_t POINTER_NEXT = static_cast<size_t>(-1ll);
	constexpr size_t POINTER_NEXT_POINTER = static_cast<size_t>(-2ll);

	enum LinkerMode {
		LM_HEADER = 0,
		LM_DATA = 1,
		LM_COUNT
	};

	class LinkerData {
	public:
		std::vector<byte> fileData[LM_COUNT]{};
		std::vector<std::vector<LinkerDataChunk>> chunks[LM_COUNT]{};
		std::stack<size_t> blockTypes{};
		std::vector<AssetData> assets{};
		core::memory_allocator::MemoryAllocator alloc{};
		std::vector<const char*> strings{};
		std::unordered_set<size_t> tempBlocks;
		LinkerMode headerMode{};
		size_t numBlocks;

		template<typename... Args>
		LinkerData(size_t numBlocks, Args... tempBlocks) : numBlocks(numBlocks) {
			for (size_t i = 0; i < LM_COUNT; i++) {
				chunks[i].resize(numBlocks);
			}
			SetTempBlock(tempBlocks...);
		}

		template<typename... Args>
		void SetTempBlock(size_t block, Args... blocks) {
			SetTempBlock(block);
			SetTempBlock(blocks...);
		}
		template<>
		void SetTempBlock(size_t block) {
			tempBlocks.insert(block);
		}

		size_t GetCurrentStream() const;
		void PushStream(size_t blockType);
		void PopStream();
		void Align(size_t alignment);
		void WriteData(const void* data, size_t size);
		template<typename Type>
		void WriteData(const Type& data) {
			WriteData(&data, sizeof(data));
		}
		void WriteData(const char* str) {
			WriteData(str, std::strlen(str) + 1);
		}
		void WriteData(const std::string& str) {
			WriteData(str.data(), str.size() + 1);
		}
		void SetMode(LinkerMode mode);
		size_t AddString(const char* str);
		template<typename Type = void>
		void Align() {
			Align(sizeof(Type));
		}
		size_t AddString(const std::string& str) { return AddString(str.data()); }
		void AddAsset(size_t type, size_t header);
		void Link(std::vector<byte>& data, size_t* blocks);
	};
}