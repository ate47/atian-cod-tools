#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	constexpr uint32_t MaterialHash(const char* name) {
		uint32_t h{};

		while (*name) {
			h = (33 * h) ^ (*(name++) | 0x20);
		}

		return h;
	}

	struct MaterialHashLookup {
		std::unordered_map<uint32_t, const char*> map{};

		void Init() {
			if (!map.empty()) return;

			static const char* names[]{
				"colorMap",
				"detailMap",
				"glossMap",
				"heightMap",
				"thermalHeatmap",
				"normalMap",
				"revealMap",
				"specColorMap",
				"tintMask",
				"$white_ao",
				"$white_diffuse",
				"$white_gloss",
				"$white_reveal",
				"detailNormal1",
				"detailNormal2",
				"detailNormal3",
				"detailNormal4",
				"detailNormalMask",
				"aoMap",
				"lookupTexture",
				"heatLookup",
				"transGlossMap",
				"tintMask",
				"glossBodyMap",
				"thicknessMap",
				"diffuseMap",
				"edgeFadeMap",
			};

			for (const char* n : names) {
				map[MaterialHash(n)] = n;
			}
		}
	} hashLookup;

	const char* LookupHash(uint32_t hash) {
		auto it{ hashLookup.map.find(hash) };
		if (it == hashLookup.map.end()) return nullptr;
		return it->second;
	}

	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		std::unordered_set<uint32_t> unktypes{};

		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			unktypes.clear();
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (unktypes.size()) {
				std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / std::format("unktext_{}.txt", ctx.ffname) };
				std::filesystem::create_directories(outFile.parent_path());
				utils::OutFileCE os{ outFile, true };

				for (uint32_t h : unktypes) {
					os << std::hex << "0x" << h << "\n";
				}
			}
		}
		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			struct MaterialTechnique;
			struct MaterialTechniqueSet;
			struct MaterialTextureDef;
			struct MaterialSamplerDef;
			struct MaterialUnkA040;

			struct MaterialUnkA0 {
				uint64_t unk0;
				uint64_t unk8;
				uint64_t unk10;
				uint64_t unk18;
				uint64_t unk20;
				uint64_t unk28;
				MaterialUnkA040* unk30[12];
				uint64_t datalen;
				byte* data;
			}; static_assert(sizeof(MaterialUnkA0) == 0xa0);

			struct MaterialTextureDef {
				GfxImage* image;
				uint32_t nameHash;
				vec2_t uvScale;
				byte texSizeShift[2];
				uint16_t uvOffset[3];
				uint16_t usage;
				byte isMatureContent;
			}; static_assert(sizeof(MaterialTextureDef) == 0x20);


			struct Material {
				XHash name;
				uint64_t unk10;
				uint64_t unk18;
				uint64_t unk20;
				uint64_t unk28;
				MaterialTechniqueSet* techniqueSet;
				MaterialTextureDef* textureTable;
				MaterialSamplerDef* samplerTable;
				uint64_t unk48;
				uint64_t unk50;
				uint64_t unk58;
				uint64_t unk60;
				uint64_t unk68;
				uint64_t unk70;
				uint64_t unk78;
				uint64_t unk80;
				uint64_t unk88;
				MaterialUnkA0 unk90;
				byte textureCount;
				byte samplerCount;
				byte unk132;
				byte unk133;
			}; static_assert(sizeof(Material) == 0x138);
			sizeof(Material);

			Material* asset{ (Material*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / "material" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump material {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("techniqueSet", games::bo4::pool::XAssetType::ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet);

			json.WriteFieldNameString("textureTable");
			json.BeginArray();
			for (size_t i = 0; i < asset->textureCount; i++) {
				MaterialTextureDef* def{ asset->textureTable + i };
				json.BeginObject();
				json.WriteFieldValueXAsset("image", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, def->image);
				const char* h{ LookupHash(def->nameHash) };
				if (!h) {
					unktypes.insert(def->nameHash);
					h = utils::va("unk_%x", def->nameHash);
				}
				json.WriteFieldValueString("type", h);
				json.WriteFieldValueString("uvScale", std::format("({}, {})", def->uvScale[0], def->uvScale[1]));
				json.WriteFieldValueString("texSizeShift", std::format("({}, {})", (int)def->texSizeShift[0], (int)def->texSizeShift[1]));
				json.WriteFieldValueString("uvOffset", std::format("({}, {})", (int)def->uvOffset[0], (int)def->uvOffset[1]));
				json.WriteFieldValueNumber("usage", def->usage);
				json.WriteFieldValueBool("isMatureContent", def->isMatureContent);
				json.EndObject();
			}
			json.EndArray();

			json.EndObject();

			return;

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_MATERIAL, true };
}