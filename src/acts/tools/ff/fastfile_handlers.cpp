#include <includes.hpp>
#include "fastfile_handlers.hpp"
#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)


namespace fastfile {
	std::vector<FFDecompressor*>& GetDecompressors() {
		static std::vector<FFDecompressor*> handlers{};
		return handlers;
	}

	std::vector<FFHandler*>& GetHandlers() {
		static std::vector<FFHandler*> handlers{};
		return handlers;
	}

	FFDecompressor* FindDecompressor(uint64_t magic) {
		for (FFDecompressor* handler : GetDecompressors()) {
			if (handler->magic == (magic & handler->mask)) {
				return handler;
			}
		}
		return nullptr;
	}

	FFHandler* FindHandler(const char* name) {
		for (FFHandler* handler : GetHandlers()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	const char* GetFastFileCompressionName(FastFileCompression comp) {
		static const char* names[]{
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
		if (comp >= ARRAYSIZE(names) || comp < 0) {
			return "unknown";
		}
		return names[comp];
	}

	const char* GetFastFileCompressionName(FastFileIWCompression comp) {
		static const char* names[]{
			"invalid",
			"none",
			"zlib_size",
			"zlib_speed",
			"lz4_hc",
			"lz4",
			"oodle_kraken_size",
			"oodle_kraken_speed",
			"hw_zlib_size",
			"hw_zlib_speed",
			"ps4_zlib_size",
			"ps4_zlib_spreed",
			"oodle_leviathan_size",
			"oodle_leviathan_speed",
			"oodle_mermaid_size",
			"oodle_mermaid_speed",
			"oodle_selkie_size",
			"oodle_selkie_speed",
			"zstd_size",
			"zstd_speed",
		};

		if (comp >= ARRAYSIZE(names) || comp < 0) {
			return "unknown";
		}
		return names[comp];
	}

	const char* GetFastFilePlatformName(FastFilePlatform plt) {
		static const char* names[]{
			"pc",
			"xbox",
			"playstation",
			"dev",
		};
		if (plt >= ARRAYSIZE(names) || plt < 0) {
			return "unknown";
		}
		return names[plt];
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileCompression comp) {
		switch (comp) {
		case fastfile::XFILE_UNCOMPRESSED:
			return utils::compress::COMP_NONE;
		case fastfile::XFILE_ZLIB:
		case fastfile::XFILE_ZLIB_HC:
			return utils::compress::COMP_ZLIB;
		case fastfile::XFILE_LZ4:
		case fastfile::XFILE_LZ4_HC:
			return utils::compress::COMP_LZ4;
		case fastfile::XFILE_OODLE_KRAKEN:
		case fastfile::XFILE_OODLE_MERMAID:
		case fastfile::XFILE_OODLE_SELKIE:
		case fastfile::XFILE_OODLE_LZNA:
			return utils::compress::COMP_OODLE;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileIWCompression comp) {
		switch (comp) {
		case fastfile::IWFFC_NONE:
			return utils::compress::COMP_NONE;
		case fastfile::IWFFC_LZ4:
		case fastfile::IWFFC_LZ4_HC:
			return utils::compress::COMP_LZ4;
		case IWFFC_HW_ZLIB_SIZE:
		case IWFFC_HW_ZLIB_SPEED:
		case IWFFC_PS4_ZLIB_SIZE:
		case IWFFC_PS4_ZLIB_SPREED:
			return utils::compress::COMP_ZLIB;
		case IWFFC_OODLE_KRAKEN_SIZE:
		case IWFFC_OODLE_KRAKEN_SPEED:
		case IWFFC_OODLE_LEVIATHAN_SIZE:
		case IWFFC_OODLE_LEVIATHAN_SPEED:
		case IWFFC_OODLE_MERMAID_SIZE:
		case IWFFC_OODLE_MERMAID_SPEED:
		case IWFFC_OODLE_SELKIE_SIZE:
		case IWFFC_OODLE_SELKIE_SPEED:
			return utils::compress::COMP_OODLE;
		case IWFFC_ZSTD_SIZE:
		case IWFFC_ZSTD_SPEED:
			return utils::compress::COMP_ZSTD;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
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
			else if (!strcmp("-r", arg) || !_strcmpi("--handler", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* id{ args[++i] };
				if (!(handler = FindHandler(id))) {
					LOG_ERROR("Can't find handler for name '{}'", id);
					return false;
				}
			}
			else if (!_strcmpi("--patch", arg) || !strcmp("-p", arg)) {
				m_fd = true;
			}
			else if (!strcmp("-R", arg) || !_strcmpi("--handlers", arg)) {
				print_handlers = true;
			}
			else if (!strcmp("-D", arg) || !_strcmpi("--decompressors", arg)) {
				print_decompressors = true;
			}
			else if (!strcmp("-d", arg) || !_strcmpi("--dump", arg)) {
				dump_decompressed = true;
			}
			else if (!_strcmpi("--header", arg) || !strcmp("-H", arg)) {
				m_header = true;
			}
			else if (!_strcmpi("--noAssetDump", arg)) {
				noAssetDump = true;
			}
			else if (!_strcmpi("--assertContainer", arg)) {
				assertContainer = true;
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
		LOG_INFO("-r --handler         : Handler to use (use --handlers to print)");
		LOG_INFO("-R --handlers        : Print handlers");
		LOG_INFO("-D --decompressors   : Print decompressors");
		LOG_INFO("-d --dump            : Dump decompressed (if available)");
		LOG_INFO("-C --casc [c]        : Use casc db");
		LOG_INFO("-g --game [g]        : exe");
		LOG_INFO("-p --patch           : Use patch files (fd/fp)");
		LOG_INFO("--noAssetDump        : No asset dump");
		LOG_INFO("--assertContainer    : Use acts as a container for other software");
	}

	hook::library::Library FastFileOption::GetGame(bool crashError, bool* init) {
		if (init) *init = false;

		if (!game) {
			if (crashError) {
				throw std::runtime_error("No game module specified");
			}
			return (HMODULE)0;
		}

		if (!gameMod) {
			if (!gameMod.Load(game)) {
				throw std::runtime_error(std::format("Can't load {}", game));
			}
			if (init) *init = true;
		}

		return *gameMod;
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

		if (!opt.Compute(argv, 2, argc) || opt.m_help || (opt.files.empty() && !(opt.print_handlers || opt.print_decompressors))) {
			opt.PrintHelp();
			if (opt.files.empty()) {
				LOG_ERROR("Missing fast file");
				return tool::BASIC_ERROR;
			}
			else {
				return tool::OK;
			}
		}
		

		if (opt.print_handlers || opt.print_decompressors) {
			if (opt.print_handlers) {
				LOG_INFO("Handlers:");

				for (FFHandler* handler : GetHandlers()) {
					LOG_INFO("{:10} - {}", handler->name, handler->description);
				}
			}

			if (opt.print_decompressors) {
				LOG_INFO("Decompressors:");

				for (FFDecompressor* decompr : GetDecompressors()) {
					LOG_INFO("{:20} - 0x{:x}/0x{:x}", decompr->name, decompr->magic, decompr->mask);
				}
			}
			return tool::OK;
		}

		if (opt.handler) {
			opt.handler->Init(opt);
		}

		std::vector<byte> buff{};
		std::vector<byte> ffdata{};
		size_t count{}, completed{};
		FFDecompressor* lastDecompressor{};

		utils::CloseEnd hce{ [&opt, &lastDecompressor] {
			if (lastDecompressor) lastDecompressor->Cleanup();
			if (opt.handler) opt.handler->Cleanup();
		} };

		for (const char* f : opt.files) {
			for (const std::string filename : opt.GetFileRecurse(f)) {
				ffdata.clear();
				if (!filename.ends_with(".ff")) {
					LOG_DEBUG("Ignore {}", filename);
					continue;
				}
				count++;

				if (!opt.ReadFile(filename.data(), buff)) {
					LOG_ERROR("Can't read file {}", filename);
					continue;
				}

				core::bytebuffer::ByteBuffer reader{ buff };

				if (!reader.CanRead(sizeof(uint64_t))) {
					LOG_ERROR("Can't read file {}: too small", filename);
					continue;
				}

				uint64_t magic{ *reader.Ptr<uint64_t>() };

				try {
					fastfile::FastFileContext ctx{};
					ctx.file = filename.c_str();
					FFDecompressor* handler{ FindDecompressor(magic) };

					if (!handler) {
						LOG_ERROR("Can't open {}: Can't find decompressor for magic 0x{:x}", filename, magic);
						continue;
					}

					if (lastDecompressor != handler) {
						if (lastDecompressor) lastDecompressor->Cleanup();
						lastDecompressor = handler;
						handler->Init(opt);
					}

					LOG_INFO("Loading {}... ({})", filename, handler->name);

					handler->LoadFastFile(opt, reader, ctx, ffdata);

					LOG_TRACE("Decompressed 0x{:x} byte(s)", ffdata.size());

					if (opt.dump_decompressed) {
						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / ctx.ffname };

						decfile.replace_extension(".ff.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, ffdata.data(), ffdata.size())) {
							LOG_ERROR("Can't dump {}", decfile.string());
						}
						else {
							LOG_INFO("Dump into {}", decfile.string());
						}
					}

					if (opt.handler) {
						core::bytebuffer::ByteBuffer ffreader{ ffdata };
						LOG_TRACE("Reading using {}", opt.handler->name);

						opt.handler->Handle(opt, ffreader, ctx);
					}
					completed++;
				}
				catch (std::runtime_error& err) {
					LOG_ERROR("Can't read {}: {}", filename, err.what());
				}
			}
		}

		size_t errors{ count - completed };
		if (errors) {
			LOG_ERROR("Parsed {} (0x{:x}) file(s) with {} (0x{:x}) error(s)", count, count, errors, errors);
			return tool::BASIC_ERROR;
		}
		LOG_INFO("Parsed {} (0x{:x}) file(s)", count, count);
		return tool::OK;
	}

	ADD_TOOL(fastfile, "common", "", "fastfile reader", fastfile);

}