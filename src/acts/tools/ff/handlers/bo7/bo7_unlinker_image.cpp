#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo7.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo7;

	struct GfxImageUnk {
		uint64_t unk0;
		uint64_t unk8;
		uint64_t unk10;

	};
	struct GfxImage {
		XHash64 name;
		void* data;
		uint64_t unk10;
		uint32_t unk38_count;
		uint32_t unk1c;
		uint64_t unk20;
		uint16_t dataLen;
		uint16_t unk2a;
		byte unk2c;
		byte unk2d;
		byte unk2e;
		byte unk2f;
		GfxImageUnk* unk30;
		void* unk38;
	};
	static_assert(sizeof(GfxImage) == 0x40);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			GfxImage* asset{ (GfxImage*)ptr };

			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueHash("data", (uint64_t)asset->data);
			json.WriteFieldValueNumber("asset->dataLen", asset->dataLen);
			json.WriteFieldValueHash("unk38", (uint64_t)asset->unk38);
			json.WriteFieldValueNumber("asset->unk38_count", asset->unk38_count);

			json.WriteFieldValueUnknown("unk10", asset->unk10);
			json.WriteFieldValueUnknown("unk1c", asset->unk1c);
			json.WriteFieldValueUnknown("unk20", asset->unk20);
			json.WriteFieldValueUnknown("unk2a", asset->unk2a);
			json.WriteFieldValueUnknown("unk2c", asset->unk2c);
			json.WriteFieldValueUnknown("unk2d", asset->unk2d);
			json.WriteFieldValueUnknown("unk2e", asset->unk2e);
			json.WriteFieldValueUnknown("unk2f", asset->unk2f);

			if (asset->unk30) {
				json.WriteFieldNameString("unk30");
				json.BeginArray();
				size_t len{ (size_t)(asset->unk2e & 0xf) };
				for (size_t i = 0; i < len; i++) {
					GfxImageUnk* unk{ &asset->unk30[i] };
					json.BeginObject();

					json.WriteFieldValueUnknown("unk30:unk0", unk->unk0);
					json.WriteFieldValueUnknown("unk30:unk8", unk->unk8);
					json.WriteFieldValueUnknown("unk30:unk10", unk->unk10);
					json.EndObject();
				}
				json.EndArray();
			}

			json.EndObject();

			std::filesystem::path outFile{ opt.m_output / gamePath / "graphics" / "image" / std::format("{}.json", hashutils::ExtractTmp("hash", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump image {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_IMAGE, sizeof(GfxImage), true };
}