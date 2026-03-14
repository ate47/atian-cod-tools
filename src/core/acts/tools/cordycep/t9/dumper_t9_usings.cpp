#include <includes.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/cordycep/t9/dumper_t9.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t9;
	using namespace cw;

	class UnlinkerImpl : public Unlinker {
	public:
		const char* type{};
		UnlinkerImpl(const char* type) : type(type) {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };

			struct ScriptUsing {
				uint64_t name;
				uint64_t serverName;
				uint64_t clientName;
			};

			ScriptUsing entry{};

			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read {}", hashutils::ExtractTmp("hash", asset.ID));
			}

			const char* name{ hashutils::ExtractPtr(entry.name) };

			std::filesystem::path loc;

			if (name) {
				loc = ctx.outDir / "tables" / "usings" / type / std::format("{}.json", name);
			}
			else {
				const char* ext;
				if (entry.serverName) {
					ext = "gsc";
				}
				else {
					ext = "csc";
				}

				loc = ctx.outDir / "tables" / "usings" / type / "hashed" / std::format("file_{:x}.{}.json", entry.name, ext);
			}

			utils::raw_file_extractor::JsonWriter json{};

			json.BeginObject();
			json.WriteFieldNameString("name"); json.WriteValueHash(entry.name);
			if (entry.serverName) { json.WriteFieldNameString("serverName"); json.WriteValueHash(entry.serverName); }
			if (entry.clientName) { json.WriteFieldNameString("clientName"); json.WriteValueHash(entry.clientName); }
			json.EndObject();


			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write to {}", loc.string());
				return false;
			}
			LOG_INFO("Dump {}", loc.string());
			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> impl{ GetUnlinkers(), cw::ASSET_TYPE_SCRIPT_USING, "common"};
	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> implcp{ GetUnlinkers(), cw::ASSET_TYPE_SCRIPT_USING_CP, "cp" };
	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> implmp{ GetUnlinkers(), cw::ASSET_TYPE_SCRIPT_USING_MP, "mp" };
	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> implwz{ GetUnlinkers(), cw::ASSET_TYPE_SCRIPT_USING_WZ, "wz" };
	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> implzm{ GetUnlinkers(), cw::ASSET_TYPE_SCRIPT_USING_ZM, "zm" };
}
