#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	struct PhysConstraints;
	struct XModelMesh;
	struct Material;
	struct XModel;
	typedef float vec_t;


	struct XModelMeshMaterials {
		uint16_t numMaterials;
		Material** materials;
		vec_t* himipInvSqRadii;
	}; static_assert(sizeof(XModelMeshMaterials) == 0x18);

	struct XSubmodel {
		XModel* model;
		ScrString_t boneName;
		vec3_t trans;
		vec3_t rot;
	}; static_assert(sizeof(XSubmodel) == 0x28);

	struct Unk18XModel {
		ScrString_t unk0;
		uint32_t unk4;
	}; static_assert(sizeof(Unk18XModel) == 8);

	struct __declspec(align(8)) XModel
	{
		XHash name;
		ScrString_t* boneNames;
		Unk18XModel* unk18;
		byte* unk20;
		uint64_t* unk28;
		void* unk30;
		byte* unk38;
		void* unk40;
		uint64_t unk48;
		uint64_t unk50;
		uint64_t unk58;
		uint64_t unk60;
		PhysConstraints* physConstraints;
		XSubmodel* submodels;
		XModelMesh* meshes[8];
		XModelMeshMaterials* meshMaterials;
		uint64_t unkc0;
		uint32_t numLods;
		uint32_t unkcc;
		uint64_t unkd0;
		uint64_t unkd8;
		uint64_t unke0;
		uint64_t unke8;
		uint64_t unkf0;
		uint64_t unkf8;
		uint64_t unk100;
		uint64_t unk108;
		uint64_t unk110;
		uint64_t unk118;
		uint64_t unk120;
		uint64_t unk128;
		uint64_t unk130;
		uint64_t unk138;
		int32_t unk140;
		uint16_t numCosmeticBones;
		uint16_t unk146;
		byte unk148;
		byte unk149;
		int8_t numSubmodels;
		byte numUnk14b;
		byte numUnk14c;
		byte unk14d;
		byte unk14e;
		byte unk14f;
	}; static_assert(sizeof(XModel) == 0x150);


	class ImplWorker : public Worker {
		using Worker::Worker;

		size_t max = 100;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {

			XModel* asset{ (XModel*)ptr };

			if (!asset->numSubmodels) return; // ignore

			std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / "xmodel" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump xmodel {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("physConstraints", games::bo4::pool::XAssetType::ASSET_TYPE_PHYSCONSTRAINTS, asset->physConstraints);
			json.WriteFieldValueNumber("numCosmeticBones", (int)asset->numCosmeticBones);
			json.WriteFieldValueNumber("unk146", (int)asset->unk146);
			json.WriteFieldValueNumber("unk148", (int)asset->unk148);
			json.WriteFieldValueNumber("unk149", (int)asset->unk149);
			json.WriteFieldValueNumber("numSubmodels", (int)asset->numSubmodels);
			json.WriteFieldValueNumber("numUnk14b", (int)asset->numUnk14b);
			json.WriteFieldValueNumber("numUnk14c", (int)asset->numUnk14c);
			json.WriteFieldValueNumber("unk14d", (int)asset->unk14d);
			json.WriteFieldValueNumber("unk14e", (int)asset->unk14e);
			json.WriteFieldValueNumber("unk14f", (int)asset->unk14f);

			json.WriteFieldValueXAssetArray("meshes", games::bo4::pool::XAssetType::ASSET_TYPE_XMODELMESH, ARRAYSIZE(asset->meshes), asset->meshes);

			json.WriteFieldNameString("bones");
			json.BeginArray();

			size_t bones{ (size_t)asset->numCosmeticBones + asset->numUnk14b };

			for (size_t i = 0; i < bones; i++) {
				json.WriteValueString(GetScrString(asset->boneNames[i]));
			}

			json.EndArray();

			json.WriteFieldNameString("submodels");
			json.BeginArray();
			for (size_t i = 0; i < asset->numSubmodels; i++) {
				XSubmodel* sub{ asset->submodels + i };
				json.BeginObject();
				json.WriteFieldValueXAsset("model", games::bo4::pool::XAssetType::ASSET_TYPE_XMODEL, sub->model);
				json.WriteFieldValueScrString("boneName", sub->boneName);
				json.WriteFieldValueString("translation", std::format("({}, {}, {})", sub->trans[0], sub->trans[1], sub->trans[2]));
				json.WriteFieldValueString("rotation", std::format("({}, {}, {})", sub->rot[0], sub->rot[1], sub->rot[2]));
				json.EndObject();
			}
			json.EndArray();

			json.WriteFieldNameString("meshMaterials");
			json.BeginArray();
			for (size_t i = 0; i < asset->numLods; i++) {
				XModelMeshMaterials* mat{ asset->meshMaterials + i };
				json.BeginArray();
				for (size_t j = 0; j < mat->numMaterials; j++) {
					json.BeginObject();
					json.WriteFieldValueXAsset("material", games::bo4::pool::XAssetType::ASSET_TYPE_MATERIAL, mat->materials[j]);
					json.WriteFieldValueNumber("himipInvSqRadii", mat->himipInvSqRadii[j]);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndArray();

			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (--max <= 1) *(byte*)(nullptr) = 1;
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_XMODEL, true };
}