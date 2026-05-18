#pragma once
#include <utils/utils.hpp>
#if __has_include(<rapidjson/document.h>)
#include <rapidjson/document.h>
#define __ACTS_COMPRESS_HAS_RAPIDJSON
#endif

#if __has_include(<magic_enum/magic_enum.hpp>)
#include <magic_enum/magic_enum.hpp>
#define __ACTS_COMPRESS_HAS_MAGIC_ENUM
#endif


/*
 * Config utilities, search for a path inside the config file, a path is delimited by '.',
 * example for:
 * { "test": { "test2": 42 } }
 * 42 is at the path "test.test2"
 */
namespace core::config {
	constexpr const char* MAIN_CONFIG_FILE = "acts.json";

	struct ConfigEnumData {
		const char* name;
		int64_t enumValue;
	};


	int64_t ParseEnumValue(const char* value, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = {}, const char* path = nullptr);

	template <typename T>
	inline T ParseEnumValue(const char* value, const ConfigEnumData* data, size_t dataCount, T defaultEnumValue = {}, const char* path = nullptr) {
		return (T)ParseEnumValue(value, data, dataCount, (int64_t)defaultEnumValue, path);
	}

#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
	using RapidJsonDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>;
	using RapidJsonGeneric = rapidjson::GenericValue<RapidJsonDocument::EncodingType, RapidJsonDocument::AllocatorType>;
	using RapidJsonGenericArray = rapidjson::GenericArray<false, RapidJsonGeneric>;
	using RapidJsonGenericObject = rapidjson::GenericObject<false, RapidJsonGeneric>;


	bool ParseJson(std::filesystem::path path, RapidJsonDocument& doc);
	void WriteJson(std::filesystem::path path, const RapidJsonGeneric& value);
#endif

	class ConfigGenericRefs {
	public:

#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
		RapidJsonDocument& main;
		RapidJsonGeneric& base;

		ConfigGenericRefs(RapidJsonDocument& main, RapidJsonGeneric& base) : main(main), base(base) {}
		ConfigGenericRefs(const ConfigGenericRefs& other) noexcept : main(other.main), base(other.base) {}
		ConfigGenericRefs(ConfigGenericRefs&& other) noexcept : main(other.main), base(other.base) {}

		RapidJsonGeneric& GetVal(const char* path, size_t off, RapidJsonGeneric& loc);
		void SetVal(const char* path, rapidjson::Value& value, size_t off, RapidJsonGeneric& loc);
#endif

		int64_t GetInteger(const char* path, int64_t defaultValue = 0);
		inline int64_t GetInteger(const std::string& path, int64_t defaultValue = 0) { return GetInteger(path.c_str(), defaultValue); }

		uint64_t GetUnsignedInteger(const char* path, uint64_t defaultValue = 0);
		inline uint64_t GetUnsignedInteger(const std::string& path, uint64_t defaultValue = 0) { return GetUnsignedInteger(path.c_str(), defaultValue); }
		double GetDouble(const char* path, double defaultValue = 0);
		inline double GetDouble(const std::string& path, double defaultValue = 0) { return GetDouble(path.c_str(), defaultValue); }
		const char* GetCString(const char* path, const char* defaultValue = nullptr);
		inline const char* GetCString(const std::string& path, const char* defaultValue = nullptr) { return GetCString(path.c_str(), defaultValue); }
		inline const char* GetCString(const char* path, const std::string& defaultValue) { return GetCString(path, defaultValue.c_str()); }
		inline const char* GetCString(const std::string& path, const std::string& defaultValue) { return GetCString(path.c_str(), defaultValue.c_str()); }
		inline std::string GetString(const char* path, const char* defaultValue = "") {
			return GetCString(path, defaultValue);
		}
		inline std::string GetString(const std::string& path, const char* defaultValue = "") { return GetString(path.c_str(), defaultValue); }
		inline std::string GetString(const char* path, const std::string& defaultValue) { return GetString(path, defaultValue.c_str()); }
		inline std::string GetString(const std::string& path, const std::string& defaultValue) { return GetString(path.c_str(), defaultValue.c_str()); }
		bool GetBool(const char* path, bool defaultValue = false);
		inline bool GetBool(const std::string& path, bool defaultValue = false) { return GetBool(path.c_str(), defaultValue); }
		int64_t GetEnum(const char* path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0);
		inline int64_t GetEnum(const std::string& path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) { return GetEnum(path.c_str(), data, dataCount, defaultEnumValue); }
		template<typename T>
		inline T GetEnumVal(const char* path, const ConfigEnumData* data, size_t dataCount, T defaultEnumValue = {}) {
			return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
		}
		template<typename T>
		inline T GetEnumVal(const std::string& path, const ConfigEnumData* data, size_t dataCount, T defaultEnumValue = {}) {
			return (T)GetEnum(path.c_str(), data, dataCount, (int64_t)defaultEnumValue);
		}
		bool ScanStringN(const char* path, const char* format, size_t count, ...);
		template<typename... Args>
		inline bool ScanString(const char* path, const char* format, Args... args) {
			return ScanStringN(path, format, sizeof...(Args), args...);
		}
		template<typename... Args>
		inline bool ScanString(const std::string& path, const char* format, Args... args) {
			return ScanString<Args...>(path.c_str(), format, args...);
		}

		void SetInteger(const char* path, int64_t defaultValue);
		inline void SetInteger(const std::string& path, int64_t defaultValue) { SetInteger(path.c_str(), defaultValue); }
		void SetUnsignedInteger(const char* path, uint64_t defaultValue);
		inline void SetUnsignedInteger(const std::string& path, uint64_t defaultValue) { SetUnsignedInteger(path.c_str(), defaultValue); }
		void SetDouble(const char* path, double defaultValue);
		inline void SetDouble(const std::string& path, double defaultValue) { SetDouble(path.c_str(), defaultValue); }
		void SetString(const char* path, const std::string& defaultValue);
		inline void SetString(const std::string& path, const std::string& defaultValue) { SetString(path.c_str(), defaultValue); }
		void SetBool(const char* path, bool defaultValue);
		inline void SetBool(const std::string& path, bool defaultValue) { SetBool(path.c_str(), defaultValue); }
		void SetEnum(const char* path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount);
		inline void SetEnum(const std::string& path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount) { SetEnum(path.c_str(), enumValue, data, dataCount); }

		operator bool() const;
	};

	class BaseTypeGetters;

	template<typename DefaultTypeGetters = BaseTypeGetters>
	class ConfigGeneric : public ConfigGenericRefs {
	public:
		using ConfigGenericRefs::ConfigGenericRefs;

		ConfigGeneric<DefaultTypeGetters> GetSubVal(const char* path, size_t off, RapidJsonGeneric& loc) {
			return GetSub(GetVal(path, off, loc));
		}

		ConfigGeneric<DefaultTypeGetters> GetSubVal(const char* path) {
			return GetSubVal(path, 0, base);
		}

		ConfigGeneric<DefaultTypeGetters> GetSub(RapidJsonGeneric& loc) {
			return ConfigGeneric<DefaultTypeGetters>{ main, loc };
		}

		template<typename T,
			typename Output = size_t, bool nullValid = true>
		inline Output LoadArray(const char* path, T* array, size_t maxCount, Output* size = nullptr) {
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
			RapidJsonGeneric& root{ GetVal(path, 0, base) };

			if (root.IsNull()) {
				if constexpr (nullValid) {
					return 0;
				}
				throw std::runtime_error(std::format("Found null for '{}', expected array", path));
			}

			if (!root.IsArray()) {
				throw std::runtime_error(std::format("Found non-array for '{}'", path));
			}


			RapidJsonGenericArray cfgArray{ root.GetArray() };

			if (cfgArray.Size() > maxCount) {
				throw std::runtime_error(std::format("Too many values for '{}'", path));
			}

			for (RapidJsonGeneric& member : cfgArray) {
				ConfigGeneric<DefaultTypeGetters> sub{ GetSub(member) };
				using MT = std::remove_reference_t<decltype(*array)>;
				sub.Load<MT>("~", *array);
				++array;
			}

			if (size) {
				*size = (Output)cfgArray.Size();
			}

			return (Output)cfgArray.Size();
#else
			return {};
#endif
		}


		template<typename T>
		inline void Load(const char* path, T& ref, T defaultValue = {})
			requires (!std::is_array_v<T>) {
			DefaultTypeGetters::Load(path, *this, ref, defaultValue);
		}

		template<typename T,
			typename Output = size_t, bool nullValid = true, size_t maxCount>
		inline Output Load(const char* path, T(&array)[maxCount], Output* size = nullptr) {
			return LoadArray<T, Output, nullValid>(path, array, maxCount, size);
		}

		template<typename T>
		inline T Get(const char* path, T defaultValue = {}) {
			T tmp{ defaultValue };
			Load<T>(path, tmp, defaultValue);
			return tmp;
		}

		template<typename T>
		inline T Get(const std::string& path, T defaultValue = {}) {
			return Get<T>(path.c_str(), defaultValue);
		}
	};

#ifdef __ACTS_COMPRESS_HAS_MAGIC_ENUM
	template<typename E>
	constexpr auto CreateConfigEnumDataArray() {
		static_assert(std::is_enum_v<E>);

		constexpr auto names = magic_enum::enum_names<E>();
		constexpr auto values = magic_enum::enum_values<E>();

		std::array<ConfigEnumData, names.size()> table{};

		for (size_t i = 0; i < names.size(); i++) {
			table[i] = ConfigEnumData{
				names[i].data(),
				static_cast<int64_t>(values[i])
			};
		}

		return table;
	}
	template<typename E>
	constexpr auto ConfigEnumDataArray = CreateConfigEnumDataArray<E>();
#endif
	template<class>
	inline constexpr bool always_false_v = false;

	class BaseTypeGetters {
	public:

		template<typename T, typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, T& ref, T defaultValue) {
			static_assert(always_false_v<T>, "Unsupported type for ConfigGenericTypeGetters::Get()");
		}

		template<typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, bool& ref, bool defaultValue) {
			ref = base.GetBool(path, defaultValue);
		}

		template<typename T, typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, T& ref, T defaultValue)
			requires std::is_floating_point_v<T> {
			ref = (T)(base.GetDouble(path, (double)defaultValue));
		}

		template<typename T, typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, T& ref, T defaultValue)
			requires (std::is_integral_v<T>&& std::is_signed_v<T> && !std::is_same_v<T, bool>) {
			ref = (T)(base.GetInteger(path, (int64_t)defaultValue));
		}

		template<typename T, typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, T& ref, T defaultValue)
			requires (std::is_integral_v<T>&& std::is_unsigned_v<T>) {
			ref = (T)(base.GetUnsignedInteger(path, (uint64_t)defaultValue));
		}

		template<typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, const char*& ref, const char* defaultValue) {
			ref = base.GetCString(path, defaultValue);
		}

		template<typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, std::string& ref, std::string defaultValue) {
			ref = base.GetString(path, defaultValue);
		}

		template<typename T, typename CG = ConfigGeneric<BaseTypeGetters>>
		static void Load(const char* path, CG& base, T& ref, T defaultValue)
			requires std::is_enum_v<T> {
#ifdef __ACTS_COMPRESS_HAS_MAGIC_ENUM
			constexpr auto table = ConfigEnumDataArray<T>;
			ref = base.GetEnumVal<T>(path, table.data(), table.size(), defaultValue);
#else
			static_assert(always_false_v<T>,
				"Unsupported enum type without magic_enum");
#endif
		}
	};

	void SetMainConfig(const std::filesystem::path& path);
	const std::filesystem::path& GetMainConfigFile();

	template<typename DefaultTypeGetters = BaseTypeGetters>
	class ConfigDocument : public ConfigGeneric<DefaultTypeGetters> {
	public:
		std::filesystem::path configFile;
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
		RapidJsonDocument main{};
#endif
		ConfigDocument() : ConfigGeneric<DefaultTypeGetters>(main, main) {
			SetConfigPath(GetMainConfigFile());
		}

		ConfigDocument(const std::filesystem::path& path) : ConfigGeneric<DefaultTypeGetters>(main, main) {
			SetConfigPath(path);
		}
		ConfigDocument(const ConfigDocument& other) : ConfigGeneric<DefaultTypeGetters>(this->main, this->main), configFile(other.configFile) {
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
			// Deep copy JSON document
			this->main.CopyFrom(other.main, this->main.GetAllocator());
#endif
		}

		ConfigDocument(ConfigDocument&& other) noexcept : ConfigGeneric<DefaultTypeGetters>(main, main), configFile(std::move(other.configFile)) {
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
			main = std::move(other.main);
#endif
		}

		ConfigDocument& operator=(const ConfigDocument& other) {
			if (this != &other) {
				configFile = other.configFile;
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
				main.CopyFrom(other.main, main.GetAllocator());
#endif
			}
			return *this;
		}

		ConfigDocument& operator=(ConfigDocument&& other) noexcept {
			if (this != &other) {
				configFile = std::move(other.configFile);
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
				main = std::move(other.main);
#endif
			}
			return *this;
		}

		void SetConfigPath(std::filesystem::path path) {
			configFile = path.is_absolute() ? path : (utils::GetProgDir() / path);
		}

		bool SyncConfig(bool save) {
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
			bool r{ ParseJson(utils::GetProgDir() / configFile, main) };

			// not reading isn't an error
			if (save) {
				this->SaveConfig();
			}
			return r;
#else
			return false;
#endif
		}

		void SaveConfig() const {
#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON
			WriteJson(utils::GetProgDir() / configFile, main);
#endif
		}
	};

	using Config = ConfigDocument<BaseTypeGetters>;

	Config& GetMainConfig();

	inline int64_t GetInteger(const char* path, int64_t defaultValue = 0) {
		return GetMainConfig().GetInteger(path, defaultValue);
	}
	inline int64_t GetInteger(const std::string& path, int64_t defaultValue = 0) {
		return GetMainConfig().GetInteger(path, defaultValue);
	}
	inline double GetDouble(const char* path, double defaultValue = 0) {
		return GetMainConfig().GetDouble(path, defaultValue);
	}
	inline double GetDouble(const std::string& path, double defaultValue = 0) {
		return GetMainConfig().GetDouble(path, defaultValue);
	}
	inline const char* GetCString(const char* path, const char* defaultValue = "") {
		return GetMainConfig().GetCString(path, defaultValue);
	}
	inline const char* GetCString(const std::string& path, const char* defaultValue = "") {
		return GetMainConfig().GetCString(path, defaultValue);
	}
	inline std::string GetString(const char* path, const char* defaultValue = "") {
		return GetMainConfig().GetString(path, defaultValue);
	}
	inline std::string GetString(const std::string& path, const char* defaultValue = "") {
		return GetMainConfig().GetString(path, defaultValue);
	}
	inline bool GetBool(const char* path, bool defaultValue = false) {
		return GetMainConfig().GetBool(path, defaultValue);
	}
	inline bool GetBool(const std::string& path, bool defaultValue = false) {
		return GetMainConfig().GetBool(path, defaultValue);
	}
	inline int64_t GetEnum(const char* path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) {
		return GetMainConfig().GetEnum(path, data, dataCount, defaultEnumValue);
	}
	inline int64_t GetEnum(const std::string& path, const ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) {
		return GetMainConfig().GetEnum(path, data, dataCount, defaultEnumValue);
	}
	template<typename T>
	inline T GetEnumVal(const char* path, const ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
		return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
	}
	template<typename T>
	inline T GetEnumVal(const std::string& path, const ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
		return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
	}

	inline void SetInteger(const char* path, int64_t value) {
		GetMainConfig().SetInteger(path, value);
	}
	inline void SetInteger(const std::string& path, int64_t value) {
		GetMainConfig().SetInteger(path, value);
	}

	inline void SetUnsignedInteger(const char* path, int64_t value) {
		GetMainConfig().SetUnsignedInteger(path, value);
	}
	inline void SetUnsignedInteger(const std::string& path, int64_t value) {
		GetMainConfig().SetUnsignedInteger(path, value);
	}
	inline void SetDouble(const char* path, double value) {
		GetMainConfig().SetDouble(path, value);
	}
	inline void SetDouble(const std::string& path, double value) {
		GetMainConfig().SetDouble(path, value);
	}
	inline void SetString(const char* path, const std::string& value) {
		GetMainConfig().SetString(path, value);
	}
	inline void SetString(const std::string& path, const std::string& value) {
		GetMainConfig().SetString(path, value);
	}
	inline void SetBool(const char* path, bool value) {
		GetMainConfig().SetBool(path, value);
	}
	inline void SetBool(const std::string& path, bool value) {
		GetMainConfig().SetBool(path, value);
	}
	inline void SetEnum(const char* path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount) {
		GetMainConfig().SetEnum(path, enumValue, data, dataCount);
	}
	inline void SetEnum(const std::string& path, int64_t enumValue, const ConfigEnumData* data, size_t dataCount) {
		GetMainConfig().SetEnum(path, enumValue, data, dataCount);
	}

	template<typename T>
	inline T Get(const char* path, T defaultValue = {}) {
		return GetMainConfig().Get<T>(path, defaultValue);
	}

	/*
	 * Load and save the config
	 */
	inline bool SyncConfig(bool save) {
		return GetMainConfig().SyncConfig(save);
	}
	/*
	 * Save the config
	 */
	inline void SaveConfig() {
		GetMainConfig().SaveConfig();
	}
}