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
					serverScripts.emplace_back(hash::Hash64(e.GetString()));
				}
			}
			if (forcedConfig.main.HasMember("client")) {
				for (auto& e : forcedConfig.main["client"].GetArray()) {
					if (!e.IsString()) {
						LOG_WARNING("Invalid entry in sptforced.json: {}", e.GetString());
						continue;
					}
					clientScripts.emplace_back(hash::Hash64(e.GetString()));
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
				XHash* gscScripts;
				XHash* cscScripts;
			};
			ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREEFORCED, fastfile::linker::data::POINTER_NEXT);

			ctx.data.PushStream(XFILE_BLOCK_TEMP);
			ScriptParseTreeForced header{};
			header.name.name = ctx.ffnameHash;
			header.gscCount = (uint32_t)serverScripts.size();
			header.cscCount = (uint32_t)clientScripts.size();
			if (serverScripts.size()) header.gscScripts = (XHash*)fastfile::linker::data::POINTER_NEXT;
			if (clientScripts.size()) header.cscScripts = (XHash*)fastfile::linker::data::POINTER_NEXT;

			// todo: implement gdb compiler
			ctx.data.WriteData(header);

			ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ctx.data.Align(8);
			ctx.data.WriteData(serverScripts.data(), serverScripts.size() * sizeof(serverScripts[0]));
			ctx.data.Align(8);
			ctx.data.WriteData(clientScripts.data(), clientScripts.size() * sizeof(clientScripts[0]));
			ctx.data.PopStream();

			ctx.data.PopStream();
			LOG_INFO("Added asset scriptparsetreeforced {} (hash_{:x})", forcedConfigPath.string(), ctx.ffnameHash);
		}
	};

	utils::ArrayAdder<ScriptParseTreeForcedWorker, LinkerWorker> impl{ GetWorkers() };
}