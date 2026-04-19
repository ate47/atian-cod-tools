#include <includes_shared.hpp>
#include "library.hpp"
#include "process.hpp"
#include <utils/utils.hpp>
#include <utils/crc.hpp>

namespace hook::library {
	void* GetLibraryInfo(const void* address) {
		return platform::GetModuleOfAddress(address);
	}

	const char* GetLibraryName(void* hmod) {
		return platform::GetSharedName(hmod);
	}

	const char* GetLibraryPath(void* hmod) {
		return platform::GetSharedPath(hmod);
	}

	const char* LocatePDB(void* hmod) {
		return platform::GetDebugPath(hmod);
	}

	const char* CreateScanPattern(const void* data, size_t len) {
		std::string patStr{};
		patStr.resize((len + 1) * 3 + 1);
		const byte* bd{ (byte*)data };

		char* patStrPtr{ patStr.data() };

		for (size_t i = 0; i < len; i++) {
			std::snprintf(&patStrPtr[3 * i], 4, "%02x ", (uint32_t)bd[i]);
		}

		return patStrPtr;
	}

	const char* CreateScanPatternString(const char* str) {
		return CreateScanPattern(str, std::strlen(str) + 1);
	}

	std::vector<ScanResult> ScanLibraryString(void* hmod, const char* pattern, bool single, const char* name) {
		size_t pl{ std::strlen(pattern) };
		std::string patStr{};
		patStr.resize((pl + 1) * 3 + 1);

		char* patStrPtr{ patStr.data() };

		for (size_t i = 0; i <= pl; i++) {
			std::snprintf(&patStrPtr[3 * i], 4, "%02x ", (uint32_t)pattern[i]);
		}

		return ScanLibrary(hmod, patStrPtr, single, name);
	}

	std::vector<ScanResult> ScanLibrary(void* hmod, const char* pattern, bool single, const char* name) {
		std::vector<ScanResult> res{};


		std::vector<uint8_t*> find{};

		std::vector<uint8_t> mask{};
		std::vector<uint8_t> searched{};

		bool mid{ true };

		// parse pattern
		const char* str{ pattern };
		while (*str) {
			char c = *(str++);
			if (isspace(c)) {
				continue;
			}

			mid = !mid;

			if (c == '?') {
				if (mid) {
					throw std::runtime_error(utils::va(actssec("Wildcard pattern in half byte! %s (%s)"), pattern, name ? name : actssec("no name")));
				}
				if (str[0] == '?') {
					// test if we are in a packed context
					if (!str[1] || isspace(str[1])) {
						str++; // consume both ??
					}
				}
				// consume both
				mid = !mid;
				mask.push_back(0);
				searched.push_back(0);
				continue;
			}

			auto b = utils::ctob(c);

			if (mid) {
				*(mask.end() - 1) |= 0xF;
				*(searched.end() - 1) |= b;
			}
			else {
				mask.push_back(0xF0);
				searched.push_back(b << 4);
			}
		}

		// reversed because we set it by default to 0
		if (!mid) {
			throw std::runtime_error(utils::va(actssec("Scan pattern has half byte! %s (%s)"), pattern, name ? name : actssec("no name")));
		}

		auto it1 = mask.begin();
		auto it2 = searched.begin();

		// clear start
		while (it1 != mask.end()) {
			if (*it1) {
				break;
			}
			it1 = mask.erase(it1);
			it2 = searched.erase(it2);
		}

		if (!mask.size()) {
			throw std::runtime_error(std::format(actssec("Empty pattern! %s (%s)"), pattern, name ? name : actssec("no name")));
		}

		// clear end
		auto eit1 = mask.end() - 1;
		auto eit2 = searched.end() - 1;
		while (eit1 != mask.begin()) {
			if (*eit1) {
				break;
			}
			eit1 = mask.erase(eit1) - 1;
			eit2 = searched.erase(eit2) - 1;
		}

		if (!mask.size()) {
			throw std::runtime_error(std::format(actssec("Empty pattern! %s (%s)"), pattern, name ? name : actssec("no name")));
		}

		library::Library lib{ hmod };

		constexpr size_t lazySize = 0x10000000;

		if (lazySize < mask.size()) {
			throw std::runtime_error(utils::va("Pattern too big! %s (%s)", pattern, pattern, name ? name : "no name")); // wtf?
		}


		LOG_TRACE("Start searching of pattern {} ({})", pattern, name ? name : "no name");

		for (platform::MemoryRegion& region : platform::EnumerateMemoryRegions(hmod)) {
			if (!region.readable) {
				continue; // can't read
			}

			uint8_t* current = region.base;
			size_t regionSize = region.size;

			for (size_t off = 0; off + mask.size() <= regionSize; off++) {
				bool match = true;
				for (size_t i = 0; i < mask.size(); i++) {
					if ((current[off + i] & mask[i]) != searched[i]) {
						match = false;
						break;
					}
				}
				if (match) {
					res.emplace_back(current + off);
					if (single) {
						LOG_TRACE("Pattern find -> {}", (hook::library::CodePointer)res[0].location);
						return res;
					}
				}
			}
		}
		LOG_TRACE("Pattern find -> {}", res.size());

		return res;
	}

	void hook::library::Library::Redirect(const char* pattern, void* func, const char* name) const {
		hook::memory::RedirectJmp(ScanSingle(pattern, name).location, func);
	}

	void hook::library::Library::Redirect(size_t offset, void* func) const {
		hook::memory::RedirectJmp((*this)[offset], func);
	}

	uint32_t hook::library::Library::GetUID() const {
		if (!hmod) return 0;
		return ModuleInformation().GetUID();
	}

	hook::library::Detour hook::library::Library::CreateDetour(const char* pattern, void* to, const char* name) const {
		hook::library::Detour det;

		det.Create(ScanSingle(pattern, name).location, to);

		return det;
	}

	Detour hook::library::Library::CreateDetour(size_t offset, void* to) const {
		hook::library::Detour det;

		det.Create((*this)[offset], to);

		return det;
	}

	constexpr uint64_t SCAN_CONTAINER_FILE_MAGIC = 0x000a4e41435380;
	constexpr uint64_t SCAN_CONTAINER_FILE_VERSION = 0x210000000000000;

	struct ScanContainer {
		const char* name{};
		std::unordered_map<std::string, std::unordered_set<uint32_t>> deltas{};

		std::filesystem::path GetPath() {
			return utils::va("%s.scan", name);
		}
	};

	static ScanContainer container{};

	void InitScanContainer(const char* name) {
		container.name = name;

		// load scan file

		std::string buff{};
		auto path = container.GetPath();

		if (!utils::ReadFile(path, buff)) {
			LOG_WARNING_s("No scan container found, the loading will take some time");
			return; // empty, nothing to read
		}

		byte* file = (byte*)buff.data();

		if (*reinterpret_cast<decltype(SCAN_CONTAINER_FILE_MAGIC)*>(file) != (SCAN_CONTAINER_FILE_MAGIC | SCAN_CONTAINER_FILE_VERSION)) {
			// bad version/magic, we need to erase it
			LOG_WARNING("Detected scan container with bad magic, it'll be erased at save '{}'", path.string());
			return;
		}

		file += sizeof(SCAN_CONTAINER_FILE_MAGIC);

		uint32_t count = *reinterpret_cast<uint32_t*>(file);
		file += sizeof(count);

		for (size_t i = 0; i < count; i++) {
			if (file + 5 > (byte*)buff.data() + buff.size()) {
				return;
			}

			char* str = reinterpret_cast<char*>(file);

			file += strlen(str) + 1;

			uint32_t num = *reinterpret_cast<uint32_t*>(file);
			file += sizeof(num);

			uint32_t* locs = reinterpret_cast<uint32_t*>(file);

			auto& delta = container.deltas[str];
			for (size_t j = 0; j < num; j++) {
				file += sizeof(num);

				delta.insert(locs[j]);
			}
		}
	}
	bool ScanMatch(void* ptr, const char* scan) {
		std::vector<uint8_t> mask{};
		std::vector<uint8_t> searched{};
		bool mid{ true };
		const char* str{ scan };
		while (*str) {
			char c = *(str++);
			if (isspace(c)) {
				continue;
			}

			mid = !mid;

			if (c == '?') {
				if (mid) {
					throw std::runtime_error(utils::va(actssec("Wildcard pattern in half byte! %s"), scan));
				}
				if (str[0] == '?') {
					// test if we are in a packed context
					if (!str[1] || isspace(str[1])) {
						str++; // consume both ??
					}
				}
				// consume both
				mid = !mid;
				mask.push_back(0);
				searched.push_back(0);
				continue;
			}

			auto b = utils::ctob(c);

			if (mid) {
				*(mask.end() - 1) |= 0xF;
				*(searched.end() - 1) |= b;
			}
			else {
				mask.push_back(0xF0);
				searched.push_back(b << 4);
			}
		}

		// reversed because we set it by default to 0
		if (!mid) {
			throw std::runtime_error(utils::va(actssec("Scan pattern has half byte! %s"), scan));
		}

		byte* loc{ (byte*)ptr };
		for (size_t i = 0; i < searched.size(); i++) {
			if ((loc[i] & mask[i]) != searched[i]) {
				return false;
			}
		}

		return true;
	}

	void SaveScanContainer() {
		std::vector<byte> file{};

		utils::WriteValue<uint64_t>(file, SCAN_CONTAINER_FILE_MAGIC | SCAN_CONTAINER_FILE_VERSION);

		utils::WriteValue<uint32_t>(file, (uint32_t)container.deltas.size());
		LOG_TRACE("Save scan:");
		for (const auto& [name, delta] : container.deltas) {
			utils::WriteString(file, name.data());
			utils::WriteValue<uint32_t>(file, (uint32_t)delta.size());
			for (const auto loc : delta) {
				utils::WriteValue<uint32_t>(file, loc);
				LOG_TRACE("{} -> 0x{:x}", name, loc);
			}
		}

		utils::WriteFile(container.GetPath(), file.data(), file.size());

	}

	ScanEntry* ScanLogger::AllocEntry(const char* name) {
		if (!name) return nullptr;
		ScanEntry* e{ alloc.New<ScanEntry>() };
		entries.push_back(e);
		e->name = alloc.CloneStr(name);
		return e;
	}

	void ScanLogger::WriteLogs(std::filesystem::path out, ScanLoggerLogsOpt* opt) {
		std::filesystem::create_directories(out);
		std::filesystem::path infoFilePath{ out / std::format("{}_scans.csv", opt->base) };
		std::filesystem::path idaScriptPath{ out / std::format("{}_scans.idc", opt->base) };

		std::unordered_map<const char*, std::vector<ScanEntry*>> entryMap{};

		// group by name
		for (ScanEntry* e : entries) {
			if (!e->name) {
				continue;
			}
			entryMap[e->name].push_back(e);
		}
		{
			// scan file
			utils::OutFileCE infoFile{ infoFilePath };

			infoFile << "name,offsets";
			for (auto& [name, es] : entryMap) {
				infoFile << "\n" << name << ",";
				for (size_t i = 0; i < es.size(); i++) {
					if (i) infoFile << ";";
					infoFile << hook::library::CodePointer{ es[i]->location };
				}
			}
		}
		{
			// script to add to ida to load the 
			utils::OutFileCE idaScript{ idaScriptPath };

			idaScript
				<< "#include <idc/idc.idc>\n\n";
			if (opt->headerWriter) opt->headerWriter(idaScript);
			idaScript << "static main() {\n";
			utils::Padding(idaScript, 1) << "auto base = get_imagebase();\n";
			for (auto& [name, es] : entryMap) {
				if (es.size() > 1) {
					continue;
				}
				byte* loc{ es[0]->location };
				hook::library::Library library{ GetLibraryInfo(loc) };
				if (!library) {
					continue;
				}
				size_t rva{ (size_t)(loc - (byte*)*library) };
				utils::Padding(idaScript, 1) << "MakeName(base + 0x" << std::hex << rva << ", \"" << name << "\");\n";
			}
			if (opt->inMainWriter) opt->inMainWriter(idaScript);
			idaScript << "}\n";
		}
	}

	void ScanLogger::Clean() {
		entries.clear();
	}

	static Library main{};
	std::vector<ScanResult> QueryScanContainer(const char* name, const char* pattern, bool single) {
		auto it = container.deltas.find(name);
		if (it != container.deltas.end()) {
			std::vector<ScanResult> res{};

			for (auto loc : it->second) {
				res.emplace_back((byte*)main[loc]);
			}

			return res;
		}

		auto res = main.Scan(pattern, single);
		auto& locs = container.deltas[pattern];

		// save scan
		for (auto& loc : res) {
			locs.insert((uint32_t)(loc.location - (byte*)*main));
		}

		return res;
	}

	ScanResult QueryScanContainerSingle(const char* name, const char* pattern) {
		auto res = QueryScanContainer(name, pattern, true);

		if (res.empty()) {
			throw std::runtime_error(utils::va(actssec("Can't find pattern %s"), name ? name : pattern));
		}
		if (res.size() != 1) {
			throw std::runtime_error(utils::va(actssec("Too many finds for pattern %s"), name ? name : pattern));
		}

		return res[0];
	}

	hook::library::CodePointer::CodePointer(void* location) : location(location) {}

	std::ostream& operator<<(std::ostream& out, const hook::library::Library& ptr) {
		if (!ptr) {
			return out << "<INVALID_LIB>";
		}
		return out << ptr.GetName() << "[0x" << std::hex << reinterpret_cast<uintptr_t>(ptr.hmod) << "]";
	}

	std::vector<const char*> hook::library::Library::GetIATModules() const {
		return platform::GetIATModules(*this);
	}

	void hook::library::Library::PatchIAT() {
		platform::PatchIAT(*this);
	}

	std::ostream& operator<<(std::ostream& out, const hook::library::CodePointer& ptr) {
		hook::library::Library library{ GetLibraryInfo(ptr.location) };
		return out << library << "+0x" << std::hex << ((byte*)ptr.location - (byte*)*library);
	}
}