#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

namespace fastfile::linker::bo4 {
	enum GfxPixelFormat : int32_t {
		GFX_PF_INVALID = 0x0,
		GFX_PF_R8G8B8A8_T1 = 28,
		GFX_PF_R8G8B8A8_T2 = 29,
		GFX_PF_R8G8B8A8_T3 = 31,
		GFX_PF_R8G8B8A8_T4 = 87,
	};
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
		void* unk0;
		void* dxgiAdapter;
		uintptr_t unk10;
		byte* pixels;
		XHash name;
		void* streamedParts;
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


	class GfxImageWorker : public LinkerWorker {
	public:
		GfxImageWorker() : LinkerWorker("GfxImage", -1) {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["image"]) {
				assval.handled = true;
				std::filesystem::path path{ ctx.linkCtx.input / "images" / assval.value };
				std::filesystem::path filename{ path.filename() };
				filename.replace_extension();
				std::string name{ filename.string() };
				uint64_t hash{ ctx.HashXHash(name.data()) };
				std::string pathStr{ path.string() };
				GfxImage gfx{};
				gfx.name.name = hash;

				LOG_TRACE("Processing image {} ({}) 0x{:x}", name, path.string(), hash);
				{
					int x, y, channels;
					stbi_uc* img{ stbi_load(pathStr.data(), &x, &y, &channels, 0) };
					
					if (!img) {
						ctx.error = true;
						LOG_ERROR("Can't process image {}: {}", path.string(), stbi_failure_reason());
						continue;
					}

					utils::CloseEnd imgce{ [img]() { stbi_image_free(img); } };


					gfx.unk0 = 0;
					gfx.totalSize = x * y * channels;
					gfx.width = x;
					gfx.height = y;
					gfx.mapType = MAPTYPE_2D;
					gfx.levelCount = 1;
					gfx.depth = 1;
					gfx.pixels = (byte*)fastfile::linker::data::POINTER_NEXT;

					LOG_TRACE("Image: {}x{}x{}", x, y, channels);
					switch (channels) {
					case 4:
						gfx.imageFormat = GFX_PF_R8G8B8A8_T1;
						break;
					default:
						LOG_ERROR("Can't compile {} channels image", channels);
						return;
					}

					ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_IMAGE, fastfile::linker::data::POINTER_NEXT);

					ctx.data.PushStream(XFILE_BLOCK_TEMP);
					// header
					ctx.data.WriteData(gfx);

					ctx.data.PushStream(XFILE_BLOCK_PHYSICAL);
					// pixels
					ctx.data.Align(0xFF);
					ctx.data.WriteData(img, gfx.totalSize);

					ctx.data.PopStream();

					ctx.data.PopStream();

					LOG_INFO("Added asset gfximage {} (hash_{:x})", name, gfx.name.name);
				}

			}
		}
	};

	utils::ArrayAdder<GfxImageWorker, LinkerWorker> impl{ GetWorkers() };
}