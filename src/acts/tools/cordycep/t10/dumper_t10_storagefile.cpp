#include <includes.hpp>
#include <tools/pool.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct StorageFile {
		uint64_t name;
		const char* nameStr;
		const char* cfgFile;
		const char* ddlFile;
		const char* type;
		uint32_t unk28;
		int32_t unk30_count;
		uint64_t* unk30;
		uint64_t unk38;
		uint64_t unk40;
		uint64_t unk48;
		uint64_t unk50;
		const char* env;
		uint64_t unk60;
	};

	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			StorageFile data;

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} header ", hashutils::ExtractTmp("hash", asset.ID));
				return false;
			}

			if (data.nameStr) {
				opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.nameStr)));
			}

			std::filesystem::path loc{ ctx.outDir / "tables" / "storagefile" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(data.name);
			json.WriteFieldNameString("nameStr");
			json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.nameStr))));
			json.WriteFieldNameString("cfgFile");
			json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.cfgFile))));
			json.WriteFieldNameString("ddlFile");
			json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.ddlFile))));
			json.WriteFieldNameString("type");
			json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.type))));
			if (data.env) {
				json.WriteFieldNameString("env");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.env))));
			}

			json.WriteFieldNameString("unk30");
			json.BeginArray();
			if (data.unk30 && data.unk30_count) {
				auto unk30{ proc.ReadMemoryArrayEx<uint64_t>(reinterpret_cast<uintptr_t>(data.unk30), data.unk30_count) };
				for (size_t i = 0; i < data.unk30_count; i++) {
					json.WriteValueHash(unk30[i]);
				}
			}
			json.EndArray();

			json.EndObject();


			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_STORAGEFILE };
}