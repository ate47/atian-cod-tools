#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo6;

	struct ParticleSystem;
	struct GfxImage;
	struct XModel;
	struct XBoneSet;
	struct ReactiveAudioPackage;
	struct ReactiveVFXPackage;
	struct FootStepVFX;

	struct OperatorSkin {
		scriptbundle::ScriptBundleObjectData bundleData;
		uint64_t name;
		XHash64 description;
		XHash64 head;
		XHash64 body;
		uint64_t unk40;
		XHash64 gasMask;
		XHash64 bagbackTag;
		GfxImage* image;
		const char* unk60;
		XModel* model68;
		XModel* model70;
		ParticleSystem* particleSystem;
		float unk80;
		uint32_t lootid;
		bool unk88;
		bool unk89;
		bool unk8a;
	};
	static_assert(sizeof(OperatorSkin) == 0x90);

	struct Operator {
		XHash64 name;
		XHash64 defaultSkin;
		XBoneSet* xboneSet;
		uint64_t unk18;
		uint64_t unk20;
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
		bool unk8c;
		bool unk8d;
		bool unk8e;
		bool unk8f;
	};
	static_assert(sizeof(Operator) == 0x90);

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
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("defaultSkin", asset->defaultSkin);
			json.WriteFieldValueXHash("gasMask", asset->gasMask);
			json.WriteFieldValueXHash("genderHash", asset->genderHash);
			json.WriteFieldValueXAsset("xboneSet", bo6::T10R_ASSET_XBONESET, asset->xboneSet);
			json.WriteFieldValueXAsset("portraitImage", bo6::T10R_ASSET_IMAGE, asset->portraitImage);
			json.WriteFieldValueXAsset("iconMinimap", bo6::T10R_ASSET_IMAGE, asset->iconMinimap);
			json.WriteFieldValueNumber("lootid", asset->lootid);
			if (asset->dialog) json.WriteFieldValueString("dialog", asset->dialog);
			if (asset->gender) json.WriteFieldValueString("gender", asset->gender);


			//json.WriteFieldValueUnknown("unk18", asset->unk18); //-1?
			json.WriteFieldValueUnknown("unk20", asset->unk20); // unused?
			if (asset->unk8c) json.WriteFieldValueBool("unk8c", asset->unk8c);
			if (asset->unk8d) json.WriteFieldValueBool("unk8d", asset->unk8d);
			if (asset->unk8e) json.WriteFieldValueBool("unk8e", asset->unk8e);
			if (asset->unk8f) json.WriteFieldValueBool("unk8f", asset->unk8f);

			if (asset->operatorSkins) {
				json.WriteFieldNameString("operatorSkins");
				json.BeginArray();
				for (size_t i = 0; i < asset->operatorSkinsCount; i++) {
					OperatorSkin* skin{ asset->operatorSkins[i] };
					if (skin) {
						json.WriteValueHash(GetXAssetName(bo6::T10R_ASSET_OPERATORSKIN, skin));
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
				opt.m_output / "bo6" / "source" / "tables" / "operator"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump operator {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplSkinWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			OperatorSkin* asset{ (OperatorSkin*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("description", asset->description);
			json.WriteFieldValueXHash("head", asset->head);
			json.WriteFieldValueXHash("body", asset->body);
			json.WriteFieldValueUnknown("unk40", asset->unk40);
			json.WriteFieldValueXHash("gasMask", asset->gasMask);
			json.WriteFieldValueXHash("bagbackTag", asset->bagbackTag);
			json.WriteFieldValueNumber("unk80", asset->unk80);
			if (asset->unk88) json.WriteFieldValueBool("unk88", asset->unk88);
			if (asset->unk89) json.WriteFieldValueBool("unk89", asset->unk89);
			if (asset->unk8a) json.WriteFieldValueBool("unk8a", asset->unk8a);
			json.WriteFieldValueNumber("lootid", asset->lootid);
			json.WriteFieldValueXAsset("image", bo6::T10R_ASSET_IMAGE, asset->image);
			json.WriteFieldValueXAsset("model68", bo6::T10R_ASSET_XMODEL, asset->model68);
			json.WriteFieldValueXAsset("model70", bo6::T10R_ASSET_XMODEL, asset->model70);
			json.WriteFieldValueXAsset("particleSystem", bo6::T10R_ASSET_PARTICLESYSTEM, asset->particleSystem);
			if (asset->unk60) json.WriteFieldValueString("unk60", asset->unk60);


			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundleData);
			json.EndObject();

			std::filesystem::path outFile{
				opt.m_output / "bo6" / "source" / "tables" / "operator" / "skin"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump operatorskin {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplListWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			OperatorList* asset{ (OperatorList*)ptr };
			BO6JsonWriter json{};

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
						json.WriteValueHash(GetXAssetName(bo6::T10R_ASSET_OPERATOR, skin));
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
				opt.m_output / "bo6" / "source" / "tables" / "operator" / "list"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump operatorlist {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplReacWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			ReactiveOperator* asset{ (ReactiveOperator*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);

			auto DumpArray = [&json](const char* name, bo6::T10RAssetType type, void** package, size_t count) {
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
			DumpArray("reactiveVFXPackage1", bo6::T10R_ASSET_REACTIVEVFXPACKAGE, (void**)asset->reactiveVFXPackage1, asset->reactiveVFXPackage1Count);
			DumpArray("reactiveVFXPackage2", bo6::T10R_ASSET_REACTIVEVFXPACKAGE, (void**)asset->reactiveVFXPackage2, asset->reactiveVFXPackage2Count);
			DumpArray("reactiveAudioPackage1", bo6::T10R_ASSET_REACTIVEAUDIOPACKAGE, (void**)asset->reactiveAudioPackage1, asset->reactiveAudioPackage1Count);
			DumpArray("reactiveAudioPackage2", bo6::T10R_ASSET_REACTIVEAUDIOPACKAGE, (void**)asset->reactiveAudioPackage2, asset->reactiveAudioPackage2Count);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6" / "source" / "tables" / "operator" / "reactive"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump reactiveoperator {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_OPERATOR };
	utils::MapAdder<ImplSkinWorker, bo6::T10RAssetType, Worker> impls{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_OPERATORSKIN };
	utils::MapAdder<ImplReacWorker, bo6::T10RAssetType, Worker> implr{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_REACTIVEOPERATOR };
	utils::MapAdder<ImplListWorker, bo6::T10RAssetType, Worker> impll{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_OPERATORLIST };
}