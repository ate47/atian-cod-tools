#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class TTFWorker : public LinkerWorker {
	public:
		TTFWorker() : LinkerWorker("TTF") {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["ttf"]) {
				std::filesystem::path path{ ctx.linkCtx.input / assval.value };
				std::filesystem::path filenamePath{ assval.value };
				std::filesystem::path rfpath{ path.filename() };
				assval.handled = true;
				rfpath.replace_extension();
				std::string defname{ rfpath.string() };

				std::string buffer{};
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				struct TTFKerningEntry {
					uint16_t height;
					uint16_t prevCharIndex;
					uint16_t nextCharIndex;
					int16_t kerning;
				}; static_assert(sizeof(TTFKerningEntry) == 8);

				struct TTFDef {
					XHash name;
					XHash filename;
					int32_t fileLen;
					byte* file;
					void* ftFace;
					TTFDef* next;
					TTFKerningEntry* kerningCache;
					uint32_t kerningCacheCount;
				}; static_assert(sizeof(TTFDef) == 0x50);

				ctx.mainFF.data.AddAsset(games::bo4::pool::ASSET_TYPE_TTF, fastfile::linker::data::POINTER_NEXT);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_TEMP);
				TTFDef ttf{};

				ttf.name.name = ctx.HashXHash(defname);
				ttf.filename.name = ctx.HashPathName(filenamePath);
				ttf.file = (byte*)fastfile::linker::data::POINTER_NEXT;
				ttf.next = nullptr;
				ttf.fileLen = (int32_t)buffer.size();
				ttf.kerningCache = (TTFKerningEntry*)fastfile::linker::data::POINTER_NEXT;
				ttf.kerningCacheCount = 0x4000;
				ctx.mainFF.data.WriteData(ttf);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.mainFF.data.Align(0x10);
				ctx.mainFF.data.WriteData(buffer.data(), buffer.size() + 1);
				ctx.mainFF.data.PopStream();

				// alloc cache
				ctx.mainFF.data.PushStream(XFILE_BLOCK_RUNTIME_VIRTUAL);
				ctx.mainFF.data.Align<uint16_t>();
				ctx.mainFF.data.AllocRuntimeData(ttf.kerningCacheCount * sizeof(TTFKerningEntry));
				ctx.mainFF.data.PopStream();

				ctx.mainFF.data.PopStream();

				LOG_INFO("Added asset ttf {} (hash_{:x})", rfpath.string(), ttf.name.name);
			}
		}
	};

	utils::ArrayAdder<TTFWorker, LinkerWorker> impl{ GetWorkers() };
}