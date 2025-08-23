#include <includes.hpp>
#include "fastfile_handlers.hpp"
#include <decryptutils.hpp>
#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)
#include <core/actsinfo.hpp>
#include <HwBpLib.h>
#include <regex>


namespace fastfile {
	std::vector<FFDecompressor*>& GetDecompressors() {
		static std::vector<FFDecompressor*> handlers{};
		return handlers;
	}

	std::vector<FFCompressor*>& GetCompressors() {
		static std::vector<FFCompressor*> handlers{};
		return handlers;
	}

	std::vector<FFLinker*>& GetLinkers() {
		static std::vector<FFLinker*> handlers{};
		return handlers;
	}
	
	std::vector<FFHandler*>& GetHandlers() {
		static std::vector<FFHandler*> handlers{};
		return handlers;
	}
	
	bool FFDecompressor::MatchFile(const std::filesystem::path& path, core::bytebuffer::ByteBuffer& data) const {
		uint64_t magic{ *data.Ptr<uint64_t>() };
		return (magic & mask) == (this->magic & mask);
	}

	FFDecompressor* FindDecompressor(const std::filesystem::path& path, core::bytebuffer::ByteBuffer& data) {
		for (FFDecompressor* handler : GetDecompressors()) {
			if (handler->MatchFile(path, data)) {
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

	FFCompressor* FindCompressor(const char* name) {
		for (FFCompressor* handler : GetCompressors()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	FFLinker* FindLinker(const char* name) {
		for (FFLinker* handler : GetLinkers()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	namespace {
		const char* tFFNames[]{
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
		const char* iwFFNames[]{
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
		const char* ffPltNames[]{
			"pc",
			"xbox",
			"playstation",
			"dev",
		};

		fastfile::FastFileContext* currentCtx{};
	}

	fastfile::FastFileContext& GetCurrentContext() {
		return *currentCtx;
	}

	const char* GetFastFileCompressionName(FastFileCompression comp) {
		if (comp >= ARRAYSIZE(tFFNames) || comp < 0) {
			return "unknown";
		}
		return tFFNames[comp];
	}

	const char* GetFastFileCompressionName(FastFileIWCompression comp) {

		if (comp >= ARRAYSIZE(iwFFNames) || comp < 0) {
			return "unknown";
		}
		return iwFFNames[comp];
	}

	const char* GetFastFilePlatformName(FastFilePlatform plt) {
		if (plt >= ARRAYSIZE(ffPltNames) || plt < 0) {
			return "unknown";
		}
		return ffPltNames[plt];
	}
	FastFileIWCompression GetFastFileIWCompression(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(iwFFNames); i++) {
			if (!_strcmpi(iwFFNames[i], name)) {
				return (FastFileIWCompression)i;
			}
		}
		return FastFileIWCompression::IWFFC_COUNT;
	}

	FastFileCompression GetFastFileCompression(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(tFFNames); i++) {
			if (!_strcmpi(tFFNames[i], name)) {
				return (FastFileCompression)i;
			}
		}
		return FastFileCompression::XFILE_COMPRESSION_COUNT;
	}

	FastFilePlatform GetFastFilePlatform(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(ffPltNames); i++) {
			if (!_strcmpi(ffPltNames[i], name)) {
				return (FastFilePlatform)i;
			}
		}
		return FastFilePlatform::XFILE_PLATFORM_COUNT;
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileCompression comp) {
		switch (comp) {
		case fastfile::XFILE_UNCOMPRESSED:
			return utils::compress::COMP_NONE;
		case fastfile::XFILE_ZLIB:
			return utils::compress::COMP_ZLIB;
		case fastfile::XFILE_ZLIB_HC:
			return utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION;
		case fastfile::XFILE_LZ4:
			return utils::compress::COMP_LZ4;
		case fastfile::XFILE_LZ4_HC:
			return utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION;
		case fastfile::XFILE_OODLE_KRAKEN:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN;
		case fastfile::XFILE_OODLE_MERMAID:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID;
		case fastfile::XFILE_OODLE_SELKIE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE;
		case fastfile::XFILE_OODLE_LZNA:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LZNA;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileIWCompression comp) {
		switch (comp) {
		case fastfile::IWFFC_NONE:
			return utils::compress::COMP_NONE;
		case fastfile::IWFFC_LZ4:
			return utils::compress::COMP_LZ4;
		case fastfile::IWFFC_LZ4_HC:
			return utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_ZLIB_SIZE:
		case IWFFC_ZLIB_SPEED:
		case IWFFC_HW_ZLIB_SIZE:
		case IWFFC_PS4_ZLIB_SIZE:
			return utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_HW_ZLIB_SPEED:
		case IWFFC_PS4_ZLIB_SPREED:
			return utils::compress::COMP_ZLIB;
		case IWFFC_OODLE_KRAKEN_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_KRAKEN_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN;
		case IWFFC_OODLE_LEVIATHAN_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LEVIATHAN | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_LEVIATHAN_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LEVIATHAN;
		case IWFFC_OODLE_MERMAID_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_MERMAID_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID;
		case IWFFC_OODLE_SELKIE_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_SELKIE_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE;
		case IWFFC_ZSTD_SIZE:
			return utils::compress::COMP_ZSTD | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_ZSTD_SPEED:
			return utils::compress::COMP_ZSTD;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
	}

	FastFileLinkerOption::~FastFileLinkerOption() {}
	bool FastFileLinkerOption::Compute(const char** args, size_t startIndex, size_t endIndex) {
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
			else if (!strcmp("-c", arg) || !_strcmpi("--compressor", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* id{ args[++i] };
				if (!(compressor = FindCompressor(id))) {
					LOG_ERROR("Can't find compressor for name '{}'", id);
					return false;
				}
			}
			else if (!strcmp("-l", arg) || !_strcmpi("--linker", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* id{ args[++i] };
				if (!(linker = FindLinker(id))) {
					LOG_ERROR("Can't find linker for name '{}'", id);
					return false;
				}
			}
			else if (!strcmp("-P", arg) || !_strcmpi("--platform", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* plt{ args[++i] };
				if ((platform = GetFastFilePlatform(plt)) == XFILE_PLATFORM_COUNT) {
					LOG_ERROR("Can't find handler for name '{}'", plt);
					return false;
				}
			}
			else if (!strcmp("-n", arg) || !_strcmpi("--name", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				ffname = args[++i];
			}
			else if (!strcmp("-p", arg) || !_strcmpi("--patch", arg)) {
				m_fd = true;
			}
			else if (!strcmp("-L", arg) || !_strcmpi("--linkers", arg)) {
				printLinkers = true;
			}
			else if (!strcmp("-C", arg) || !_strcmpi("--compressors", arg)) {
				printCompressors = true;
			}
			else if (!_strcmpi("--server", arg)) {
				server = true;
			}
			else if (!_strcmpi("--chunkSize", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				chunkSize = (size_t)utils::ParseFormatInt(args[++i]);
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

	void FastFileLinkerOption::PrintHelp() {
		LOG_INFO("-h --help             : Print help");
		LOG_INFO("-o --output [dir]     : Output dir");
		LOG_INFO("-p --patch            : Use patch files (fd/fp)");
		LOG_INFO("-t --compression      : Compression type");
		LOG_INFO("--server              : Server fastfile");
		LOG_INFO("-n --name [name]      : Fast file name, by default the intput name");
		LOG_INFO("-P --platform         : Platform type (pc, xbox, playstation, dev)");
		LOG_INFO("-l --linker           : Linker to use");
		LOG_INFO("-L --linkers          : Print linkers");
		LOG_INFO("-c --compressor       : Compressor to use");
		LOG_INFO("-C --compressors      : Print compressors");
		LOG_INFO("--chunkSize [size]    : Set chunk sizes");
		LOG_INFO("-H --high-compression : Use high compression (if available)");
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
			else if (!strcmp("-a", arg) || !_strcmpi("--assets", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				assetTypes = args[++i];
			}
			else if (!_strcmpi("--patch", arg) || !strcmp("-p", arg)) {
				m_fd = true;
			}
			else if (!strcmp("-R", arg) || !_strcmpi("--handlers", arg)) {
				print_handlers = true;
			}
			else if (!strcmp("-w", arg) || !_strcmpi("--wildcard", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				wildcard = args[++i];
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
			else if (!_strcmpi("--dumpXStrings", arg)) {
				dumpXStrings = true;
			}
			else if (!_strcmpi("--dumpBinaryAssets", arg)) {
				dumpBinaryAssets = true;
			}
			else if (!_strcmpi("--dumpAssetNames", arg)) {
				dumpAssetNames = true;
			}
			else if (!_strcmpi("--testDump", arg)) {
				testDump = true;
			}
			else if (!_strcmpi("--alpha", arg)) {
				alpha = true;
			}
			else if (!_strcmpi("--assertContainer", arg)) {
				assertContainer = true;
			}
			else if (!_strcmpi("--archiveDDL", arg)) {
				archiveDDL = true;
			}
			else if (!_strcmpi("--dumpBinaryAssetsMap", arg)) {
				dumpBinaryAssetsMap = true;
			}
			else if (!_strcmpi("--disableScriptsDecomp", arg)) {
				disableScriptsDecomp = true;
			}
			else if (!strcmp("-i", arg) || !_strcmpi("--fd-ignore", arg)) {
				m_fdIgnoreMissing = true;
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
		LOG_INFO("-h --help              : Print help");
		LOG_INFO("-o --output [d]        : Output dir");
		LOG_INFO("-w --wildcard [wc]     : Wildcard to match for the filename");
		LOG_INFO("-H --header            : Dump header info");
		LOG_INFO("-r --handler           : Handler to use (use --handlers to print)");
		LOG_INFO("-R --handlers          : Print handlers");
		LOG_INFO("-D --decompressors     : Print decompressors");
		LOG_INFO("-d --dump              : Dump decompressed (if available)");
		LOG_INFO("-C --casc [c]          : Use casc db");
		LOG_INFO("-g --game [g]          : exe");
		LOG_INFO("-p --patch             : Use patch files (fd/fp)");
		LOG_INFO("-i --fd-ignore         : Ignore missing fd file");
		LOG_INFO("-a --assets [g]        : Set the assets to dump by name (by default all)");
		LOG_INFO("--noAssetDump          : No asset dump");
		LOG_INFO("--dumpBinaryAssets     : Dump binary assets");
		LOG_INFO("--dumpBinaryAssetsMap  : Dump binary assets map");
		LOG_INFO("--dumpAssetNames       : Dump binary assets");
		LOG_INFO("--disableScriptsDecomp : Disable GSC script decompilation");
		LOG_INFO("--dumpXStrings         : Dump XStrings");
		LOG_INFO("--assertContainer      : Use acts as a container for other software");
		LOG_INFO("--archiveDDL           : dump archived ddl (bo6)");
		LOG_INFO("--alpha                : Is alpha file");
		
	}

	hook::library::Library FastFileOption::GetGame(bool crashError, bool* init, bool needDecrypt) {
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

			if (needDecrypt && !acts::decryptutils::LoadDecryptModule(gameMod)) {
				throw std::runtime_error(std::format("Can't load {}: Can't find DecryptString", game));
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
	void FastFileOption::AddAssetName(int type, uint64_t name) {
		if (!dumpAssetNames) return;

		if (assetNames.size() <= type) {
			assetNames.resize(type + 10);
		}

		assetNames[type].insert(name);
	}

	void SetBP(void* ptr, byte len) {
		HwBp::Set(ptr, len, HwBp::When::Written);
	}

	struct FFLoadContext {
		std::vector<byte> buff{};
		std::vector<byte> ffdata{};
		size_t count{}, completed{};
		FFDecompressor* lastDecompressor{};
		byte pad[0x100];
	};

	int fastfile(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help || (opt.files.empty() && !(opt.print_handlers || opt.print_decompressors))) {
			opt.PrintHelp();
			if (opt.files.empty()) {
				LOG_ERROR("Missing entry");
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

		FFLoadContext cc{};

		utils::CloseEnd hce{ [&opt, &cc] {
			if (cc.lastDecompressor) cc.lastDecompressor->Cleanup();
			if (opt.handler) opt.handler->Cleanup();
		} };

		std::regex wildcard{ opt.wildcard ? opt.wildcard : ".*" };

		for (const char* f : opt.files) {
			for (std::string& filename : opt.GetFileRecurse(f)) {
				if (!filename.ends_with(".ff") && !filename.ends_with(".ff.zone")) {
					LOG_TRACE("Ignore {}", filename);
					continue;
				}
				std::filesystem::path flpname{ filename };
				flpname = flpname.filename();
				std::string rfilename{ flpname.string() };
				utils::MapString(rfilename.data(), [](char c) -> char {return c == '\\' ? '/' : c; });
				std::sregex_iterator rbegin{ rfilename.begin(), rfilename.end(), wildcard };

				if (rbegin == std::sregex_iterator() || rbegin->length() != rfilename.size()) {
					continue; // nothing else
				}

				cc.count++;

				if (!opt.ReadFile(filename.data(), cc.buff)) {
					LOG_ERROR("Can't read file {}", filename);
					continue;
				}

				core::bytebuffer::ByteBuffer reader{ cc.buff };

				if (!reader.CanRead(sizeof(uint64_t))) {
					LOG_ERROR("Can't read file {}: too small", filename);
					continue;
				}

				cc.ffdata.clear();
				uint64_t magic{ *reader.Ptr<uint64_t>() };

				try {
					fastfile::FastFileContext ctx{};
					ctx.file = filename.c_str();
					currentCtx = &ctx;
					FFDecompressor* handler{ FindDecompressor(filename, reader) };

					if (!handler) {
						LOG_ERROR("Can't open {}: Can't find decompressor for magic 0x{:x}", filename, magic);
						continue;
					}
					if (cc.lastDecompressor != handler) {
						if (cc.lastDecompressor) {
							cc.lastDecompressor->Cleanup();
							cc.lastDecompressor = handler;
							//SetBP(&lastDecompressor, 8);
						}
						else {
							cc.lastDecompressor = handler;
						}
						handler->Init(opt);
					}

					LOG_INFO("Loading {}... ({})", filename, handler->name);

					handler->LoadFastFile(opt, reader, ctx, cc.ffdata);

					LOG_TRACE("Decompressed 0x{:x} byte(s)", cc.ffdata.size());

					if (opt.dump_decompressed) {
						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / ctx.ffname };

						decfile.replace_extension(".ff.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, cc.ffdata.data(), cc.ffdata.size())) {
							LOG_ERROR("Can't dump {}", decfile.string());
						}
						else {
							LOG_INFO("Dump into {}", decfile.string());
						}
					}

					if (opt.handler) {
						core::bytebuffer::ByteBuffer ffreader{ cc.ffdata };
						LOG_TRACE("Reading using {}", opt.handler->name);
						opt.handler->Handle(opt, ffreader, ctx);
					}
					cc.completed++;
				}
				catch (std::runtime_error& err) {
					LOG_ERROR("Can't read {}: {}", filename, err.what());
				}
				currentCtx = nullptr;
			}
		}

		if (cc.lastDecompressor) {
			cc.lastDecompressor->Cleanup();
		}
		if (opt.handler) {
			opt.handler->Cleanup();
		}

		size_t errors{ cc.count - cc.completed };
		if (errors) {
			LOG_ERROR("Parsed {} (0x{:x}) file(s) with {} (0x{:x}) error(s)", cc.count, cc.count, errors, errors);
			return tool::BASIC_ERROR;
		}
		LOG_INFO("Parsed {} (0x{:x}) file(s)", cc.count, cc.count);

		return tool::OK;
	}

	void FastFileLinkerContext::ReadZoneFile() {
		std::string zoneFileData{ utils::ReadFile<std::string>(zoneFile) };
		zone.ParseFile(zoneFile, zoneFileData);

		if (!mainFFName) {
			mainFFName = zone.GetConfig("name");
		}
		const char* game{ zone.GetConfig("game") };

		if (game) {
			if (!linker) {
				linker = FindLinker(game);
			}
			if (!compressor) {
				compressor = FindCompressor(game);
			}
		}
	}
	void FastFileLinkerContext::RegisterHash(uint64_t val, const char* str) {
		if (!val) return;

		const char*& old{ storedHashes[val] };
		if (old) {
			if (!utils::EqualIgnoreCase(old, str)) {
				LOG_WARNING("Hash collision {} != {} for val 0x{:x}", old, str, val);
			}
		}

		old = strs.CloneStr(str);
	}

	int fastfilelinker(int argc, const char* argv[]) {
		FastFileLinkerOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help || (opt.files.empty() && !(opt.printCompressors || opt.printLinkers))) {
			opt.PrintHelp();
			if (opt.files.empty()) {
				LOG_ERROR("Missing fast file");
				return tool::BASIC_ERROR;
			}
			else {
				return tool::OK;
			}
		}


		if (opt.printCompressors || opt.printLinkers) {
			if (opt.printLinkers) {
				LOG_INFO("Linkers:");

				for (FFLinker* handler : GetLinkers()) {
					LOG_INFO("{:10} - {}", handler->name, handler->description);
				}
			}

			if (opt.printCompressors) {
				LOG_INFO("Compressors:");

				for (FFCompressor* handler : GetCompressors()) {
					LOG_INFO("{:10} - {}", handler->name, handler->description);
				}
			}
			return tool::OK;
		}

		for (const char* file : opt.files) {
			try {
				FastFileLinkerContext ctx{ opt, file };

#ifdef CI_BUILD
				ctx.zone.preProcOpt.AddDefine("CI");
#endif
				if (core::actsinfo::DEV_VERSION_ID == core::actsinfo::VERSION_ID) {
					ctx.zone.preProcOpt.AddDefine("ACTS_DEV");
				}

				ctx.ReadZoneFile();

				if (!ctx.linker) {
					LOG_ERROR("No linker for {}", file);
					continue;
				}

				ctx.linker->Init(opt);

				utils::CloseEnd linkerCE{ [&ctx]() {ctx.linker->Cleanup(); } };


				if (ctx.compressor) {
					ctx.compressor->Init(opt);
				}

				utils::CloseEnd compCE{ [&ctx]() { if (ctx.compressor) ctx.compressor->Cleanup(); } };
				
				ctx.linker->Link(ctx);

				if (ctx.compressor) {
					ctx.compressor->Compress(ctx);
				}
			}
			catch (std::runtime_error& err) {
				LOG_ERROR("Can't read {}: {}", file, err.what());
			}

		}


		return tool::OK;
	}

	ADD_TOOL(fastfile, "common", "", "fastfile reader", fastfile);
	ADD_TOOL(fastfilelinker, "common", "", "fastfile linker", fastfilelinker);

}