#include <includes.hpp>
#include <actscli.hpp>
#include <scans_dir.hpp>

namespace acts::scan_dir {
	std::filesystem::path GetBaseDir() {
		actscli::ActsOptions& opts{ actscli::options() };
		std::string scanpath{ core::config::GetString("scan.scandir", "") };
		if (scanpath.empty()) {
			return utils::GetProgDir() / "scan_paths";
		}
		return scanpath;
	}

	ScanDir::ScanDir(const char* dirname, hook::scan_container::ScanContainer& scan)
		: cfg(GetBaseDir() / std::format("{}.json", dirname)), dirname(dirname), scan(scan) {
		if (!cfg.SyncConfig(false)) {
			throw std::runtime_error(std::format("Can't find scan dir {}: {}", dirname, cfg.configFile.string()));
		}
	}

	core::config::ConfigEnumData scanTypeInfo[]{
		{ "Relative", TYPE_RELATIVE },
		{ "Absolute", TYPE_ABSOLUTE },
	};

	ScanData ScanDir::GetScan(const char* id) {
		ScanData res{};
		res.name = id;
		res.ctype = cfg.GetString(std::format("{}.ctype", id), "");
		res.path = cfg.GetString(std::format("{}.scan", id), "");
		res.single = cfg.GetBool(std::format("{}.single", id), false);
		res.offset = (size_t)cfg.GetInteger(std::format("{}.offset", id), 0);
		res.type = cfg.GetEnumVal<ScanType>(std::format("{}.type", id), scanTypeInfo, ARRAYSIZE(scanTypeInfo), TYPE_UNKNOWN);


		if (res.path.empty()) {
			throw std::runtime_error(std::format("can't find scan path for {}::{}", dirname, id));
		}
		if (!res.type) {
			throw std::runtime_error(std::format("invalid scan type for {}::{}", dirname, id));
		}

		return res;
	}
}