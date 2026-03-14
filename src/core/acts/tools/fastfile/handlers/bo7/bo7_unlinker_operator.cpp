#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>
#include <tools/fastfile/handlers/bo7/bo7_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo7;

	struct OperatorSkin {
		scriptbundle::ScriptBundleObjectData bundleData;
		XHash64 name;
		XHash64 description;
		XHash64 head;
		XHash64 body;
		XHash64 headModel;
		XHash64 bodyModel;
		XHash64 unk_872d17c7eb1da015;
		XHash64 unk_a21038428e485fbf;
		XHash64 unk60;
		XHash64 gasMask;
		XHash64 bagbackTag;
		uint64_t unk78;
		uint64_t unk80;
		GfxImage* icon;
		const char* unk90;
		XModel* parachuteVM;
		XModel* parachuteWM;
		ParticleSystem* particleSystem;
		float unkb0;
		uint32_t lootid;
		bool unkb8;
		bool unkb9;
		bool unkba;
	};
	static_assert(sizeof(OperatorSkin) == 0xc0);

	struct Operator {
		XHash64 name;
		XHash64 defaultSkin;
		XHash64 unk_2166b3eb7bbab0c9;
		XBoneSet* xboneSet;
		uint64_t unk20;
		uint64_t unk28;
		uint64_t operatorSkinsCount;
		OperatorSkin** operatorSkins;
		GfxImage* portraitImage;
		GfxImage* iconMinimap;
		const char* dialog;
		const char* gender;
		XHash64 genderHash;
		XHash64 gasMask;
		scriptbundle::ScriptBundleObjectData bundleData;
		uint32_t lootid;
		byte unk94;
		bool unk95;
		bool unk96;
		bool unk97;
	};
	static_assert(sizeof(Operator) == 0x98);

	struct ReactiveOperator {
		XHash64 name;
		ReactiveVFXPackage** reactiveVFXPackage1;
		ReactiveVFXPackage** reactiveVFXPackage2;
		ReactiveAudioPackage** reactiveAudioPackage1;
		ReactiveAudioPackage** reactiveAudioPackage2;
		FootStepVFX* footStepVFX;
		int32_t reactiveVFXPackage1Count;
		int32_t reactiveVFXPackage2Count;
		int32_t reactiveAudioPackage1Count;
		int32_t reactiveAudioPackage2Count;
	};
	static_assert(sizeof(ReactiveOperator) == 0x40);

	struct OperatorList {
		XHash64 name;
		XHash64 operatorRefAllies;
		XHash64 operatorRefAxis;
		uint64_t operatorsCount;
		Operator** operators;
		scriptbundle::ScriptBundleObjectData bundleData;
	};
	static_assert(sizeof(OperatorList) == 0x48);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			Operator* asset{ (Operator*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("defaultSkin", asset->defaultSkin);
			json.WriteFieldValueXHash(0x2166b3eb7bbab0c9, asset->unk_2166b3eb7bbab0c9);
			json.WriteFieldValueXHash("gasMask", asset->gasMask);
			json.WriteFieldValueXHash("genderHash", asset->genderHash);
			json.WriteFieldValueXAsset("xboneSet", SATH_ASSET_XBONESET, asset->xboneSet);
			json.WriteFieldValueXAsset("portraitImage", SATH_ASSET_IMAGE, asset->portraitImage);
			json.WriteFieldValueXAsset("iconMinimap", SATH_ASSET_IMAGE, asset->iconMinimap);
			json.WriteFieldValueNumber("lootid", asset->lootid);
			if (asset->dialog) json.WriteFieldValueString("dialog", asset->dialog);
			if (asset->gender) json.WriteFieldValueString("gender", asset->gender);


			//json.WriteFieldValueUnknown("unk20", asset->unk20); //-1?
			json.WriteFieldValueUnknown("unk28", asset->unk28); // unused?
			if (asset->unk94) json.WriteFieldValueBool("unk94", asset->unk94);
			if (asset->unk95) json.WriteFieldValueBool("unk95", asset->unk95);
			if (asset->unk96) json.WriteFieldValueBool("unk96", asset->unk96);
			if (asset->unk97) json.WriteFieldValueBool("unk97", asset->unk97);

			if (asset->operatorSkins) {
				json.WriteFieldNameString("operatorSkins");
				json.BeginArray();
				for (size_t i = 0; i < asset->operatorSkinsCount; i++) {
					OperatorSkin* skin{ asset->operatorSkins[i] };
					if (skin) {
						json.WriteValueHash(GetXAssetName(SATH_ASSET_OPERATORSKIN, skin));
					}
					else {
						json.WriteValueNull();
					}
				}
				json.EndArray();
			}
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundleData);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "operator"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump operator {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplSkinWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			OperatorSkin* asset{ (OperatorSkin*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueLocalized("description", asset->description);
			json.WriteFieldValueXHash("head", asset->head);
			json.WriteFieldValueXHash("body", asset->body);
			json.WriteFieldValueXHash("headModel", asset->headModel);
			json.WriteFieldValueXHash("bodyModel", asset->bodyModel);
			json.WriteFieldValueXHash(0x872d17c7eb1da015, asset->unk_872d17c7eb1da015);
			json.WriteFieldValueXHash(0xa21038428e485fbf, asset->unk_a21038428e485fbf);
			json.WriteFieldValueXHash("unk60", asset->unk60);
			json.WriteFieldValueXHash("gasMask", asset->gasMask);
			json.WriteFieldValueXHash("bagbackTag", asset->bagbackTag);
			json.WriteFieldValueXHash("unk78", asset->unk78);
			json.WriteFieldValueXHash("unk80", asset->unk80);
			json.WriteFieldValueNumber("unkb0", asset->unkb0);
			json.WriteFieldValueNumber("lootid", asset->lootid);

			if (asset->unkb8) json.WriteFieldValueBool("unkb8", asset->unkb8);
			if (asset->unkb9) json.WriteFieldValueBool("unkb9", asset->unkb9);
			if (asset->unkba) json.WriteFieldValueBool("unkba", asset->unkba);
			json.WriteFieldValueXAsset("icon", SATH_ASSET_IMAGE, asset->icon);
			json.WriteFieldValueXAsset("parachuteVM", SATH_ASSET_XMODEL, asset->parachuteVM);
			json.WriteFieldValueXAsset("parachuteWM", SATH_ASSET_XMODEL, asset->parachuteWM);
			json.WriteFieldValueXAsset("particleSystem", SATH_ASSET_PARTICLESYSTEM, asset->particleSystem);
			json.WriteFieldValueXString("unk80", asset->unk90);

			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundleData);
			json.EndObject();

			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "operator" / "skin"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump operatorskin {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplListWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			OperatorList* asset{ (OperatorList*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("operatorRefAllies", asset->operatorRefAllies);
			json.WriteFieldValueXHash("operatorRefAxis", asset->operatorRefAxis);
			if (asset->operators) {
				json.WriteFieldNameString("operatorSkins");
				json.BeginArray();
				for (size_t i = 0; i < asset->operatorsCount; i++) {
					Operator* skin{ asset->operators[i] };
					if (skin) {
						json.WriteValueHash(GetXAssetName(SATH_ASSET_OPERATOR, skin));
					}
					else {
						json.WriteValueNull();
					}
				}
				json.EndArray();
			}
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundleData);
			json.EndObject();

			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "operator" / "list"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump operatorlist {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplReacWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			ReactiveOperator* asset{ (ReactiveOperator*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);

			auto DumpArray = [&json](const char* name, SatHashAssetType type, void** package, size_t count) {
				if (package) {
					json.WriteFieldNameString(name);
					json.BeginArray();
					for (size_t i = 0; i < count; i++) {
						void* p{ package[i] };
						if (p) {
							json.WriteValueHash(GetXAssetName(type, p));
						}
						else {
							json.WriteValueNull();
						}
					}
					json.EndArray();
				}
				};
			DumpArray("reactiveVFXPackage1", SATH_ASSET_REACTIVEVFXPACKAGE, (void**)asset->reactiveVFXPackage1, asset->reactiveVFXPackage1Count);
			DumpArray("reactiveVFXPackage2", SATH_ASSET_REACTIVEVFXPACKAGE, (void**)asset->reactiveVFXPackage2, asset->reactiveVFXPackage2Count);
			DumpArray("reactiveAudioPackage1", SATH_ASSET_REACTIVEAUDIOPACKAGE, (void**)asset->reactiveAudioPackage1, asset->reactiveAudioPackage1Count);
			DumpArray("reactiveAudioPackage2", SATH_ASSET_REACTIVEAUDIOPACKAGE, (void**)asset->reactiveAudioPackage2, asset->reactiveAudioPackage2Count);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "operator" / "reactive"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump reactiveoperator {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_OPERATOR, sizeof(Operator) };
	utils::MapAdder<ImplSkinWorker, SatHashAssetType, Worker> impls{ GetWorkers(), SatHashAssetType::SATH_ASSET_OPERATORSKIN, sizeof(OperatorSkin) };
	utils::MapAdder<ImplReacWorker, SatHashAssetType, Worker> implr{ GetWorkers(), SatHashAssetType::SATH_ASSET_REACTIVEOPERATOR, sizeof(ReactiveOperator) };
	utils::MapAdder<ImplListWorker, SatHashAssetType, Worker> impll{ GetWorkers(), SatHashAssetType::SATH_ASSET_OPERATORLIST, sizeof(OperatorList) };
}