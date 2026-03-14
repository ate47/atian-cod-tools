#pragma once

namespace filesystem::afs {

	constexpr uint64_t AFS_MAGIC = 0x0d0AF5008010;

	struct ActsFileSystemFilePre {
		uint64_t name;
		uint64_t offset;
		uint64_t size;
	};

	struct ActsFileSystemFile {
		const char* name;
		byte* offset;
		uint64_t size;
	};

	struct ActsFileSystemHeader {
		byte magic[sizeof(uint64_t)];
		uint64_t fileTable{};
		uint32_t fileCount{};
		uint32_t fileItemSize{};
	};

	class ActsFileSystem {
		byte* data;

		std::unordered_map<std::string, ActsFileSystemFile*> files{};
	public:
		ActsFileSystem(byte* data, size_t len);

		ActsFileSystemFile* QueryFS(const std::string& path);
	};

	class ActsFileSystemWriter {
		std::vector<byte> data{};
		std::vector<ActsFileSystemFilePre> files{};

	public:
		ActsFileSystemWriter();

		void WriteFile(const char* name, void* file, size_t len);
		inline void WriteFile(const std::string& name, void* file, size_t len) {
			WriteFile(name.c_str(), file, len);
		}
		void Save(const std::filesystem::path& output);
	};
}