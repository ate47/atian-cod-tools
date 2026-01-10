#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

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
			uint32_t hash;
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


			std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / "image" / std::format("{}.csv", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump gfximage {} ({})", outFile.string(), keys.size());

			utils::OutFileCE os{ outFile };
			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			os << "name,unk0,dxgiAdapter,unk10,pixels,streamedParts,unk38,unk40,unk48,unk50,imageFlags,hash,totalSize,imageFormat,width,height,depth,"
				"unk6e, unk70, unk71, unk72, unk74, unk76, alignment, unk78, mapType, levelCount, await_stream, streaming, streamedParts_count, unk7e, unk7f, unk80\n";
			for (GfxImage* asset : keys) {
				os << hashutils::ExtractTmp("hash", asset->name) 
					<< std::hex
					<< "," << (((void*)asset->unk0)) 
					<< "," << (((void*)asset->dxgiAdapter)) 
					<< "," << (((void*)asset->unk10)) 
					<< "," << (((void*)asset->pixels)) 
					<< "," << (((void*)asset->streamedParts)) 
					<< "," << ((asset->unk38)) 
					<< "," << ((asset->unk40)) 
					<< "," << ((asset->unk48))
					<< "," << ((asset->unk50)) 
					<< "," << ((asset->imageFlags)) 
					<< "," << ((asset->hash))
					<< "," << ((asset->totalSize))
					<< "," << (((int)asset->imageFormat)) 
					<< "," << ((asset->width))
					<< "," << ((asset->height)) 
					<< "," << ((asset->depth)) 
					<< "," << ((asset->unk6e)) 
					<< "," << (((int)asset->unk70)) 
					<< "," << (((int)asset->unk71)) 
					<< "," << ((asset->unk72))
					<< "," << ((asset->unk74)) 
					<< "," << (((int)asset->unk76))
					<< "," << (((int)asset->alignment))
					<< "," << (((int)asset->unk78))
					<< "," << (((int)asset->mapType)) 
					<< "," << (((int)asset->levelCount))
					<< "," << (((int)asset->await_stream))
					<< "," << (((int)asset->streaming))
					<< "," << (((int)asset->streamedParts_count))
					<< "," << (((int)asset->unk7e))
					<< "," << (((int)asset->unk7f)) 
					<< "," << ((asset->unk80))
					<< "\n";
			}


		}
	};

	utils::MapAdder<ImageWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE };
}