#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class TTFWorker : public LinkerWorker {
	public:
		TTFWorker() : LinkerWorker("TTF") {}

		void Compute(BO4LinkContext& ctx) override {
			for (const char*& filename : ctx.linkCtx.zone.assets["ttf"]) {
				std::filesystem::path path{ ctx.linkCtx.input / filename };
				std::filesystem::path filenamePath{ filename };
				std::filesystem::path rfpath{ path.filename() };
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
				};

				struct TTFDef {
					XHash name;
					XHash filename;
					int32_t fileLen;
					byte* file;
					void* ftFace;
					TTFDef* next;
					TTFKerningEntry* kerningCache;
					uint32_t kerningCacheCount;
				};; static_assert(sizeof(TTFDef) == 0x50);

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_TTF, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				TTFDef ttf{};

				ttf.name.name = ctx.HashXHash(defname);
				ttf.filename.name = ctx.HashPathName(filenamePath);
				ttf.file = (byte*)fastfile::linker::data::POINTER_NEXT;
				ttf.fileLen = (int32_t)buffer.size();
				ttf.kerningCache = (TTFKerningEntry*)fastfile::linker::data::POINTER_NEXT;
				ttf.kerningCacheCount = 0x4000;
				ctx.data.WriteData(ttf);

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.data.Align(0x10);
				ctx.data.WriteData(buffer.data(), buffer.size() + 1);
				ctx.data.PopStream();

				// alloc cache
				ctx.data.PushStream(XFILE_BLOCK_RUNTIME_VIRTUAL);
				ctx.data.Align(2);
				ctx.data.AllocData(ttf.kerningCacheCount * sizeof(TTFKerningEntry));
				ctx.data.PopStream();

				ctx.data.PopStream();

				LOG_INFO("Added asset ttf {} (hash_{:x})", rfpath.string(), ttf.name.name);
			}
		}
	};

	utils::ArrayAdder<TTFWorker, LinkerWorker> impl{ GetWorkers() };
}