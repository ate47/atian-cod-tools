#include <includes.hpp>
#include <utils/utils.hpp>
#include <core/async.hpp>
#include <rapidcsv.h>
#include "actscli.hpp"
#include "compatibility/scobalula_wni.hpp"
#include <core/memory_allocator.hpp>
#include <core/bytebuffer.hpp>
#include <BS_thread_pool.hpp>

namespace {
	core::memory_allocator::MemoryAllocator alloc{};
	std::unordered_map<uint64_t, const char*> g_hashMap{};
	std::set<uint64_t> g_extracted{};
	const char* hashPrefix{};
	bool g_saveExtracted = false;
	bool g_saveExtractedUnk = false;
	bool show0 = false;
	bool markHash = false;
	bool heavyHashes = false;
	std::mutex asyncMutex{};
}

namespace hashutils {
	std::mutex* GetMutex(bool forceAsync) {
		if (forceAsync || !core::async::IsSync(core::async::AT_HASHES)) {
			return nullptr;
		}
		return &asyncMutex;
	}

	const std::unordered_map<uint64_t, const char*>& GetMap() {
		return g_hashMap;
	}

	void* AllocHashMemory(size_t len) {
		return alloc.Alloc<byte>(len);
	}

	static void ReadDefaultFile0() {
		auto& opt = actscli::options();
		std::filesystem::path wniPackageIndex;
		if (opt.wniFiles) {
			wniPackageIndex = opt.wniFiles;
		}
		else {
			wniPackageIndex = utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir;
		}

		actslib::profiler::Profiler prof{ "hash" };
		if (opt.installDirHashes) {

			std::vector<std::filesystem::path> wnipaths{};

			utils::GetFileRecurseExt(wniPackageIndex, wnipaths, ".wni\0");

			for (std::filesystem::path& p : wnipaths) {
				if (!compatibility::scobalula::wni::ReadWNIFile(p, [](uint64_t hash, const char* str) {
					AddPrecomputed(hash, str, true, false);
					}, [](size_t len) -> void* {
						return AllocHashMemory(len);
					})) {
					LOG_ERROR("Error when reading WNI files");
				};
			}

			std::vector<std::filesystem::path> csvs{};

			utils::GetFileRecurse(wniPackageIndex, csvs, [](const std::filesystem::path& p) {
				auto s = p.string();
				return s.ends_with(".hash.csv");
				});

			for (const std::filesystem::path& csv : csvs) {
				LOG_DEBUG("Reading HASH CSV {}", csv.string());
				std::string buffer{};

				if (!utils::ReadFile(csv, buffer)) {
					LOG_WARNING("Can't read hash csv {}", csv.string());
					continue;
				}

				rapidcsv::Document doc{};

				std::stringstream stream{ buffer };

				doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

				if (doc.GetColumnCount() < 2) {
					LOG_WARNING("Can't read hash csv {}: invalid file", csv.string());
					continue;
				}

				for (size_t i = 0; i < doc.GetRowCount(); i++) {
					const std::string hash = doc.GetCell<std::string>(0, i);
					const std::string value = doc.GetCell<std::string>(1, i);

					try {
						AddPrecomputed(std::strtoull(hash.c_str(), nullptr, 16), value.c_str(), true);
					}
					catch (std::runtime_error& e) {
						LOG_WARNING("Error when reading {}: invalid line {}: {}", csv.string(), i, e.what());
					}
				}

			}

		}

		::show0 = opt.show0Hash;
		::markHash = opt.markHash;
		::hashPrefix = opt.hashPrefixByPass;
		::heavyHashes = opt.heavyHashes;

		if (opt.noDefaultHash) {
			return;
		}
		const char* file = opt.defaultHashFile;

		if (!file) {
			file = DEFAULT_HASH_FILE;
		}
		std::filesystem::path filePath{ file };
		LOG_DEBUG("Load default hash file {}", filePath.string());
		LoadMap(file, true, !opt.noIWHash, true);

		prof.Stop();
		LOG_TRACE("Loaded wni in {}ms", prof.GetMainSection().GetMillis());
		LOG_DEBUG("End load default hash file");
	}

	void ReadDefaultFile(bool cleanup) {
		static bool loaded{};
		std::lock_guard lg{ asyncMutex };

		if (cleanup) {
			g_hashMap.clear();
			alloc.FreeAll();
			loaded = false;
		}
		if (!loaded) {
			ReadDefaultFile0();
			loaded = true;
		}
	}

	void SaveExtracted(bool value, bool unk) {
		g_saveExtracted = value;
		g_saveExtractedUnk = unk;
		g_extracted.clear();
	}

	void WriteExtracted(const char* file) {
		if (!g_saveExtracted || !file) {
			return; // nothing to do
		}
		LOG_TRACE("Start write extracted");
		std::ofstream out{ file };

		if (!out) {
			LOG_ERROR("Can't open extracted file {}", file);
			return;
		}

		for (const auto& v : g_extracted) {
			auto e = g_hashMap.find(v & hashutils::MASK63);
			if (e != g_hashMap.end()) {
				out << std::hex << v << "," << e->second << "\n";
			}
			else {
				out << std::hex << v << ",hash_" << v << "\n";
			}
		}

		out.close();
		// clear and unset extract
		SaveExtracted(false, false);
		LOG_TRACE("End write extracted into {}", file);
	}

	int LoadMap(const char* file, bool ignoreCol, bool iw, bool async) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		// add common hashes
		LOG_TRACE("Load hash file {}", file);

		// special value
		AddPrecomputed(0, "", true, false);
		AddPrecomputed(hash::HashT89Scr("<error>"), "<error>", true, false);
		AddPrecomputed(hash::HashT89Scr("self"), "self", true, false);
		AddPrecomputed(hash::HashT89Scr("size"), "size", true, false);
		AddPrecomputed(hash::HashT89Scr("nextarray"), "nextarray", true, false);
		AddPrecomputed(hash::HashT89Scr("_"), "_", true, false);

		// class special things
		Add("__constructor", true, iw, true, false);
		Add("__destructor", true, iw, true, false);
		Add("__vtable", true, iw, true, false);
		Add("_deleted", true, iw, true, false);

		// global vars
		Add("level", true, iw, true, false);
		Add("game", true, iw, true, false);
		Add("classes", true, iw, true, false);
		Add("mission", true, iw, true, false);
		Add("anim", true, iw, true, false);
		Add("world", true, iw, true, false);
		Add("sharedstructs", true, iw, true, false);
		Add("memory", true, iw, true, false);

		// structure basic hashes
		Add("system", true, iw, true, false);
		Add("scripts/core_common/system_shared.csc", true, iw, true, false);
		Add("scripts/core_common/system_shared.gsc", true, iw, true, false);
		Add("scripts/common/system.gsc", true, iw, true, false);
		Add("register", true, iw, true, false);
		Add("__init__system__", true, iw, true, false);
		Add("__init__", true, iw, true, false);
		Add("__main__", true, iw, true, false);
		Add("main", true, iw, true, false);
		Add("init", true, iw, true, false);
		// it seems all the varargs are called "vararg", but a flag is also describing, so idk
		// in t10 the vararg param is followed by varargcount
		Add("vararg", true, iw, true, false);
		Add("varargcount", true, iw, true, false);

		// basic letter
		char buff[2] = { 0, 0 };
		for (char c = 'a'; c <= 'z'; c++) {
			*buff = c;
			Add(buff, true, iw, true);
		}
		for (char c = '0'; c <= '9'; c++) {
			*buff = c;
			Add(buff, true, iw, true);
		}

		// nameless fields in compiler
		Add("$$tmp", true, iw, true, false);
		Add("$notif_checkum", true, iw, true, false);
		Add("$BAD_VAR", true, iw, true, false);
		for (size_t i = 0; i < 0x100; i++) {
			Add(utils::va("$nameless_%llx", i), true, iw, true);
			Add(utils::va("$$v%llx", i), true, iw, true);
		}

		// Decompiler special values
		Add("self", true, iw, true, false);
		Add("size", true, iw, true, false);

		// DDL names
		Add("root", true, iw, true, false); // root struct
		Add("__pad", true, iw, true, false); // padding

		// gsc temp vars
		static const char* gscTmpStart =
			"__a\0" // array
			"__k\0" // key
			"__ki\0" // key iterator
			"__ka\0" // keys array
			;
		const char* gscTmpStartIt = gscTmpStart;
		while (*gscTmpStartIt) {
			for (size_t i = 0; i < 1000; i++) {
				const char* s{ alloc.CloneStr(utils::va("%s%lld", gscTmpStartIt, i)) };
				AddPrecomputed(hash::HashT10Scr(s), s, true, false);
				AddPrecomputed(hash::HashT10ScrSP(s), s, true, false);
				AddPrecomputed(hash::HashJupScr(s), s, true, false);
			}
			gscTmpStartIt += std::strlen(gscTmpStartIt) + 1;
		}

		// ADL names
		AddPrecomputed(hash::Hash64("bool"), "bool", true, false);
		AddPrecomputed(hash::Hash64("byte"), "byte", true, false);
		AddPrecomputed(hash::Hash64("uint8"), "uint8", true, false);
		AddPrecomputed(hash::Hash64("uint8_t"), "uint8_t", true, false);
		AddPrecomputed(hash::Hash64("uint16"), "uint16", true, false);
		AddPrecomputed(hash::Hash64("uint16_t"), "uint16_t", true, false);
		AddPrecomputed(hash::Hash64("uint32"), "uint32", true, false);
		AddPrecomputed(hash::Hash64("uint32_t"), "uint32_t", true, false);
		AddPrecomputed(hash::Hash64("uint64"), "uint64", true, false);
		AddPrecomputed(hash::Hash64("uint64_t"), "uint64_t", true, false);
		AddPrecomputed(hash::Hash64("char"), "char", true, false);
		AddPrecomputed(hash::Hash64("int8"), "int8", true, false);
		AddPrecomputed(hash::Hash64("int8_t"), "int8_t", true, false);
		AddPrecomputed(hash::Hash64("int16"), "int16", true, false);
		AddPrecomputed(hash::Hash64("int16_t"), "int16_t", true, false);
		AddPrecomputed(hash::Hash64("int32"), "int32", true, false);
		AddPrecomputed(hash::Hash64("int32_t"), "int32_t", true, false);
		AddPrecomputed(hash::Hash64("int64"), "int64", true, false);
		AddPrecomputed(hash::Hash64("int64_t"), "int64_t", true, false);
		AddPrecomputed(hash::Hash64("float"), "float", true, false);
		AddPrecomputed(hash::Hash64("double"), "double", true, false);
		AddPrecomputed(hash::Hash64("string"), "string", true, false);
		AddPrecomputed(hash::Hash64("hash"), "hash", true, false);
		AddPrecomputed(hash::Hash64("int"), "int", true, false);
		AddPrecomputed(hash::Hash64("uint"), "uint", true, false);
		AddPrecomputed(hash::Hash64("long"), "long", true, false);
		AddPrecomputed(hash::Hash64("ulong"), "ulong", true, false);
		AddPrecomputed(hash::Hash64("$$padding"), "$$padding", true, false);
		// Dump CF
		AddPrecomputed(hash::Hash64("localize.json"), "localize.json", true, false);

		size_t hashFileLen;
		byte* hashFile{ (byte*)utils::ReadFilePtr(file, &hashFileLen, [](size_t len) { return AllocHashMemory(len); }) };

		if (!hashFile) {
			LOG_TRACE("End load hash file {}", file);
			return 0; // nothing to read
		}

		hashFile[hashFileLen] = '\n'; // for last

		int issues{};
		core::bytebuffer::ByteBuffer hashFileBuffer{ hashFile, hashFileLen + 1 };

		size_t count{};
		while (hashFileBuffer.CanRead(1)) {
			size_t len;
			char* line{ hashFileBuffer.ReadString(&len, '\n') };
			if (len) {
				if (line[len - 1] == '\r') {
					line[len - 1] = 0;
					len--;
				}
				else {
					line[len] = 0;
				}
			}
			if (!len) continue;
			if (!Add(line, ignoreCol, iw, true, false)) {
				issues++;
			}
			count++;
		}

		LOG_TRACE("End load hash file {} -> {}", file, count);
		return issues;
	}

	bool Add(const char* str, bool ignoreCol, bool iw, bool async, bool clone) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		// use the same string for all the hashes
		if (clone) {
			str = alloc.CloneStr(str);
		}
		AddPrecomputed(hash::Hash64(str), str, true, false);
		if (iw) {
			AddPrecomputed(hash::HashIWRes(str), str, true, false);
			AddPrecomputed(hash::HashJupScr(str), str, true, false);
			AddPrecomputed(hash::Hash64(str, 0x811C9DC5, 0x1000193) & 0xFFFFFFFF, str, true, false);
			AddPrecomputed(hash::HashIWDVar(str), str, true, false);
			AddPrecomputed(hash::HashT10Scr(str), str, true, false);
			AddPrecomputed(hash::HashT10ScrSP(str), str, true, false);
		}
		bool cand32 = true;

		for (const char* s = str; *s; s++) {
			auto c = *s;
			if (!(
				(c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z')
				|| (c >= '0' && c <= '9')
				|| c == '_')) {
				cand32 = false; // a hash32 can only match [a-z0-9A-Z_]* in this context
				break;
			}
		}

		if (cand32) {
			AddPrecomputed(hash::HashT7(str), str, true, false);

			auto h = hash::HashT89Scr(str);
			if (!ignoreCol) {
				auto find = g_hashMap.find(h);
				if (find != g_hashMap.end() && _strcmpi(str, find->second)) {
					LOG_WARNING("Coll '{}'='{}' #{:x}", str, find->second, h);
					return false;
				}
			}
			AddPrecomputed(h, str, true, false);
		}
		return true;
	}
	void AddPrecomputed(uint64_t value, const char* str, bool async, bool clone) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		g_hashMap.emplace(value & hashutils::MASK63, clone ? alloc.CloneStr(str) : str);
	}

	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) {
		ReadDefaultFile();
		core::async::opt_lock_guard lg{ GetMutex(false) };
		if (hashPrefix) type = hashPrefix;
		if (!hash) {
			if (show0 || markHash) {
				snprintf(out, outSize, "%s_0", type);
			}
			else if (outSize) {
				*out = 0;
			}
			return true;
		}
		const auto res = g_hashMap.find(hash & hashutils::MASK63);
		if (g_saveExtracted) {
			if (g_saveExtractedUnk || res != g_hashMap.end()) {
				g_extracted.emplace(hash);
			}
		}
		if (res == g_hashMap.end()) {
			snprintf(out, outSize, heavyHashes ? "%s_%016llX" : "%s_%llx", type, hash);
			return false;
		}
		if (markHash) {
			snprintf(out, outSize, heavyHashes ? "<%016llX>%s" : "<%llx>%s", hash, res->second);
		}
		else {
			snprintf(out, outSize, "%s", res->second);
		}
		return true;
	}

	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash) {
		static thread_local char buffer[10][0x600];
		static thread_local size_t bufferIndex = 0;
		ReadDefaultFile();
		bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
		auto& buff = buffer[bufferIndex];
		return { Extract(type, hash, buff, sizeof(buff)), buff };
	}

	char* ExtractTmp(const char* type, uint64_t hash) {
		auto [ok, buff] = ExtractTmpPair(type, hash);
		return buff;
	}

	const char* ExtractPtr(uint64_t hash) {
		ReadDefaultFile();
		const auto res = g_hashMap.find(hash & hashutils::MASK63);
		if (res == g_hashMap.end()) {
			return NULL;
		}
		return res->second;
	}

	size_t Size() {
		ReadDefaultFile();
		return g_hashMap.size() >> 1; // 2 hashes/string
	}

}