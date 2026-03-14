#pragma once

namespace fastfile::names_store {
	class NamesStore {
		uint64_t(*HashAsset)(const char* name);
		std::unordered_map<uint64_t, size_t> assets{};

	public:
		NamesStore(uint64_t(*HashAsset)(const char* name)) 
			: HashAsset(HashAsset) {}

		void LoadConfig(const char* cfg) {
			assets.clear();
			if (!cfg || !*cfg) {
				return; // empty
			}

			std::string v{ cfg };
			char* data{ v.data() };

			size_t idx{};
			while (idx < v.size()) {
				size_t next{ v.find(',', idx) };

				if (next == std::string::npos) {
					next = v.size();
				}

				data[next] = 0; // replace , or \0

				const char* name{ &data[idx] };
				uint64_t out;
				if (!(hash::TryHashPattern(name, out) && out)) {
					out = HashAsset(name); // can't reuse, use fonction
					hashutils::AddPrecomputed(out, name);
				}

				assets[out] = 0;

				idx = next + 1;
			}
		}

		bool Contains(uint64_t name, bool addRef) {
			if (assets.empty()) {
				return true; // all by default
			}

			auto it{ assets.find(name) };
			if (it == assets.end()) {
				return false;
			}
			if (addRef) {
				it->second++;
			}
			return true;
		}

		void WarnMissings() {
			for (const auto& [k, v] : assets) {
				if (!v) {
					LOG_WARNING("Unref asset {}", hashutils::ExtractTmp("hash", k));
				}
			}
		}
	};
}