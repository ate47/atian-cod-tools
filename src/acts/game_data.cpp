#include <includes.hpp>
#include <actscli.hpp>
#include <game_data.hpp>

namespace acts::game_data {
	static core::config::ConfigEnumData cTypeInfo[]{
		{ "enum", CTT_ENUM },
		{ "struct", CTT_STRUCT },
	};
	static core::config::ConfigEnumData scanTypeInfo[]{
		{ "Relative", SCT_RELATIVE },
		{ "Absolute", SCT_ABSOLUTE },
		{ "Offset", SCT_OFFSET },
	};


	std::filesystem::path GetBaseDir() {
		actscli::ActsOptions& opts{ actscli::options() };
		std::string scanpath{ core::config::GetString("game.dir", "") };
		if (scanpath.empty()) {
			return utils::GetProgDir() / "games";
		}
		return scanpath;
	}
	size_t ParseOffsetScan(const std::string& scan) {
		size_t val;
		try {
			size_t idx{};
			if (scan.starts_with("0x")) {
				idx = 2;
			}
			val = std::strtoull(scan.data() + idx, nullptr, 16);
		}
		catch (...) {
			val = 0;
		}
		if (!val) {
			throw std::runtime_error(std::format("Can't parse offset: {}", scan));
		}
		return val;
	}

	GameData::GameData(const char* dirname)
		: cfg(GetBaseDir() / std::format("{}.json", dirname)), dirname(dirname) {
		if (!cfg.SyncConfig(false)) {
			throw std::runtime_error(std::format("Can't find scan dir {}: {}", dirname, cfg.configFile.string()));
		}
	}
	hook::scan_container::ScanContainer& GameData::GetScanContainer() {
		if (!scan) throw std::runtime_error("Missing scan container");
		return *scan;
	}

	void GameData::Redirect(const char* id, void* to, const char* parent) {
		void* from{ GetPointer(id, parent) };
		if (from) {
			hook::memory::RedirectJmp(from, to);
		}
	}

	void GameData::Nulled(const char* id, const char* parent) {
		void* loc{ GetPointer(id, parent) };
		if (loc) {
			hook::memory::Nulled(loc);
		}
	}

	ScanData GameData::GetScan(const char* id, const char* parent) {
		ScanData res{};
		res.name = id;
		res.ctype = cfg.GetString(std::format("{}.{}.ctype", parent, id), "");
		res.path = cfg.GetString(std::format("{}.{}.scan", parent, id), "");
		res.single = cfg.GetBool(std::format("{}.{}.single", parent, id), false);
		res.offset = (size_t)cfg.GetInteger(std::format("{}.{}.offset", parent, id), 0);
		res.postOffset = cfg.GetInteger(std::format("{}.{}.postOffset", parent, id), 0);
		res.type = cfg.GetEnumVal<ScanType>(std::format("{}.{}.type", parent, id), scanTypeInfo, ARRAYSIZE(scanTypeInfo), SCT_UNKNOWN);


		if (res.path.empty()) {
			throw std::runtime_error(std::format("can't find scan path for {}::{}.{}", dirname, parent, id));
		}
		if (!res.type) {
			throw std::runtime_error(std::format("invalid scan type for {}::{}.{}", dirname, parent, id));
		}
		if (res.type == SCT_OFFSET) {
			res.offset += ParseOffsetScan(res.path);
		}

		return res;
	}

	void GameData::ApplyNullScans(const char* id) {
		std::string parent{ std::format("nullscans.{}", id) };
		rapidjson::Value& nullScans{ cfg.GetVal(parent.data(), 0, cfg.main) };
		if (!nullScans.IsObject()) {
			throw std::runtime_error(std::format("Invalid nullscan type in {}: {} is an object", dirname, id));
		}

		for (auto& [k, v] : nullScans.GetObj()) {
			Nulled(k.GetString(), parent.data());
		}
	}

	void GameData::AddTypesToIdc(deps::idc_builder::IdcBuilder& builder) {
		rapidjson::Value& ctypesVal{ cfg.GetVal("ctypes", 0, cfg.main) };
		rapidjson::Value& cdeclsVal{ cfg.GetVal("cdecls", 0, cfg.main) };
		if (ctypesVal.IsArray()) {
			for (rapidjson::Value& ctype : ctypesVal.GetArray()) {
				if (!ctype.IsObject()) {
					LOG_WARNING("Invalid ctype in {}: Not an object", dirname);
					continue;
				}
				auto obj{ ctype.GetObj() };
				auto nameIt{ obj.FindMember("name") };
				auto typeIt{ obj.FindMember("type") };

				if (nameIt == obj.MemberEnd() || !nameIt->value.IsString()) {
					LOG_WARNING("Invalid ctype in {}: missing valid name", dirname);
					continue;
				}
				const char* name{ nameIt->value.GetString() };

				if (typeIt == obj.MemberEnd() || !typeIt->value.IsString()) {
					LOG_WARNING("Invalid ctype in {}: missing valid type for {}", dirname, name);
					continue;
				}

				CTypeType type{ core::config::ParseEnumValue<CTypeType>(typeIt->value.GetString(), cTypeInfo, ARRAYSIZE(cTypeInfo)) };

				if (!type) {
					LOG_WARNING("Invalid ctype in {}: bad type for {}", dirname, name);
					continue;
				}

				switch (type) {
				case CTT_STRUCT:
					builder.AddStruct(name);
					break;
				case CTT_ENUM: {
					deps::idc_builder::IdcEnumId id{ builder.AddEnum(name) };
					auto fieldsIt{ obj.FindMember("fields") };
					if (fieldsIt != obj.MemberEnd()) {
						if (!fieldsIt->value.IsObject()) {
							LOG_WARNING("Invalid ctype in {}: 'fields' should be an object for {}", dirname, name);
							continue;
						}

						for (auto& [key, val] : fieldsIt->value.GetObj()) {

							const char* fieldName{ key.GetString() };
							if (!val.IsInt64()) {
								LOG_WARNING("Invalid ctype in {}: field {} isn't an int {}", dirname, fieldName, name);
								continue;
							}
							builder.AddEnumMember(id, fieldName, val.GetInt64());
						}
					}
				}
					break;
				default: throw std::runtime_error(std::format("CTYPE TYPE NOT IMPLEMENTED : {}", (int)type));
				}
			}
		}
		if (cdeclsVal.IsArray()) {
			for (rapidjson::Value& cdeclVal : cdeclsVal.GetArray()) {
				if (!cdeclVal.IsObject()) {
					LOG_WARNING("Invalid cdecl in {}: Not an object", dirname);
					continue;
				}
				auto obj{ cdeclVal.GetObj() };
				auto declIt{ obj.FindMember("decl") };
				auto flagsIt{ obj.FindMember("flags") };

				if (declIt == obj.MemberEnd() || !declIt->value.IsString()) {
					LOG_WARNING("Invalid cdecl in {}: missing valid decl", dirname);
					continue;
				}
				const char* decl{ declIt->value.GetString() };

				const char* flags;
				if (flagsIt != obj.MemberEnd()) {
					if (!flagsIt->value.IsString()) {
						LOG_WARNING("Invalid cdecl in {}: bad type for decl {}", dirname, decl);
						continue;
					}
					flags = flagsIt->value.GetString();
				} else {
					flags = nullptr;
				}

				builder.AddCDecl(decl, flags);
			}
		}
	}
	void GameData::ScanToIdc(deps::idc_builder::IdcBuilder& builder, const char* parent) {
		hook::scan_container::ScanContainer& container{ GetScanContainer() };

		rapidjson::Value& scansVal{ cfg.GetVal(parent, 0, cfg.main) };
		if (!scansVal.IsObject()) {
			return; // not an object
		}

		for (auto& [k, v] : scansVal.GetObj()) {
			ScanData data{ GetScan(k.GetString(), parent) };
			if (data.name.empty() || data.name[0] == '$') {
				continue; // unused
			}

			std::vector<void*> array{ GetPointerArray<void*>(data.name.data()) };

			if (array.size() != 1) {
				continue; // too many or no found
			}

			builder.AddAddressEx(array[0], data.name.data(), "SN_CHECK | SN_NOWARN", data.ctype.empty() ? nullptr : data.ctype.data());
		}
	}

	void GameData::ScanAllToIdc(deps::idc_builder::IdcBuilder& builder) {
		ScanToIdc(builder, BASE_PARENT);
		rapidjson::Value& nullscans{ cfg.GetVal("nullscans", 0, cfg.main) };
		if (nullscans.IsObject()) {
			for (auto& [k, v] : nullscans.GetObj()) {
				ScanToIdc(builder, utils::va("nullscans.%s", k.GetString()));
			}
		}

	}
}