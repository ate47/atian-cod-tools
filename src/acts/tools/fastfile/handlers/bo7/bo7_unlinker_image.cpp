#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo7;

	struct GfxImagePacked {
		uint64_t packed;
	};

	struct GfxImageUnk {
		uint64_t id;
		uint64_t unk8;
		GfxImagePacked packed;
	};
	static_assert(sizeof(GfxImageUnk) == 0x18);
	union GfxImageAtlasSize {
		struct {
			byte rowCount;
			byte colCount;
		} unpack;
		uint16_t packed;
	};

	struct GfxImage {
		XHash64 name;
		byte* packedAtlasData;
		uint32_t textureId;
		uint32_t flags;
		uint32_t totalSize;
		uint32_t semanticSpecific;
		uint16_t width;
		uint16_t height;
		uint16_t depth;
		uint16_t numElements;
		GfxImageAtlasSize atlasInfo;
		uint16_t unk2a;
		byte unk2c;
		byte unk2d;
		byte unk30_count;
		byte unk2f;
		GfxImageUnk* unk30;
		void* pixels;
	};

	static_assert(sizeof(GfxImage) == 0x40);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			GfxImage* asset{ (GfxImage*)ptr };

			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueHash("packedAtlasData", (uint64_t)asset->packedAtlasData);
			json.WriteFieldValueNumber("atlasInfo", asset->atlasInfo.packed);
			json.WriteFieldValueHash("pixels", (uint64_t)asset->pixels);
			json.WriteFieldValueNumber("textureId", asset->textureId);
			json.WriteFieldValueNumber("flags", asset->flags);
			json.WriteFieldValueNumber("totalSize", asset->totalSize);
			json.WriteFieldValueNumber("semanticSpecific", asset->semanticSpecific);
			json.WriteFieldValueNumber("width", asset->width);
			json.WriteFieldValueNumber("height", asset->height);
			json.WriteFieldValueNumber("depth", asset->depth);
			json.WriteFieldValueNumber("numElements", asset->numElements);

			json.WriteFieldValueUnknown("unk2a", asset->unk2a);
			json.WriteFieldValueUnknown("unk2c", asset->unk2c);
			json.WriteFieldValueUnknown("unk2d", asset->unk2d);
			json.WriteFieldValueUnknown("unk2f", asset->unk2f);
			json.WriteFieldValueUnknown("unk30_count", asset->unk30_count);

			if (asset->unk30) {
				json.WriteFieldNameString("unk30");
				json.BeginArray();
				size_t len{ (size_t)(asset->unk30_count & 0xf) };
				for (size_t i = 0; i < len; i++) {
					GfxImageUnk* unk{ &asset->unk30[i] };
					json.BeginObject();

					json.WriteFieldValueXHash("unk30:id", unk->id);
					json.WriteFieldValueUnknown("unk30:unk8", unk->unk8);
					json.WriteFieldValueUnknown("unk30:packed", unk->packed.packed);
					json.EndObject();
				}
				json.EndArray();
			}

			json.EndObject();

			std::filesystem::path outFile{ opt.m_output / gamePath / "graphics" / "image" / std::format("{}.json", hashutils::ExtractTmp("hash", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump image {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_IMAGE, sizeof(GfxImage), true };
}