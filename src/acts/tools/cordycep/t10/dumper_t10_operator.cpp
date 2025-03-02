#include <includes.hpp>
#include <tools/pool.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/cordycep/t10/dumper_t10_scriptbundle.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace tool::cordycep::dump::t10::scriptbundle;
	using namespace bo6;


	struct OperatorSkin {
		uint64_t name;
		uint64_t description;
		uint64_t head;
		uint64_t body;
		GfxImage* image;
		sbv2::ScriptBundleObjectData bundleData;
		uint64_t unk48;
		uint64_t unk50;
		uint64_t unk58;
		const char* unk60;
		XModel* model68;
		XModel* model70;
	}; static_assert(sizeof(OperatorSkin) == 0x78);

	struct Operator {
		uint64_t name;
		uint64_t defaultSkin;
		uint64_t operatorSkinsCount;
		OperatorSkin** operatorSkins;
		GfxImage* portraitImage;
		GfxImage* iconMinimap;
		const char* dialog;
		sbv2::ScriptBundleObjectData bundleData;
		uint32_t unk58;
		byte unk5c;
		bool unk5d;
		bool unk5e;
		bool unk5f;
	}; static_assert(sizeof(Operator) == 0x60);

	struct OperatorList {
		uint64_t name;
		uint64_t operatorRefAllies;
		uint64_t operatorRefAxis;
		uint64_t operatorsCount;
		Operator** operators;
		sbv2::ScriptBundleObjectData bundleData;
	}; static_assert(sizeof(OperatorList) == 0x48);

	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			Operator data;

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} header ", hashutils::ExtractTmp("hash", asset.ID));
				return false;
			}

			std::filesystem::path loc{ ctx.outDir / "tables" / "operator" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			json.BeginObject();
			{
				json.WriteFieldNameString("name");
				json.WriteValueHash(data.name);
				json.WriteFieldNameString("defaultSkin");
				json.WriteValueHash(data.defaultSkin, "%#");
				json.WriteFieldNameString("dialog");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.dialog))));
				if (data.portraitImage) {
					json.WriteFieldNameString("portraitImage");
					json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.portraitImage)));
				}
				if (data.iconMinimap) {
					json.WriteFieldNameString("iconMinimap");
					json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.iconMinimap)));
				}
				//json.WriteFieldNameString("unk58");
				//json.WriteValueHash(data.unk58);
				//json.WriteFieldNameString("unk5c");
				//json.WriteValueHash(data.unk5c);
				json.WriteFieldNameString("skins");
				json.BeginArray();
				if (data.operatorSkinsCount) {
					auto operators{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.operatorSkins), data.operatorSkinsCount) };
					for (size_t i = 0; i < data.operatorSkinsCount; i++) {
						json.WriteValueHash(proc.ReadMemory<uint64_t>(operators[i]));
					}
				}
				json.EndArray();

				json.WriteFieldNameString("data");
				if (!sbv2::WriteBundleData(opt, json, proc, data.bundleData, ctx.GetLocalized)) {
					LOG_ERROR("Error when loading bundle data");
					return false;
				}
			}
			json.EndObject();

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}

			return true;
		}

	};

	class UnlinkerSkinImpl : public Unlinker {
	public:
		UnlinkerSkinImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			OperatorSkin data;

			if (sizeof(data) != asset.HeaderSize) {
				LOG_ERROR("Invalid header size for {} 0x{:x} != 0x{:x} for 0x{:x}", hashutils::ExtractTmp("hash", asset.ID), sizeof(data), asset.HeaderSize, asset.Type);
				std::cout << "--------------------\n";
				auto d{ proc.ReadMemoryArrayEx<byte>(asset.Header, asset.HeaderSize) };
				tool::pool::WriteHex(std::cout, 0, d.get(), asset.HeaderSize, proc);
				std::cout << "--------------------\n";
				return false;
			}

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} 0x{:x} header", hashutils::ExtractTmp("hash", asset.ID), asset.Type);
				return false;
			}

			std::filesystem::path loc{ ctx.outDir / "tables" / "operator" / "skin" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			json.BeginObject();
			{
				/*
				

					struct OperatorSkin {
						uint64_t unk48;
						uint64_t unk50;
						uint64_t unk58;
					}; static_assert(sizeof(OperatorSkin) == 0x78);

				
				
				*/
				json.WriteFieldNameString("name");
				json.WriteValueHash(data.name);
				json.WriteFieldNameString("description");
				json.WriteValueString(std::format("r#{}", ctx.GetLocalized(data.description)));
				json.WriteFieldNameString("head");
				json.WriteValueHash(data.head, "%#");
				json.WriteFieldNameString("body");
				json.WriteValueHash(data.body, "%#");
				//json.WriteFieldNameString("unk48");
				//json.WriteValueHash(data.unk48, "%#");
				//json.WriteFieldNameString("unk50");
				//json.WriteValueHash(data.unk50, "%#");
				//json.WriteFieldNameString("unk48");
				//json.WriteValueHash(data.unk48, "%#");
				if (data.unk60) {
					json.WriteFieldNameString("unk60");
					json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.unk60))));
				}
				if (data.image) {
					json.WriteFieldNameString("image");
					json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.image)));
				}
				if (data.model68) {
					json.WriteFieldNameString("model68");
					json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.model68)));
				}
				if (data.model70) {
					json.WriteFieldNameString("model70");
					json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.model70)));
				}

				json.WriteFieldNameString("data");
				if (!sbv2::WriteBundleData(opt, json, proc, data.bundleData, ctx.GetLocalized)) {
					LOG_ERROR("Error when loading bundle data");
					return false;
				}
			}
			json.EndObject();

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}

			return true;
		}

	};

	class UnlinkerListImpl : public Unlinker {
	public:
		UnlinkerListImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			OperatorList data;

			if (sizeof(data) != asset.HeaderSize) {
				LOG_ERROR("Invalid header size for {} 0x{:x} != 0x{:x} for 0x{:x}", hashutils::ExtractTmp("hash", asset.ID), sizeof(data), asset.HeaderSize, asset.Type);
				std::cout << "--------------------\n";
				auto d{ proc.ReadMemoryArrayEx<byte>(asset.Header, asset.HeaderSize) };
				tool::pool::WriteHex(std::cout, 0, d.get(), asset.HeaderSize, proc);
				std::cout << "--------------------\n";
				return false;
			}

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} 0x{:x} header", hashutils::ExtractTmp("hash", asset.ID), asset.Type);
				return false;
			}

			std::filesystem::path loc{ ctx.outDir / "tables" / "operator" / "list" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			json.BeginObject();
			{
				json.WriteFieldNameString("name");
				json.WriteValueHash(data.name);
				json.WriteFieldNameString("operatorRefAllies");
				json.WriteValueHash(data.operatorRefAllies);
				json.WriteFieldNameString("operatorRefAxis");
				json.WriteValueHash(data.operatorRefAxis);

				json.WriteFieldNameString("data");
				if (!sbv2::WriteBundleData(opt, json, proc, data.bundleData, ctx.GetLocalized)) {
					LOG_ERROR("Error when loading bundle data");
					return false;
				}
			}
			json.EndObject();

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_OPERATOR };
	utils::MapAdder<UnlinkerSkinImpl, bo6::T10RAssetType, Unlinker> impls{ GetUnlinkers(), bo6::T10R_ASSET_OPERATORSKIN };
	utils::MapAdder<UnlinkerListImpl, bo6::T10RAssetType, Unlinker> impll{ GetUnlinkers(), bo6::T10R_ASSET_OPERATORLIST };
}