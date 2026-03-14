#include <includes_shared.hpp>
#include "afs.hpp"
#include <utils/utils.hpp>

namespace filesystem::afs {
	ActsFileSystem::ActsFileSystem(byte* data, size_t len) : data(data) {
		ActsFileSystemHeader& header = *(ActsFileSystemHeader*)data;

		if (*(uint64_t*)header.magic != AFS_MAGIC) {
			throw std::runtime_error(utils::va("Invalid magic: 0x%llx", *(uint64_t*)header.magic));
		}

		if (header.fileCount && header.fileTable + (size_t)header.fileCount * header.fileItemSize > len) {
			throw std::runtime_error(utils::va("File table too big: 0x%llx > 0x%llx", header.fileTable + (size_t)header.fileCount * header.fileItemSize, len));
		}

		for (size_t i = 0; i < header.fileCount; i++) {
			ActsFileSystemFile* file = (ActsFileSystemFile*)(header.magic + header.fileTable + i * header.fileItemSize);

			uint64_t nameoffset = reinterpret_cast<uint64_t>(file->name);

			if (nameoffset > len) {
				throw std::runtime_error(utils::va("Invalid name offset for file %lld: 0x%llx", i, nameoffset));
			}
			uint64_t offset = reinterpret_cast<uint64_t>(file->offset);
			if (offset + file->size > len) {
				throw std::runtime_error(utils::va("Invalid file offset for file %lld: 0x%llx(0x%llx)", i, offset, file->size));
			}


			// link file
			file->name = (const char*)(header.magic + nameoffset);
			file->offset = header.magic + offset;


			files[file->name] = file;
		}
	}
	ActsFileSystemFile* ActsFileSystem::QueryFS(const std::string& path) {
		auto it = files.find(path);

		if (it == files.end()) return nullptr;

		return it->second;
	}

	ActsFileSystemWriter::ActsFileSystemWriter() {
		utils::Allocate(data, sizeof(ActsFileSystemHeader));
	}

	void ActsFileSystemWriter::WriteFile(const char* name, void* file, size_t len) {
		if (files.size() == INT32_MAX) throw std::runtime_error("too many files for this datastructure");
		size_t dataOffset = data.size();
		utils::Allocate(data, len);
		memcpy(data.data() + dataOffset, file, len);
		size_t nameOffset = data.size();
		utils::WriteString(data, name);
		files.emplace_back(nameOffset, dataOffset, len);
	}

	void ActsFileSystemWriter::Save(const std::filesystem::path& output) {
		ActsFileSystemHeader& header = *(ActsFileSystemHeader*)data.data();
		*(uint64_t*)(header.magic) = AFS_MAGIC;
		header.fileCount = (uint32_t)files.size();
		header.fileItemSize = sizeof(ActsFileSystemFilePre);
		header.fileTable = data.size();

		std::ofstream out{ output, std::ios::binary };

		if (!out) {
			std::string fn = output.string();
			throw std::runtime_error(utils::va("Can't open file '%s'", fn.c_str()));
		}

		out.write((const char*)data.data(), data.size());
		out.write((const char*)files.data(), files.size() * sizeof(files[0]));
		out.close();
	}
}