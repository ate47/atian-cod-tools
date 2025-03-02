#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct ZivArt {
		uint64_t name;
		byte* unk8;
		uint64_t* unk10;
		uint32_t* unk18;
		XModel* model20;
		uint64_t unk28;
		unsigned int unk8_count;
		unsigned int unk10_count;
		unsigned int unk18_count;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			ZivArt data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}

			std::filesystem::path loc = ctx.outDir / "tables" / "zivar" / std::format("{}.json", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(data.name);
			json.WriteFieldNameString("unk28");
			json.WriteValueNumber(data.unk28);
			json.WriteFieldNameString("unk8_count");
			json.WriteValueNumber(data.unk8_count);
			json.WriteFieldNameString("unk10_count");
			json.WriteValueNumber(data.unk10_count);
			json.WriteFieldNameString("unk18_count");
			json.WriteValueNumber(data.unk18_count);
			if (data.model20) {
				json.WriteFieldNameString("model20");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.model20)));
			}

			json.EndObject();


			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_ZIVART };
}