#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct NetConstStrings {
		uint64_t name;
		uint64_t unk8;
		uint32_t unk10;
		uint32_t stringsCount;
		uint64_t unk18;
		char** strings;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			NetConstStrings data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}

			std::filesystem::path loc = ctx.outDir / "tables" / "netconststrings" / std::format("{}.json", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(data.name);
			json.WriteFieldNameString("unk8"); json.WriteValueNumber(data.unk8);
			json.WriteFieldNameString("unk18"); json.WriteValueNumber(data.unk18);
			//json.WriteFieldNameString("unk10"); json.WriteValueNumber(data.unk10);
			json.WriteFieldNameString("strings");
			json.BeginArray();
			if (data.stringsCount) {
				auto strings{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.strings), data.stringsCount) };
				for (size_t i = 0; i < data.stringsCount; i++) {
					json.WriteValueString(opt.AddString(proc.ReadStringTmp(strings[i])));
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
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_NETCONSTSTRINGS };
}