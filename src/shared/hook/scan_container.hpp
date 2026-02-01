#pragma once
#include <hook/library.hpp>

namespace hook::scan_container {
	std::filesystem::path GetContainersPath();
	
	std::filesystem::path GetContainerPath(uint32_t uid);
	
	inline std::filesystem::path GetContainerPath(const hook::library::Library& lib) {
		return GetContainerPath(lib.GetUID());
	}

	void CleanContainers();

	struct ResultValue {
		bool loaded{};
		std::vector<uint32_t> res{};
	};

	class ScanContainer {
		hook::library::Library lib{};
		uint32_t uid{};
		std::filesystem::path scanPath;
		std::unordered_map<uint64_t, ResultValue> results{};
		bool savePost;
		bool sync{};
		bool anyUpdate{};

	public:
		bool ignoreMissing{};
		bool foundMissing{};

		ScanContainer(bool savePost = false) : savePost(savePost) {
		}

		ScanContainer(hook::library::Library lib, bool savePost = false)
			: savePost(savePost) {
			Load(lib);
		}
		~ScanContainer() {
			if (savePost) Save();
		}

		void Load(hook::library::Library lib);
		void Sync(bool force = false);
		void Save();

		std::vector<hook::library::ScanResult> Scan(const char* path, const char* name = nullptr);
		hook::library::ScanResult ScanSingle(const char* path, const char* name = nullptr);
		hook::library::ScanResult ScanAny(const char* path, const char* name = nullptr);

		inline const hook::library::Library& GetLibrary() const {
			return lib;
		}
	};
}