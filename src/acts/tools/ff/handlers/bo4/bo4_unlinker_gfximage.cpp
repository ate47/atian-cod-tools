#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class ImageWorker : public Worker {
		struct GfxImageUnk0 {
			void* data;
			uint16_t len;
			uint64_t unk10;
			uint32_t refs;
			uint32_t unk1c;
		};

		struct IDXGIAdapter;
		struct GfxStreamedPartInfo;

		typedef int32_t GfxPixelFormat;

		enum MapType : byte {
			MAPTYPE_NONE = 0x0,
			MAPTYPE_2D = 0x1,
			MAPTYPE_2D_ARRAY = 0x2,
			MAPTYPE_3D = 0x3,
			MAPTYPE_CUBE = 0x4,
			MAPTYPE_CUBE_ARRAY = 0x5,
			MAPTYPE_COUNT = 0x6,
		};

		struct GfxImage {
			GfxImageUnk0* unk0;
			IDXGIAdapter* dxgiAdapter;
			uintptr_t unk10;
			byte* pixels;
			XHash name;
			GfxStreamedPartInfo* streamedParts;
			uint64_t unk38;
			uint64_t unk40;
			uint64_t unk48;
			uint64_t unk50;
			uint32_t imageFlags;
			uint32_t unk5c;
			uint32_t totalSize;
			GfxPixelFormat imageFormat;
			uint16_t width;
			uint16_t height;
			uint16_t depth;
			uint16_t unk6e;
			byte unk70;
			byte unk71;
			uint16_t unk72;
			uint16_t unk74;
			byte unk76;
			byte alignment;
			byte unk78;
			MapType mapType;
			byte levelCount;
			byte await_stream;
			byte streaming;
			byte streamedParts_count;
			byte unk7e;
			byte unk7f;
			uint64_t unk80;
		};
		static_assert(sizeof(GfxImage) == 0x88);


		std::vector<GfxImage*> keys{};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			if (!opt.testDump) return;
			keys.push_back((GfxImage*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			keys.clear();
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (keys.empty()) return; // nothing to dump


			utils::raw_file_extractor::JsonWriter json{};

			json.BeginObject();

			for (GfxImage* asset : keys) {
				json.WriterFieldNameHash(asset->name);
				json.BeginObject();
				json.WriteFieldNameString("name"); json.WriteValueHash(asset->name);
				json.WriteFieldNameString("unk0"); json.WriteValueString(std::format("{}", (void*)asset->unk0));
				json.WriteFieldNameString("dxgiAdapter"); json.WriteValueString(std::format("{}", (void*)asset->dxgiAdapter));
				json.WriteFieldNameString("unk10"); json.WriteValueString(std::format("{}", (void*)asset->unk10));
				json.WriteFieldNameString("pixels"); json.WriteValueString(std::format("{}", (void*)asset->pixels));
				json.WriteFieldNameString("streamedParts"); json.WriteValueString(std::format("{}", (void*)asset->streamedParts));
				json.WriteFieldNameString("unk38"); json.WriteValueString(std::format("{}", asset->unk38));
				json.WriteFieldNameString("unk40"); json.WriteValueString(std::format("{}", asset->unk40));
				json.WriteFieldNameString("unk48"); json.WriteValueString(std::format("{}", asset->unk48));
				json.WriteFieldNameString("unk50"); json.WriteValueString(std::format("{}", asset->unk50));
				json.WriteFieldNameString("imageFlags"); json.WriteValueString(std::format("{}", asset->imageFlags));
				json.WriteFieldNameString("unk5c"); json.WriteValueString(std::format("{}", asset->unk5c));
				json.WriteFieldNameString("totalSize"); json.WriteValueString(std::format("{}", asset->totalSize));
				json.WriteFieldNameString("imageFormat"); json.WriteValueString(std::format("{}", (int)asset->imageFormat));
				json.WriteFieldNameString("width"); json.WriteValueString(std::format("{}", asset->width));
				json.WriteFieldNameString("height"); json.WriteValueString(std::format("{}", asset->height));
				json.WriteFieldNameString("depth"); json.WriteValueString(std::format("{}", asset->depth));
				json.WriteFieldNameString("unk6e"); json.WriteValueString(std::format("{}", asset->unk6e));
				json.WriteFieldNameString("unk70"); json.WriteValueString(std::format("{}", asset->unk70));
				json.WriteFieldNameString("unk71"); json.WriteValueString(std::format("{}", asset->unk71));
				json.WriteFieldNameString("unk72"); json.WriteValueString(std::format("{}", asset->unk72));
				json.WriteFieldNameString("unk74"); json.WriteValueString(std::format("{}", asset->unk74));
				json.WriteFieldNameString("unk76"); json.WriteValueString(std::format("{}", asset->unk76));
				json.WriteFieldNameString("alignment"); json.WriteValueString(std::format("{}", asset->alignment));
				json.WriteFieldNameString("unk78"); json.WriteValueString(std::format("{}", asset->unk78));
				json.WriteFieldNameString("mapType"); json.WriteValueString(std::format("{}", (int)asset->mapType));
				json.WriteFieldNameString("levelCount"); json.WriteValueString(std::format("{}", asset->levelCount));
				json.WriteFieldNameString("await_stream"); json.WriteValueString(std::format("{}", asset->await_stream));
				json.WriteFieldNameString("streaming"); json.WriteValueString(std::format("{}", asset->streaming));
				json.WriteFieldNameString("streamedParts_count"); json.WriteValueString(std::format("{}", asset->streamedParts_count));
				json.WriteFieldNameString("unk7e"); json.WriteValueString(std::format("{}", asset->unk7e));
				json.WriteFieldNameString("unk7f"); json.WriteValueString(std::format("{}", asset->unk7f));
				json.WriteFieldNameString("unk80"); json.WriteValueString(std::format("{}", asset->unk80));
				json.EndObject();
			}

			json.EndObject();

			std::filesystem::path outFile{ opt.m_output / "bo4" / "data" / "gfx" / "image" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump gfximage {} ({})", outFile.string(), keys.size());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

		}
	};

	utils::MapAdder<ImageWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE };
}