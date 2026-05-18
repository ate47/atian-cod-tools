#include <includes_shared.hpp>
#include <core/config.hpp>
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif
#include <utils/utils.hpp>

namespace core::config {
	namespace {
		std::filesystem::path mainConfigFile{ MAIN_CONFIG_FILE };
	}
	int64_t ParseEnumValue(const char* value, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue, const char* path) {
		if (!value || !*value) {
			return defaultEnumValue;
		}

		for (size_t i = 0; i < dataCount; i++) {
			if (!_strcmpi(value, data[i].name)) {
				return data[i].enumValue;
			}
		}

		if (path) {
			LOG_WARNING("Invalid enum config for path '{}': '{}'", path, value);
		}
		else {
			LOG_WARNING("Invalid enum config: '{}'", value);
		}

		return defaultEnumValue;
	}

#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON

	RapidJsonGeneric& ConfigGenericRefs::GetVal(const char* path, size_t off, RapidJsonGeneric& loc) {
		static rapidjson::Value nullAnswer{ rapidjson::kNullType };
		if (!path || !*path || loc.IsNull()) {
			return nullAnswer; // not a valid path/object
		}
		if (path[0] == '~' && !path[1]) {
			return loc;
		}
		if (path[0] == '/' && !path[1]) {
			return main;
		}

		std::string_view sv{ path + off };
		size_t idx = sv.find('.', 0);

		if (idx == std::string::npos) {
			std::string node{ path + off, sv.length() };

				if (!loc.HasMember(node.c_str())) {
				return nullAnswer;
			}

			return loc[node.c_str()];
		}
		std::string node{ path + off, idx };

			if (!loc.HasMember(node.c_str())) {
			return nullAnswer;
		}

		auto& val = loc[node.c_str()];

		if (!val.IsObject()) {
			return nullAnswer;
		}

		return GetVal(path + off, idx + 1, val);
	}

	void ConfigGenericRefs::SetVal(const char* path, rapidjson::Value& value, size_t off, RapidJsonGeneric& loc) {
		if (!path || !*path) {
			LOG_WARNING("Using setval with invalid path");
			return; // not a valid path
		}
		if (!loc.IsObject()) {
			loc.SetObject();
		}
		std::string_view sv{ path + off };
		size_t idx = sv.find('.', 0);

		if (idx == std::string::npos) {
			std::string node{ path + off, sv.length() };

			rapidjson::Value key{ rapidjson::kStringType };
			key.SetString(node.c_str(), main.GetAllocator());

			if (loc.HasMember(key)) {
				loc.EraseMember(key);
			}

			loc.AddMember(key, value, main.GetAllocator());
			return;
		}

		std::string node{ path + off, idx };

		rapidjson::Value key{ rapidjson::kStringType };
		key.SetString(node.c_str(), main.GetAllocator());

		if (!loc.HasMember(key)) {
			rapidjson::Value v{ rapidjson::kObjectType };
			SetVal(path + off, value, idx + 1, v);
			loc.AddMember(key, v, main.GetAllocator());
		}
		else {
			SetVal(path + off, value, idx + 1, loc[key]);
		}

	}

	int64_t ConfigGenericRefs::GetInteger(const char* path, int64_t defaultValue) {
		rapidjson::Value& val = GetVal(path, 0, base);

		if (val.IsNull() || !val.IsNumber()) {
			this->SetInteger(path, defaultValue);
			return defaultValue;
		}
		return val.GetInt64();
	}

	uint64_t ConfigGenericRefs::GetUnsignedInteger(const char* path, uint64_t defaultValue) {
		rapidjson::Value& val = GetVal(path, 0, base);

		if (val.IsNull() || !val.IsNumber()) {
			this->SetUnsignedInteger(path, defaultValue);
			return defaultValue;
		}
		return val.GetUint64();
	}

	double ConfigGenericRefs::GetDouble(const char* path, double defaultValue) {
		rapidjson::Value& val = GetVal(path, 0, base);

		if (val.IsNull() || !val.IsNumber()) {
			this->SetDouble(path, defaultValue);
			return defaultValue;
		}
		return val.GetDouble();
	}

	const char* ConfigGenericRefs::GetCString(const char* path, const char* defaultValue) {
		rapidjson::Value& val = GetVal(path, 0, base);

		if (val.IsNull() || !val.IsString()) {
			if (defaultValue) {
				this->SetString(path, defaultValue);
			}
			return defaultValue;
		}
		return val.GetString();
	}

	bool ConfigGenericRefs::ScanStringN(const char* path, const char* format, size_t count, ...) {
		va_list va;
		va_start(va, count);
		std::string v{ this->GetString(path) };
		if (v.empty()) {
			return true; // no data
		}
		bool r{ std::vsscanf(v.c_str(), format, va) == count };
		va_end(va);
		return r;
	}

	bool ConfigGenericRefs::GetBool(const char* path, bool defaultValue) {
		rapidjson::Value& val = GetVal(path, 0, base);

		if (val.IsNull() || !val.IsBool()) {
			this->SetBool(path, defaultValue);
			return defaultValue;
		}
		return val.GetBool();
	}

	void ConfigGenericRefs::SetInteger(const char* path, int64_t defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v, 0, base);
	}

	void ConfigGenericRefs::SetUnsignedInteger(const char* path, uint64_t defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v, 0, base);
	}

	void ConfigGenericRefs::SetDouble(const char* path, double defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v, 0, base);
	}

	void ConfigGenericRefs::SetString(const char* path, const std::string& defaultValue) {
		rapidjson::Value v{ rapidjson::kStringType };
		v.SetString(defaultValue.c_str(), main.GetAllocator());
		SetVal(path, v, 0, base);
	}

	void ConfigGenericRefs::SetBool(const char* path, bool defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v, 0, base);
	}

	int64_t ConfigGenericRefs::GetEnum(const char* path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue) {
		const char* defaultValueStr{ "" };
		for (size_t i = 0; i < dataCount; i++) {
			if (data[i].enumValue == defaultEnumValue) {
				defaultValueStr = data[i].name;
				break;
			}
		}
		const char* val{ this->GetCString(path)};
		return ParseEnumValue(val, data, dataCount, defaultEnumValue, path);
	}

	void ConfigGenericRefs::SetEnum(const char* path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount) {
		for (size_t i = 0; i < dataCount; i++) {
			if (data[i].enumValue == enumValue) {
				this->SetString(path, data[i].name);
				return;
			}
		}
		LOG_WARNING("No enum value for path '{}': {}", path, enumValue);
	}

	ConfigGenericRefs::operator bool() const {
		return !base.IsNull();
	}

	bool ParseJson(std::filesystem::path path, RapidJsonDocument& doc) {
		std::string json{};
		if (!utils::ReadFile(path, json)) {
			return false;
		}
		doc.Parse(json.data());
		return true;
	}

	void WriteJson(std::filesystem::path path, const RapidJsonGeneric& value) {
		rapidjson::StringBuffer buff;
		rapidjson::PrettyWriter<decltype(buff)> writer{ buff };
		value.Accept(writer);

		std::string json{ buff.GetString() };
		utils::WriteFile(path, json);
	}

	void __test() {
		Config cfg{};

		enum TestEnum {
			TTE_K0 = 0,
			TTE_K1 = 1,
			TTE_K2 = 2,
			TTE_K3 = 3,
			TTE_K4 = 4,
			TTE_K5 = 5,
		};
		TestEnum test[45];
		cfg.Load("test", test);

	}

#else  // __ACTS_COMPRESS_HAS_RAPIDJSON
// mock result when no rapidjson support, so that the rest of the code can compile and run without config support


	int64_t ConfigGenericRefs::GetInteger(const char* path, int64_t defaultValue) {
		return defaultValue;
	}

	uint64_t ConfigGenericRefs::GetUnsignedInteger(const char* path, uint64_t defaultValue) {
		return defaultValue;
	}

	double ConfigGenericRefs::GetDouble(const char* path, double defaultValue) {
		return defaultValue;
	}

	const char* ConfigGenericRefs::GetCString(const char* path, const char* defaultValue) {
		return defaultValue;
	}

	bool ConfigGenericRefs::ScanStringN(const char* path, const char* format, size_t count, ...) {
		va_list va;
		va_start(va, count);
		va_end(va);
		return false;
	}

	bool ConfigGenericRefs::GetBool(const char* path, bool defaultValue) {
		return defaultValue;
	}

	void ConfigGenericRefs::SetInteger(const char* path, int64_t defaultValue) {
	}

	void ConfigGenericRefs::SetUnsignedInteger(const char* path, uint64_t defaultValue) {
	}

	void ConfigGenericRefs::SetDouble(const char* path, double defaultValue) {
	}

	void ConfigGenericRefs::SetString(const char* path, const std::string& defaultValue) {
	}

	void ConfigGenericRefs::SetBool(const char* path, bool defaultValue) {
	}
	int64_t ConfigGenericRefs::GetEnum(const char* path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue) {
		return defaultEnumValue;
	}

	ConfigGenericRefs::operator bool() const {
		return false;
	}

	void ConfigGenericRefs::SetEnum(const char* path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount) {
	}

	bool Config::SyncConfig(bool save) {
		return false;
	}
	void Config::SaveConfig() const {
	}


#endif // __ACTS_COMPRESS_HAS_RAPIDJSON

	void SetMainConfig(const std::filesystem::path& path) {
		mainConfigFile = path;
	}

	const std::filesystem::path& GetMainConfigFile() {
		return mainConfigFile;
	}

	Config& GetMainConfig() {
		static Config mainCfg{ mainConfigFile };
#ifndef __ACTS_COMPRESS_HAS_RAPIDJSON
		LOG_WARNING("Config support is not available because rapidjson is not supported. Default values will be used for all config entries, and changes to config will not be saved.");
#endif // __ACTS_COMPRESS_HAS_RAPIDJSON
		return mainCfg;
	}
}
