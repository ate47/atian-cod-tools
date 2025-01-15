#include <includes.hpp>
#include "fastfile_handlers.hpp"
#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)


namespace fastfile {
	std::vector<FFHandler*>& GetHandlers() {
		static std::vector<FFHandler*> handlers{};
		return handlers;
	}

	FFHandler* FindHandler(uint64_t magic) {
		for (FFHandler* handler : GetHandlers()) {
			if (handler->magic == (magic & handler->mask)) {
				return handler;
			}
		}
		return nullptr;
	}

	void* FFAssetPool::FindAssetHeader(size_t type, uint64_t name) {
		if (type >= pool.size()) return nullptr;
		auto it{ pool[type].find(name) };
		if (it == pool[type].end()) return nullptr;
		return it->second;
	}

	void FFAssetPool::AddAssetHeader(size_t type, uint64_t name, void* header) {
		if (type >= pool.size()) {
			pool.resize(type + 1);
		}
		pool[type][name] = header;
	}

	void FFAssetPool::ClearRef() {
		pool.clear();
		memory.clear();
	}

	std::vector<byte>& FFAssetPool::CreateMemoryBuffer() {
		return *memory.emplace_back(std::make_unique<std::vector<byte>>()).get();
	}

	const char* GetFastFileCompressionName(FastFileCompression comp) {
		static const char* compressionNames[]{
			"none",
			"zlib",
			"zlib_hc",
			"lz4",
			"lz4_hc",
			"bdelta_uncomp",
			"bdelta_zlib",
			"bdelta_lzma",
			"oodle_kraken",
			"oodle_mermaid",
			"oodle_selkie",
			"oodle_lzna",
		};
		if (comp >= ARRAYSIZE(compressionNames) || comp < 0) {
			return "unknown";
		}
		return compressionNames[comp];
	}

	FastFileOption::~FastFileOption() {
		if (cascStorage) {
			CascCloseStorage(cascStorage);
		}
	}

	bool FastFileOption::Compute(const char** args, size_t startIndex, size_t endIndex) {
		// default values
		for (size_t i = startIndex; i < endIndex; i++) {
			const char* arg = args[i];

			if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
				m_help = true;
			}
			else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				m_output = args[++i];
			}
			else if (!strcmp("-g", arg) || !_strcmpi("--game", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				game = args[++i];
			}
			else if (!strcmp("-C", arg) || !_strcmpi("--casc", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				m_casc = args[++i];

				if (!CascOpenStorage(m_casc, 0, &cascStorage)) {
					LOG_ERROR("Can't open casc storage {}: 0x{:x}", m_casc, GetCascError());
					return false;
				}
			}
			else if (!_strcmpi("--fd", arg) || !strcmp("-p", arg)) {
				m_fd = true;
			}
			else if (!_strcmpi("--header", arg) || !strcmp("-H", arg)) {
				m_header = true;
			}
			else if (*arg == '-') {
				std::cerr << "Invalid argument: " << arg << "!\n";
				return false;
			}
			else {
				files.push_back(arg);
			}
		}
		return true;
	}

	void FastFileOption::PrintHelp() {
		LOG_INFO("-h --help            : Print help");
		LOG_INFO("-o --output [d]      : Output dir");
		LOG_INFO("-H --header          : Dump header info");
		LOG_INFO("-C --casc [c]        : Use casc db");
		LOG_INFO("-g --game [g]        : exe");
		LOG_INFO("-p --fd              : Use patch file (fd)");
	}

	std::vector<std::string> FastFileOption::GetFileRecurse(const char* path) {
		std::vector<std::string> res{};
		if (cascStorage) {
			CASC_FIND_DATA data;

			HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

			if (!firstFileHandle) {
				LOG_ERROR("Can't find path {}", path);
				return res;
			}
			utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


			do {
				if (data.bFileAvailable) {
					res.push_back(data.szFileName);
				}
			} while (CascFindNextFile(firstFileHandle, &data));
		}
		else {
			std::vector<std::filesystem::path> files{};

			utils::GetFileRecurse(path, files);


			for (const std::filesystem::path& file : files) {
				res.push_back(file.string());
			}
		}

		return res;

	}

	bool FastFileOption::ReadFile(const char* path, std::vector<byte>& buff) {
		if (cascStorage) {
			CASC_FIND_DATA data;

			HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

			if (!firstFileHandle || !data.bFileAvailable) {
				LOG_ERROR("Can't find path {}", path);
				return false;
			}
			utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


			buff.clear();
			if (!data.FileSize) {
				return true; // nothing to read
			}
			buff.resize(data.FileSize);
			HANDLE fhandle;
			if (!CascOpenFile(cascStorage, data.szFileName, CASC_LOCALE_ALL, 0, &fhandle)) {
				LOG_ERROR("Can't open path {}", data.szFileName);
				return false;
			}
			utils::CloseEnd cef{ [fhandle] { CascCloseFile(fhandle); } };

			DWORD read{};
			return CascReadFile(fhandle, buff.data(), (DWORD)data.FileSize, &read) && read == data.FileSize;
		}

		return utils::ReadFile(path, buff);
	}

	int fastfile(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help || opt.files.empty()) {
			opt.PrintHelp();
			if (opt.files.empty()) {
				LOG_ERROR("Missing fast file");
				return tool::BASIC_ERROR;
			}
			else {
				return tool::OK;
			}
		}

		FFAssetPool assetPool{};

		std::vector<byte> buff{};
		int ret{ tool::OK };
		for (const char* f : opt.files) {
			for (const std::string filename : opt.GetFileRecurse(f)) {
				if (!filename.ends_with(".ff")) {
					LOG_DEBUG("Ignore {}", filename);
					continue;
				}

				if (!opt.ReadFile(filename.data(), buff)) {
					LOG_ERROR("Can't read file {}", filename);
					ret = tool::BASIC_ERROR;
					continue;
				}

				core::bytebuffer::ByteBuffer reader{ buff };

				if (!reader.CanRead(sizeof(uint64_t))) {
					LOG_ERROR("Can't read file {}: too small", filename);
					ret = tool::BASIC_ERROR;
					continue;
				}

				uint64_t magic{ *reader.Ptr<uint64_t>() };

				try {
					FFHandler* handler{ FindHandler(magic) };

					if (!handler) {
						LOG_ERROR("Can't open {}: Can't find handler for magic 0x{:x}", filename, magic);
						ret = tool::BASIC_ERROR;
						continue;
					}

					LOG_INFO("Loading {}... ({})", filename, handler->name);

					handler->LoadFastFile(assetPool, opt, reader, filename.c_str());
				}
				catch (std::runtime_error& err) {
					LOG_ERROR("Can't read {}: {}", filename, err.what());
					ret = tool::BASIC_ERROR;
				}

			}
		}

		return ret;
	}

	ADD_TOOL(fastfile, "common", "", "fastfile reader", fastfile);

}