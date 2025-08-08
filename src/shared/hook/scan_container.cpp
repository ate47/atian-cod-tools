#include <includes_shared.hpp>
#include <hook/scan_container.hpp>
#include <utils/utils.hpp>
#include <utils/hash_mini.hpp>
#include <core/bytebuffer_file.hpp>

namespace hook::scan_container {

	std::filesystem::path GetContainersPath() {
		static std::filesystem::path path{ utils::GetProgDir() / "scans" };
		return path;
	}

	std::filesystem::path GetContainerPath(uint32_t uid) {
		return GetContainersPath() / std::format("store_{:x}.scan", uid);
	}

	void CleanContainers() {
		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurseExt(GetContainersPath(), paths, ".scan\0");
		for (const std::filesystem::path& p : paths) {
			std::filesystem::remove(p);
		}
	}

	constexpr uint32_t CONTAINER_MAGIC = 0x30435341;

	void ScanContainer::Sync(bool force) {
		if (sync && !force) return;
		results.clear();
		sync = true;
		utils::InFileCE is{ scanPath, false, std::ios::binary };
		if (!is) return; // no file
		core::bytebuffer::FileReader reader{ is };
		if (reader.Read<uint32_t>() != CONTAINER_MAGIC) {
			LOG_WARNING("Can't read ScanContainer({}): Bad magic", scanPath.string());
			return; // ignore bad magic: wrong version?
		}
		if (reader.Read<uint32_t>() != uid) {
			LOG_WARNING("Can't read ScanContainer({}): Bad uid", scanPath.string());
			return; // wtf
		}

		uint32_t scans{ reader.Read<uint32_t>() };
		for (size_t i = 0; i < scans; i++) {
			uint64_t hash{ reader.Read<uint64_t>() };
			uint32_t scans{ reader.Read<uint32_t>() };

			ResultValue& value{ results[hash] };
			value.loaded = true;
			value.res.reserve(scans);
			for (size_t j = 0; j < scans; j++) {
				uint32_t rva{ reader.Read<uint32_t>() };
				value.res.push_back(rva);
			}
		}
	}

	void ScanContainer::Save() {
		std::filesystem::create_directories(scanPath.parent_path());
		utils::OutFileCE os{ scanPath, true, std::ios::binary };
		utils::WriteValue<uint32_t>(os, CONTAINER_MAGIC);
		utils::WriteValue<uint32_t>(os, uid);
		utils::WriteValue<uint32_t>(os, (uint32_t)results.size());
		for (auto& [hash, value] : results) {
			utils::WriteValue<uint64_t>(os, hash);
			utils::WriteValue<uint32_t>(os, (uint32_t)value.res.size());
			for (uint32_t rva : value.res) {
				utils::WriteValue<uint32_t>(os, rva);
			}
		}
	}

	std::vector<hook::library::ScanResult> ScanContainer::Scan(const char* path, const char* name) {
		Sync();
		ResultValue& val{ results[hash::Hash64(path)] };

		if (val.loaded) {
			std::vector<hook::library::ScanResult> res{};
			LOG_TRACE("ScanContainer: Use cached {} ({})", path, name ? name : "no name");
			if (val.res.size()) LOG_TRACE("ScanContainer: Value: {}:0x{:x}", lib, val.res[0] );
			res.reserve(val.res.size());
			for (uint32_t rva : val.res) {
				res.emplace_back((byte*)lib[rva]);
			}
			return res;
		}

		// search value
		std::vector<hook::library::ScanResult> r{ lib.Scan(path, name) };

		val.res.reserve(r.size());
		val.loaded = true;

		for (hook::library::ScanResult& rva : r) {
			val.res.emplace_back((uint32_t)lib.Rloc(rva.location));
		}

		return r;
	}
	hook::library::ScanResult ScanContainer::ScanSingle(const char* path, const char* name) {
		std::vector<hook::library::ScanResult> res{ Scan(path, name) };

		if (res.empty()) {
			if (ignoreMissing) {
				LOG_ERROR("Can't find pattern {}", name ? name : path);
				foundMissing = true;
				return { nullptr };
			}
			throw std::runtime_error(utils::va("Can't find pattern %s", name ? name : path));
		}
		if (res.size() != 1) {
			if (ignoreMissing) {
				LOG_ERROR("Too many finds for pattern {}", name ? name : path);
				foundMissing = true;
				return { nullptr };
			}
			throw std::runtime_error(utils::va("Too many finds for pattern %s", name ? name : path));
		}

		return res[0];
	}
	hook::library::ScanResult ScanContainer::ScanAny(const char* path, const char* name) {
		std::vector<hook::library::ScanResult> res{ Scan(path, name) };

		if (res.empty()) {
			if (ignoreMissing) {
				LOG_ERROR("Can't find pattern {}", name ? name : path);
				foundMissing = true;
				return { nullptr };
			}
			throw std::runtime_error(utils::va("Can't find pattern %s", name ? name : path));
		}

		return res[0];
	}
}