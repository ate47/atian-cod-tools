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
#include <core/config.hpp>
#include <cli/clicolor.hpp>
#include <acts_api/event.h>
#include <xxhash.h>


namespace fastfile {
	class AssetPool;

	void RegisterFastfileData() {
		static std::once_flag of{};
		std::call_once(of, []() {
			ActsAPIEvent_RegisterFastFileData data{};
			ActsAPIEvent_TriggerEvent(EVT_REGISTER_FASTFILE, &data);
		});
	}

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
		RegisterFastfileData();
		for (FFDecompressor* handler : GetDecompressors()) {
			if (handler->MatchFile(path, data)) {
				return handler;
			}
		}
		return nullptr;
	}

	FFHandler* FindHandler(const char* name) {
		RegisterFastfileData();
		for (FFHandler* handler : GetHandlers()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	FFCompressor* FindCompressor(const char* name) {
		RegisterFastfileData();
		for (FFCompressor* handler : GetCompressors()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	FFLinker* FindLinker(const char* name) {
		RegisterFastfileData();
		for (FFLinker* handler : GetLinkers()) {
			if (!_strcmpi(name, handler->name)) {
				return handler;
			}
		}
		return nullptr;
	}

	namespace {
		const char* ffPltNames[]{
			"pc",
			"xbox",
			"playstation",
			"dev",
		};
		const char* gameIdNames[]{
			"invalid",
			"bo3",
			"iw",
			"wwii",
			"bo4",
			"mw19",
			"cw",
			"vg",
			"mwii",
			"mwiii",
			"bo6",
			"bo7",
		};

		const char* gameRevIdNames[]{
			"default",
			"bo6_pre_alpha",
		};

		void RegisterCrypts() {
			int r;
			if ((r = register_hash(&sha256_desc)) != CRYPT_OK) throw std::runtime_error(std::format("Can't register sha256_desc {}", error_to_string(r)));
			if ((r = register_cipher(&aes_desc)) != CRYPT_OK) throw std::runtime_error(std::format("Can't register aes_desc {}", error_to_string(r)));
			if ((r = crypt_mp_init("ltm")) != CRYPT_OK) throw std::runtime_error(std::format("Can't init ltm {}", error_to_string(r)));

		}
		

		fastfile::FastFileContext* currentCtx{};
		fastfile::FastFileOption* currentOpt{};
		AssetPool* currentAssetPool{};
	}

	fastfile::FastFileContext& GetCurrentContext() {
		return *currentCtx;
	}

	fastfile::FastFileOption& GetCurrentOptions() {
		return *currentOpt;
	}

	FastFilePlatform GetFastFilePlatform(const char* name) {
		for (size_t i = 0; i < ACTS_ARRAYSIZE(ffPltNames); i++) {
			if (!_strcmpi(ffPltNames[i], name)) {
				return (FastFilePlatform)i;
			}
		}
		return FastFilePlatform::XFILE_PLATFORM_COUNT;
	}

	const char* GetFastFilePlatformName(FastFilePlatform plt) {
		if (plt >= ACTS_ARRAYSIZE(ffPltNames) || plt < 0) {
			return "unknown";
		}
		return ffPltNames[plt];
	}

	const char* GetGameIdName(GameId plt) {
		if (plt >= ACTS_ARRAYSIZE(gameIdNames) || plt < 0) {
			return "unknown";
		}
		return gameIdNames[plt];
	}

	const char* GetGameRevIdName(GameId plt) {
		if (plt >= ACTS_ARRAYSIZE(gameRevIdNames) || plt < 0) {
			return "unknown";
		}
		return gameRevIdNames[plt];
	}

	GameId GetGameId(const char* name) {
		for (size_t i = 0; i < ACTS_ARRAYSIZE(gameIdNames); i++) {
			if (!_strcmpi(gameIdNames[i], name)) {
				return (GameId)i;
			}
		}
		return GameId::GID_INVALID;
	}

	GameRevId GetGameRevId(const char* name) {
		for (size_t i = 0; i < ACTS_ARRAYSIZE(gameRevIdNames); i++) {
			if (!_strcmpi(gameRevIdNames[i], name)) {
				return (GameRevId)i;
			}
		}
		return GameRevId::GRID_DEFAULT;
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
			else if (!strcmp("-e", arg) || !_strcmpi("--encrypt", arg)) {
				encrypt = true;
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
		LOG_INFO("-e --encrypt          : Encrypt fastfile, requires AES key");
		LOG_INFO("--chunkSize [size]    : Set chunk sizes");
		LOG_INFO("-H --high-compression : Use high compression (if available)");
	}


	FastFileOption::FastFileOption() : ActsAPIFastFile_FastFileOption() {
		gscFormatter = &tool::gsc::formatter::GetDefaultFormatter();
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
			else if (!strcmp("-G", arg) || !_strcmpi("--game-path", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				gamePath = args[++i];

				// we try to open a casc storage
				if (!CascOpenStorage(gamePath, 0, &cascStorage)) {
					m_casc = gamePath;
					cascStorage = nullptr;
				}
			}
			else if (!strcmp("-C", arg) || !_strcmpi("--casc", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				m_casc = args[++i];
				gamePath = m_casc;

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
			else if (!strcmp("-W", arg) || !_strcmpi("--ignore-wildcard", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				ignoreWildcard = args[++i];
			}
			else if (!strcmp("-I", arg) || !_strcmpi("--ignore", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				ignore = args[++i];
			}
			else if (!strcmp("-n", arg) || !_strcmpi("--name", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				assets = args[++i];
			}
			else if (!strcmp("-t", arg) || !_strcmpi("--translate", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				translation = args[++i];
				LoadTranslationKeys();
			}
			else if (!strcmp("-k", arg) || !_strcmpi("--rsa-key", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				rsaKey = args[++i];
			}
			else if (!strcmp("-D", arg) || !_strcmpi("--decompressors", arg)) {
				print_decompressors = true;
			}
			else if (!strcmp("-l", arg) || !_strcmpi("--reduced-logs", arg)) {
				reducedLogs = true;
			}
			else if (!_strcmpi("--noWorkerPreload", arg)) {
				noWorkerPreload = true;
			}
			else if (!strcmp("-d", arg) || !_strcmpi("--dump", arg)) {
				dump_decompressed = true;
			}
			else if (!_strcmpi("--header", arg) || !strcmp("-H", arg)) {
				m_header = true;
			}
			else if (!_strcmpi("--headerDump", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				headerDump = args[++i];
			}
			else if (!_strcmpi("--noAssetDump", arg)) {
				noAssetDump = true;
			}
			else if (!_strcmpi("--dumpXStrings", arg)) {
				dumpXStrings = true;
			}
			else if (!_strcmpi("--dumpXHash", arg)) {
				dumpXHash = true;
			}
			else if (!_strcmpi("--dumpBinaryAssets", arg)) {
				dumpBinaryAssets = true;
			}
			else if (!_strcmpi("--dumpAssetNames", arg)) {
				dumpAssetNames = true;
			}
			else if (!_strcmpi("--dumpCompiledZone", arg)) {
				dumpCompiledZone = true;
			}
			else if (!_strcmpi("--graphic", arg)) {
				graphic = true;
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
			else if (!_strcmpi("--scriptsFormatter", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}

				const tool::gsc::formatter::FormatterInfo* fmt{ tool::gsc::formatter::GetFromName(args[++i]) };

				if (!fmt) {
					LOG_ERROR("Unknown gsc formatter: {}!", args[i]);
					return false;
				}

				gscFormatter = fmt;
			}
			else if (!_strcmpi("--disableScriptsDecomp", arg)) {
				disableScriptsDecomp = true;
			}
			else if (!_strcmpi("--print-gameid", arg)) {
				print_gameId = true;
			}
			else if (!_strcmpi("--print-gamerevid", arg)) {
				print_revId = true;
			}
			else if (!_strcmpi("--gameId", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* gid{ args[++i] };
				if (!(m_gameId = GetGameId(gid))) {
					LOG_ERROR("Can't find game id for name '{}'", gid);
					return false;
				}
			}
			else if (!_strcmpi("--gameRevId", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				const char* grid{ args[++i] };
				if (!(m_gameRevId = GetGameRevId(grid))) {
					LOG_ERROR("Can't find game rev id for name '{}'", grid);
					return false;
				}
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
		LOG_INFO("some tools might not be available for each handler");
		LOG_INFO("");
		LOG_INFO("-h --help                 : Print help");
		LOG_INFO("-o --output [d]           : Output dir");
		LOG_INFO("-w --wildcard [wc]        : Wildcard to match for the filename");
		LOG_INFO("-W --ignore-wildcard [wc] : Wildcard to match for the filename to ignore");
		LOG_INFO("-I --ignore [path]        : Ignore a path");
		LOG_INFO("-H --header               : Dump header info");
		LOG_INFO("--headerDump [d]          : Dump header info into a directory");
		LOG_INFO("-r --handler              : Handler to use (use --handlers to print)");
		LOG_INFO("-R --handlers             : Print handlers");
		LOG_INFO("-D --decompressors        : Print decompressors");
		LOG_INFO("-d --dump                 : Dump decompressed");
		LOG_INFO("-C --casc [c]             : Use casc db");
		LOG_INFO("-g --game [g]             : exe");
		LOG_INFO("-G --game-path [g]        : Game path");
		LOG_INFO("-p --patch                : Use patch files (fd/fp)");
		LOG_INFO("-i --fd-ignore            : Ignore missing fd file");
		LOG_INFO("-a --assets [g]           : Set the asset types to dump by name (by default all)");
		LOG_INFO("-n --name [n]             : Set the assets to dump by name (by default all)");
		LOG_INFO("-t --translate [t]        : Load translation directory");
		LOG_INFO("-k --rsa-key [k]          : Set the rsa public key (by default game's key)");
		LOG_INFO("-l --reduced-logs         : Reduce the logs");
		LOG_INFO("--noAssetDump             : No asset dump");
		LOG_INFO("--dumpBinaryAssets        : Dump binary assets");
		LOG_INFO("--dumpBinaryAssetsMap     : Dump binary assets map");
		LOG_INFO("--dumpAssetNames          : Dump binary assets");
		LOG_INFO("--dumpCompiledZone        : Dump compiled zone file");
		LOG_INFO("--scriptsFormatter [f]    : Set formatter for GSC decompiler");
		LOG_INFO("--disableScriptsDecomp    : Disable GSC script decompilation");
		LOG_INFO("--noWorkerPreload         : Disable worker preload");
		LOG_INFO("--dumpXStrings            : Dump XStrings");
		LOG_INFO("--graphic                 : Dump graphic assets (do not share graphic assets)");
		LOG_INFO("--assertContainer         : Use acts as a container for other software");
		LOG_INFO("--archiveDDL              : dump archived ddl (bo6)");
		LOG_INFO("--alpha                   : Is alpha file");
		LOG_INFO("--print-gameid            : Print game ids list");
		LOG_INFO("--print-gamerevid         : Print game rev ids list");
		LOG_INFO("--gameId [g]              : Game id (might be required by some handlers)");
		LOG_INFO("--gameRevId [g]           : Game rev id (might be required by some handlers)");
		
	}

	hook::module_mapper::Module& FastFileOption::GetGameModule(bool crashError, bool* init, bool needDecrypt, const char* defaultName, const char* dumperName) {
		if (init) *init = false;

		std::filesystem::path exe;
		if (game) {
			exe = game;
		}
		else if (defaultName && *defaultName) {
			exe = utils::GetProgDir() / "deps" / defaultName;
		}
		else {
			if (crashError) {
				throw std::runtime_error("No game module specified");
			}
			gameMod.Free();
			return gameMod;
		}

		if (!gameMod) {
			if (!gameMod.Load(exe)) {
				if (dumperName && *dumperName) {
					throw std::runtime_error(std::format("Can't load {}, you can dump the exe using 'acts game_dump {} path/to/game'", exe.string(), dumperName));
				}
				else {
					throw std::runtime_error(std::format("Can't load {}", exe.string()));
				}
			}

			if (needDecrypt && !acts::decryptutils::LoadDecryptModule(gameMod)) {
				throw std::runtime_error(std::format("Can't load {}: Can't find DecryptString", exe.string()));
			}

			if (init) *init = true;
		}

		return gameMod;
	}
	hook::module_mapper::Module& FastFileOption::GetGameModule() {
		if (!gameMod) {
			throw std::runtime_error("no game module loaded for context");
		}
		return gameMod;
	}

	size_t CountDiv(const char* name) {
		size_t c{};
		for (; *name; name++) {
			if (*name == '_') c++;
		}
		return c;
	}

	const char* FastFileContext::GetFFType() {
		return ActsAPIFastFile_FastFileContext_GetType(this);
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
		else if (gamePath) {
			std::filesystem::path root{ gamePath };

			std::vector<std::filesystem::path> files{};
			utils::GetFileRecurse(path[0] ? root / path : root, files);

			for (const std::filesystem::path& file : files) {
				res.push_back(file.string());
			}
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
				return false; // nothing to read
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

		return utils::ReadFile(path, buff) && !buff.empty();
	}
	const char* FastFileOption::GetTranslation(uint64_t key) {
		if (!(key & hash::MASK60)) {
			return "";
		}

		auto it{ translationKeys.find(key & hash::MASK60) };

		if (it == translationKeys.end()) {
			return hashutils::ExtractTmp("hash", key);
		}

		return it->second;
	}
	const char* FastFileOption::GetTranslation(const char* key) {
		auto it{ translationKeys.find(hash::HashIWAsset(key) & hash::MASK60)};

		if (it == translationKeys.end()) {
			return key;
		}

		return it->second;
	}
	void FastFileOption::LoadTranslationKeys() {
		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurseExt(translation, paths, ".json\0");

		size_t loaded{};
		for (const std::filesystem::path& path : paths) {
			std::string json{};
			LOG_TRACE("read file {}", path.string());

			if (!utils::ReadFile(path, json)) {
				LOG_WARNING("Error when loading translation {}: can't read data", path.string());
				continue;
			}
			rapidjson::Document main;

			main.Parse(json.data());
			if (main.HasParseError()) {
				LOG_WARNING("Error when loading translation {}: can't parse data: {}", path.string(), (int)main.GetParseError());
				continue;
			}
			


			for (auto it = main.MemberBegin(); it != main.MemberEnd(); it++) {
				
				const char* str{ it->name.GetString() };
				if (!it->value.IsString()) {
					continue; // invalid
				}
				const char* all{ alloc.CloneStr(it->value.GetString()) };
				uint64_t hash;
				if (hash::TryHashPattern(str, hash)) {
					LOG_TRACE("{} {}", hash & hash::MASK60, all);
					translationKeys[hash & hash::MASK60] = all;
				}
				else {
					translationKeys[hash::HashIWAsset(str) & hash::MASK60] = all;
					translationKeys[hash::HashX64(str) & hash::MASK60] = all;
				}
				loaded++;
			}

		}
		LOG_INFO("loaded {} translation key(s)", loaded);
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


	using namespace compatibility::scobalula::csi;

	class AssetPool {
		FastFileOption& opt;
	public:
		core::memory_allocator::MemoryAllocator allocator{};
		CordycepGame game{};
		std::vector<XAssetPool64Mem> pools{};
		size_t numAssets{};
		std::vector<char> poolStrings{};
		std::unordered_map<uint64_t, uint32_t> poolStringsOffset{};
		size_t poolStringsFree{ 1 }; // 0 = null

		AssetPool(FastFileOption& opt) : opt(opt) {
			// alloc some pools
			GetAssetPool(512);
			poolStrings.resize(2);
		}

		XAssetPool64Mem* GetAssetPool(size_t id) {
			if (pools.size() <= id) {
				pools.resize(id * 3 / 2);
			}
			return &pools[id];
		}

		uint32_t AddScrString(const char* string) {
			if (!string) {
				return 0;
			}

			size_t stringLen{ std::strlen(string) + 1 };
			uint64_t hash{ XXH64(string, stringLen, hash::FNV1A_PRIME) };
			uint32_t& offset{ poolStringsOffset[hash] };

			if (!offset) {
				size_t newSize = utils::Aligned(poolStringsFree + stringLen, 0x100000);
				if (poolStrings.size() < newSize) {
					// we update the size and write the new csi data
					poolStrings.resize(newSize);
				}

				offset = (uint32_t)poolStringsFree;

				// we need to write the string
				char* alloc{ &poolStrings[poolStringsFree] };
				std::memcpy(alloc, string, stringLen);
				poolStringsFree += stringLen;
			}
			return offset;
		}

		const char* GetScrString(uint32_t id) {
			if ((int)id >= poolStringsFree) {
				LOG_WARNING("Can't get scr string: {} >= {}", id, poolStringsFree);
				return utils::va("<invalid:0x%x>", id);
			}
			return &poolStrings[id];
		}

		void AddAssetHeader(uint64_t name, void* header, uint32_t type, size_t size) {
			XAssetPool64Mem* pool{ GetAssetPool(type) };

			XAsset64Mem* entry{ allocator.Alloc<XAsset64Mem>() };
			entry->id = name;
			entry->type = type;
			entry->headerSize = size;
			entry->header = header;
			entry->temp = false;
			entry->previous = pool->end;
			entry->next = nullptr;

			// link the asset in the pool
			if (!pool->root) {
				pool->root = entry;
			}
			if (pool->end) {
				pool->end->next = entry;
			}
			pool->end = entry;
			numAssets++;
		}

		void WriteCsiFile() const {
			std::filesystem::path dataPath{ utils::GetProgDir() / "Data" };
			std::filesystem::path csiPath{ dataPath / "CurrentHandler.csi" };
			std::filesystem::path jsonPath{ dataPath / "CurrentHandler.json" };
			if (!opt.gamePath) {
				throw std::runtime_error("Missing game path parameter");
			}
			std::filesystem::create_directories(dataPath);
			compatibility::scobalula::csi::CordycepProc proc{};
			proc.gameId = game;
			proc.poolsAddress = reinterpret_cast<uintptr_t>(pools.data());
			proc.stringsAddress = reinterpret_cast<uintptr_t>(poolStrings.data());
			proc.stringSize = (uint32_t)poolStrings.size();
			proc.gameDir = opt.gamePath;

			if (!proc.WriteCsi(csiPath)) {
				throw std::runtime_error(std::format("Error when writing {}", csiPath.string()));
			}

			core::config::Config cfg{ jsonPath };

			cfg.SetInteger("pid", GetCurrentProcessId());
			cfg.SetInteger("game_id", game);
			cfg.SetString("game_dir", opt.gamePath);
			cfg.SetString("game_module_path", opt.GetGameModule().GetLibrary().GetPath());
			cfg.SetInteger("pools_addr", reinterpret_cast<uintptr_t>(pools.data()));
			cfg.SetInteger("strings_addr", reinterpret_cast<uintptr_t>(poolStrings.data()));
			cfg.SetInteger("str_pool_size_addr", poolStrings.size());

			rapidjson::Value flagsVal{ rapidjson::kStringType };
			flagsVal.SetString("flags", cfg.main.GetAllocator());
			rapidjson::Value flag{ rapidjson::kStringType };
			flag.SetString("default", cfg.main.GetAllocator());
			rapidjson::Value flagsArr{ rapidjson::kArrayType };
			flagsArr.SetArray();

			flagsArr.PushBack(flag, cfg.main.GetAllocator());

			cfg.main.AddMember(flagsVal, flagsArr, cfg.main.GetAllocator());



			cfg.SaveConfig();
		}
	};

	struct FFReplCmd {
		const char* usage;
		const char* description;
		std::function<void(int argc, const char* argv[])> func{};
	};

	class FFLoadContext {
	public:
		FastFileOption& opt;
		AssetPool assetPool;
		std::vector<byte> buff{};
		std::vector<byte> ffdata{};
		size_t count{}, completed{};
		FFDecompressor* lastDecompressor{};
		byte pad[0x100]{};
		std::unordered_map<std::string, FFReplCmd> cmds{};
		bool runRepl{};

		FFLoadContext(FastFileOption& opt) : opt(opt), assetPool(opt) {
			cmds["exit"] = { "", "exit the process",
				[this](int argc, const char* argv[]) {
					runRepl = false;
				}
			};
			cmds["help"] = { "", "show help",
				[this](int argc, const char* argv[]) {
					LOG_INFO("commands:");
					for (auto& [name, data] : cmds) {
						LOG_INFO("{}{}: {}", name, data.usage, data.description);
					}
				}
			};
			cmds["load"] = { " [filename]", "load fastfile",
				[this](int argc, const char* argv[]) {
					if (tool::NotEnoughParam(argc, 1)) {
						LOG_ERROR("not enough param");
					}
					for (size_t i = 2; i < argc; i++) {
						if (!LoadFastFile(argv[i])) {
							LOG_ERROR("Nothing loaded for {}", argv[i]);
						}
					}
					WriteIndex();
				}
			};
			cmds["loadre"] = { " [wildcard] (ignorewc)", "load fastfile using regex",
				[this](int argc, const char* argv[]) {
					if (tool::NotEnoughParam(argc, 1)) {
						LOG_ERROR("not enough param");
						return;
					}
					if (!LoadFastFile("", argv[2], tool::NotEnoughParam(argc, 1) ? nullptr : argv[3])) {
						LOG_ERROR("Nothing loaded for {}", argv[2]);
					}
					WriteIndex();
				}
			};
			cmds["loadcommonfiles"] = { "", "load common fastfiles",
				[this](int argc, const char* argv[]) {
					if (!this->opt.handler || this->opt.handler->commonFiles.empty()) {
						LOG_ERROR("no common files registered for this game");
						return;
					}
					for (std::string& cf : this->opt.handler->commonFiles) {
						if (!LoadFastFile(cf.data())) {
							LOG_ERROR("Nothing loaded for {}", argv[2]);
						}
					}
					WriteIndex();
				}
			};
		}

		~FFLoadContext() {
			if (lastDecompressor) lastDecompressor->Cleanup();
			if (opt.handler) opt.handler->Cleanup();
		}

		void Init() {
			if (opt.workflow == FFW_ASSET_POOL) {
				// ignore the asset loading
				opt.assetTypes = "$assetpool_ignore";
				opt.reducedLogs = true; // hide the asset and strings dump
				currentAssetPool = &assetPool;
			}
			else {
				currentAssetPool = nullptr;
			}

			if (opt.handler) {
				opt.handler->Init(opt);
				assetPool.game = opt.handler->game;

				if (!opt.files.empty() && !opt.m_fd && !opt.handler->noPatchOk) {
					LOG_WARNING("-p option not specified, it might cause an error with patched fastfiles");
				}
			}

			RegisterCrypts();
		}

		void UpdateDecompressor(FFDecompressor* decompressor) {
			if (lastDecompressor != decompressor) {
				if (lastDecompressor) {
					lastDecompressor->Cleanup();
					lastDecompressor = decompressor;
					//SetBP(&lastDecompressor, 8);
				}
				else {
					lastDecompressor = decompressor;
				}
				decompressor->Init(opt);
			}
		}

		void WriteIndex() {
			LOG_INFO("loaded asset count: {}", assetPool.numAssets);
			assetPool.WriteCsiFile();
		}

		void StartRepl() {
			runRepl = true;
			do {
				std::string line{};

				if (cli::clicolor::ConsoleAllowColor()) {
					std::cout << cli::clicolor::Color(5, 1, 3) << "acts:" << core::actsinfo::VERSION << "> " << cli::clicolor::Color(5, 5, 5);
					std::getline(std::cin, line);
					std::cout << cli::clicolor::Reset();
				}
				else {
					std::cout << "acts:" << core::actsinfo::VERSION << "> ";
					std::getline(std::cin, line);
				}

				LOG_TRACE("Run line: {}", line);

				int params{};
				const char** cmds = tool::ReadParams(line, params);

				if (!cmds || params < 2) {
					continue;
				}

				auto it{ this->cmds.find(cmds[1]) };

				if (it == this->cmds.end()) {
					LOG_ERROR("Invalid command: '{}', type help for the list of commands", cmds[1]);
					continue;
				}

				it->second.func(params, cmds);
			} while (runRepl);
		}

		bool LoadFastFile(const char* f, const char* fWildcard = nullptr, const char* iWildcard = nullptr) {
			std::regex wildcard{ fWildcard ? fWildcard : ".*" };
			std::regex ignoreWildcard{ iWildcard ? iWildcard : "." };

			bool anyLoaded{};
			for (std::string& filename : opt.GetFileRecurse(f)) {
				if (!filename.ends_with(".ff") && !filename.ends_with(".ff.zone")) {
					LOG_TRACE("Ignore {}", filename);
					continue;
				}
				std::filesystem::path flpname{ filename };
				flpname = flpname.filename();
				std::string rfilename{ flpname.string() };
				utils::MapString(rfilename.data(), [](char c) -> char {return c == '\\' ? '/' : c; });
				if (fWildcard) {
					std::sregex_iterator rbegin{ rfilename.begin(), rfilename.end(), wildcard };

					if (rbegin == std::sregex_iterator() || rbegin->length() != rfilename.size()) {
						continue; // doesn't match, we ignore it
					}
				}
				if (iWildcard) {
					std::sregex_iterator rbegin{ rfilename.begin(), rfilename.end(), ignoreWildcard };

					if (rbegin != std::sregex_iterator() && rbegin->length() == rfilename.size()) {
						continue; // match, we ignore it
					}
				}

				if (opt.ignore) {
					std::filesystem::path ffpath{ filename };
					std::filesystem::path ignoreDir{ opt.ignore };

					if (utils::IsSubDir(ignoreDir, ffpath)) {
						continue; // ignore sub paths
					}
				}

				count++;
				anyLoaded = true;

				if (!opt.ReadFile(filename.data(), buff)) {
					LOG_ERROR("Can't read file {}", filename);
					continue;
				}

				core::bytebuffer::ByteBuffer reader{ buff };

				if (!reader.CanRead(sizeof(uint64_t))) {
					LOG_ERROR("Can't read file {}: too small", filename);
					continue;
				}

				ffdata.clear();
				uint64_t magic{ *reader.Ptr<uint64_t>() };

				try {
					// we use a temporary memory block for reader because we don't need the memory after reading
					// for the pools it needs to be static
					core::memory_allocator::MemoryAllocator ffMemory{};
					fastfile::FastFileContext ctx{ .zoneMemory=opt.workflow == FFW_ASSET_POOL ? assetPool.allocator : ffMemory };
					ctx.file = filename.c_str();
					currentCtx = &ctx;
					FFDecompressor* decompressor{ FindDecompressor(filename, reader) };

					if (!decompressor) {
						LOG_ERROR("Can't open {}: Can't find decompressor for magic 0x{:x}", filename, magic);
						continue;
					}
					UpdateDecompressor(decompressor);


					std::filesystem::path ffnamet{ filename };
					ffnamet.replace_extension();
					ffnamet = ffnamet.filename();
					std::string ffnamets{ ffnamet.string() };
					sprintf_s(ctx.ffname, "%s", ffnamets.data());

					hashutils::Add(ctx.ffname, true, true);

					LOG_INFO("Loading {}... ({})", filename, decompressor->name);

					decompressor->LoadFastFile(opt, reader, ctx, ffdata);

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
				currentCtx = nullptr;
			}

			return anyLoaded;
		}
	};

	void AddAssetHeader(const char* name, void* header, uint32_t type, size_t size) {
		AddAssetHeader(hash::Hash64A(name), header, type, size);
	}
	void AddAssetHeader(uint64_t name, void* header, uint32_t type, size_t size) {
		if (header && currentAssetPool) {
			currentAssetPool->AddAssetHeader(name, header, type, size);
		}
	}
	const char* GetScrString(uint32_t id) {
		if (currentAssetPool) {
			return currentAssetPool->GetScrString(id);
		}
		if (!currentCtx) {
			throw std::runtime_error("missing context");
		}
		if ((int)id >= currentCtx->scrStringsCount) {
			LOG_WARNING("Can't get scr string: {} >= {}", id, currentCtx->scrStringsCount);
			return utils::va("<invalid:0x%x>", id);
		}
		const char* c{ currentCtx->scrStrings[id] };
		if (!c) return "";
		return c;
	}

	uint32_t RegisterScrString(const char* str, uint32_t prevId) {
		if (currentAssetPool) {
			return currentAssetPool->AddScrString(str);
		}
		return prevId;
	}

	bool PrintAny(FastFileOption& opt) {
		if (!(opt.print_handlers || opt.print_decompressors || opt.print_gameId || opt.print_revId)) {
			return false;
		}

		if (opt.print_handlers) {
			LOG_INFO("Handlers:");

			std::vector<FFHandler*>& vec{ GetHandlers() };

			std::sort(vec.begin(), vec.end(), [](FFHandler* a, FFHandler* b) {
				return _strcmpi(a->description, b->description) < 0;
				});

			for (FFHandler* handler : GetHandlers()) {
				LOG_INFO("{:10} - {}", handler->name, handler->description);
			}
		}

		if (opt.print_decompressors) {
			LOG_INFO("Decompressors:");
			std::vector<FFDecompressor*>& vec{ GetDecompressors() };

			std::sort(vec.begin(), vec.end(), [](FFDecompressor* a, FFDecompressor* b) {
				return _strcmpi(a->name, b->name) < 0;
				});

			for (FFDecompressor* decompr : GetDecompressors()) {
				LOG_INFO("{:20}", decompr->name);
			}
		}

		if (opt.print_gameId) {
			LOG_INFO("Game Ids:");

			for (const char* name : gameIdNames) {
				LOG_INFO("- {}", name);
			}
		}

		if (opt.print_revId) {
			LOG_INFO("Game rev Ids:");

			for (const char* name : gameRevIdNames) {
				LOG_INFO("- {}", name);
			}
		}

		return true;
	}

	int FastfileLoader(int argc, const char* argv[], FastfileWorkflow workflow) {
		FastFileOption opt{};
		opt.workflow = workflow;
		currentOpt = &opt;

		bool anyPrint{};
		if (!opt.Compute(argv, 2, argc) || opt.m_help || (opt.files.empty() && !((anyPrint = PrintAny(opt)) || workflow == FFW_ASSET_POOL))) {
			opt.PrintHelp();
			if (opt.files.empty() && !opt.handler) {
				LOG_ERROR("Missing entry");
				return tool::BASIC_ERROR;
			}
			else {
				return tool::OK;
			}
		} else if (anyPrint) {
			return tool::OK;
		}

		FFLoadContext cc{ opt };

		cc.Init();

		for (const char* f : opt.files) {
			cc.LoadFastFile(f, opt.wildcard, opt.ignoreWildcard);
		}

		if (workflow == FFW_ASSET_POOL) {
			cc.WriteIndex();
			cc.StartRepl();
		}

		currentOpt = nullptr;

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
		RegisterCrypts();

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

	int fastfile(int argc, const char* argv[]) {
		return FastfileLoader(argc, argv, FFW_READER);
	}

	constexpr const char* CORDYCEP_EXE = "Cordycep.CLI.exe";

	int fastfilecordycep(int argc, const char* argv[]) {
		wchar_t szFileName[MAX_PATH];
		GetModuleFileNameW(NULL, szFileName, MAX_PATH);
		std::filesystem::path exePath{ szFileName };
		exePath = exePath.filename();
		if (exePath.string() != CORDYCEP_EXE) {
			LOG_WARNING("** WARNING **********************");
			LOG_WARNING("Invalid exe name: {}", exePath.string(), CORDYCEP_EXE);
			LOG_WARNING("please use {} to run this tool", CORDYCEP_EXE);
			LOG_WARNING("*********************************");
		}

		LOG_WARNING(
			"This tool is a prototype to test Cordycep compatible tools with "
			"the Atian Tools fastfile loaders. It might not reflect 100% of "
			"Cordycep's abilities."
		);

		return FastfileLoader(argc, argv, FFW_ASSET_POOL);
	}

	ADD_TOOL(fastfile, "common", "", "fastfile reader", fastfile);
	ADD_TOOL(fastfilelinker, "common", "", "fastfile linker", fastfilelinker);
	ADD_TOOL(fastfilecordycep, "internal", "", "internal tool for the cordycep acts", fastfilecordycep);
}

const char* ActsAPIFastFile_FastFileContext_GetType(ActsAPIFastFile_FastFileContext* context) {
	if (!context->fftype[0]) {
		std::string_view ffv{ context->ffname };
		size_t div{ fastfile::CountDiv(context->ffname) };
		size_t sub{ div > 6 ? 2ull : 1ull };
		size_t fdd{};
		for (size_t i = 0; i < sub; i++) {
			fdd = ffv.find('_', fdd ? fdd + 1 : fdd);
			if (fdd == std::string::npos) {
				fdd = ffv.size();
			}
		}

		std::memcpy(context->fftype, context->ffname, fdd);
		for (size_t i = 0; i < fdd; i++) {
			if (context->fftype[i] == '_') {
				context->fftype[i] = '/';
			}
		}
	}
	return context->fftype;
}
ActsAPIFastFile_FastFileOption* ActsAPIFastFile_GetCurrentOption() {
	return fastfile::currentOpt;
}

ActsAPIFastFile_FastFileContext* ActsAPIFastFile_GetCurrentContext() {
	return fastfile::currentCtx;
}
