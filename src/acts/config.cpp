#include <includes.hpp>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "config.hpp"

namespace acts::config {
	namespace {
		rapidjson::Document main{};

		rapidjson::GenericValue<decltype(main)::EncodingType, decltype(main)::AllocatorType>& GetVal(const char* path, size_t off = 0, rapidjson::GenericValue<decltype(main)::EncodingType, decltype(main)::AllocatorType>& loc = main) {
			static rapidjson::Value nullAnswer{ rapidjson::kNullType };
			if (!path || !*path) {
				return nullAnswer; // not a valid path
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

		void SetVal(const char* path, rapidjson::Value& value, size_t off = 0, rapidjson::GenericValue<decltype(main)::EncodingType, decltype(main)::AllocatorType>& loc = main) {
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
	}

	int64_t GetInteger(const char* path, int64_t defaultValue) {
		rapidjson::Value& val = GetVal(path);

		if (val.IsNull() || !val.IsNumber()) {
			SetInteger(path, defaultValue);
			return defaultValue;
		}
		return val.GetInt64();
	}

	double GetDouble(const char* path, double defaultValue) {
		rapidjson::Value& val = GetVal(path);

		if (val.IsNull() || !val.IsNumber()) {
			SetDouble(path, defaultValue);
			return defaultValue;
		}
		return val.GetDouble();
	}

	std::string GetString(const char* path, const char* defaultValue) {
		rapidjson::Value& val = GetVal(path);

		if (val.IsNull() || !val.IsString()) {
			SetString(path, defaultValue);
			return defaultValue;
		}
		return val.GetString();
	}

	bool GetBool(const char* path, bool defaultValue) {
		rapidjson::Value& val = GetVal(path);

		if (val.IsNull() || !val.IsBool()) {
			SetBool(path, defaultValue);
			return defaultValue;
		}
		return val.GetBool();
	}

	void SetInteger(const char* path, int64_t defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v);
	}

	void SetDouble(const char* path, double defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v);
	}

	void SetString(const char* path, const std::string& defaultValue) {
		rapidjson::Value v{ rapidjson::kStringType };
		v.SetString(defaultValue.c_str(), main.GetAllocator());
		SetVal(path, v);
	}

	void SetBool(const char* path, bool defaultValue) {
		rapidjson::Value v{ defaultValue };
		SetVal(path, v);
	}

	void SyncConfig() {
		std::string json{};

		if (utils::ReadFile(utils::GetProgDir() / CONFIG_FILE, json)) {
			main.Parse(json.data());
		}
		// not reading isn't an error

		SaveConfig();
	}

	void SaveConfig() {
		rapidjson::StringBuffer buff;
		rapidjson::PrettyWriter<decltype(buff)> writer{ buff };
		main.Accept(writer);

		std::string json{ buff.GetString() };
		utils::WriteFile(utils::GetProgDir() / CONFIG_FILE, json);
	}

#ifndef CI_BUILD
	int cfgtest(Process& proc, int argc, const char* argv[]) {

		SyncConfig();

		LOG_INFO("{}", GetInteger("test", 64));
		LOG_INFO("{}", GetBool("test2.bool.val", true));
		LOG_INFO("{}", GetInteger("test2.int", 64));
		LOG_INFO("{}", GetDouble("zqdzqd.dzqdzq.zdqzdh.hqzdz", 42.69));
		LOG_INFO("{}", GetString("str", "dzqzqdzdqzdq"));

		LOG_INFO("{}", GetString("ui.injector.path"));

		main["test3"] = 42;

		LOG_INFO("Key:");
		for (auto& [key, val] : main.GetObj()) {
			LOG_INFO("- {}", key.GetString());
		}

		SetString("zdzdq", "qzddqzqzdqzd");

		SaveConfig();

		return tool::OK;
	}
	ADD_TOOL("cfgtest", "dev", "", "", nullptr, cfgtest);
#endif
}
