#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class ScriptParseTreeForcedWorker : public LinkerWorker {
	public:
		ScriptParseTreeForcedWorker() : LinkerWorker("ScriptParseTreeForced") {}

		void Compute(BO4LinkContext& ctx) override {

			std::filesystem::path forcedConfigPath{ ctx.linkCtx.input / "sptforced.csv" };
			std::string buffer{};

			if (!utils::ReadFile(forcedConfigPath, buffer)) {
				LOG_WARNING("Can't read csv {}", forcedConfigPath.string());
				return;
			}

			rapidcsv::Document doc{};

			std::stringstream stream{ buffer };

			doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

			if (doc.GetColumnCount() < 2) {
				LOG_WARNING("Can't read hash csv {}: invalid file", forcedConfigPath.string());
				return;
			}

			std::vector<XHash> serverScripts{};
			std::vector<XHash> clientScripts{};

			for (size_t i = 0; i < doc.GetRowCount(); i++) {
				const std::string type{ doc.GetCell<std::string>(0, i) };
				const std::string scriptStr{ doc.GetCell<std::string>(1, i) };

				if (type == "server") {
					serverScripts.emplace_back(hash::Hash64(scriptStr.data()));
				}
				else if (type == "client") {
					clientScripts.emplace_back(hash::Hash64(scriptStr.data()));
				}
				else {
					LOG_WARNING("Invalid entry in sptforced.csv: unknown type: {}", type);
					continue;
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