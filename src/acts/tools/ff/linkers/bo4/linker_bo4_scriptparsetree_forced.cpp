#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class ScriptParseTreeForcedWorker : public LinkerWorker {
	public:
		ScriptParseTreeForcedWorker() : LinkerWorker("ScriptParseTreeForced") {}

		void Compute(BO4LinkContext& ctx) override {

			std::filesystem::path forcedConfigPath{ ctx.linkCtx.input / "sptforced.json" };
			core::config::Config forcedConfig{ forcedConfigPath };
			forcedConfig.SyncConfig(false);
			if (forcedConfig.main.Empty()) return; // nothing to force

			std::vector<XHash> serverScripts{};
			std::vector<XHash> clientScripts{};

			if (forcedConfig.main.HasMember("server")) {
				for (auto& e : forcedConfig.main["server"].GetArray()) {
					if (!e.IsString()) {
						LOG_WARNING("Invalid entry in sptforced.json: {}", e.GetString());
						continue;
					}
					serverScripts.emplace_back(hash::Hash64Pattern(e.GetString()), 0);
				}
			}
			if (forcedConfig.main.HasMember("client")) {
				for (auto& e : forcedConfig.main["client"].GetArray()) {
					if (!e.IsString()) {
						LOG_WARNING("Invalid entry in sptforced.json: {}", e.GetString());
						continue;
					}
					clientScripts.emplace_back(hash::Hash64Pattern(e.GetString()), 0);
				}
			}

			if (clientScripts.empty() && serverScripts.empty()) {
				LOG_INFO("Nothing to compile for spt forced: {}", forcedConfigPath.string());
				return; // nothing to compile
			}

			struct ScriptParseTreeForced {
				XHash name;
				uint32_t gscCount;
				uint32_t cscCount;
				uintptr_t gscScripts; // XHash*
				uintptr_t cscScripts; // XHash*
			};

			ScriptParseTreeForced& header{ utils::Allocate<ScriptParseTreeForced>(ctx.assetData) };
			header.name.hash = ctx.ffnameHash;
			header.gscCount = (uint32_t)serverScripts.size();
			header.cscCount = (uint32_t)clientScripts.size();
			if (serverScripts.size()) header.gscScripts = fastfile::ALLOC_PTR;
			if (clientScripts.size()) header.cscScripts = fastfile::ALLOC_PTR;

			utils::WriteValue(ctx.assetData, serverScripts.data(), serverScripts.size() * sizeof(serverScripts[0]));
			utils::WriteValue(ctx.assetData, clientScripts.data(), clientScripts.size() * sizeof(clientScripts[0]));
			ctx.assets.emplace_back(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREEFORCED, fastfile::ALLOC_PTR);
			LOG_INFO("Added asset scriptparsetreeforced {} (hash_{:x})", forcedConfigPath.string(), ctx.ffnameHash);
		}
	};

	utils::ArrayAdder<ScriptParseTreeForcedWorker, LinkerWorker> impl{ GetWorkers() };
}