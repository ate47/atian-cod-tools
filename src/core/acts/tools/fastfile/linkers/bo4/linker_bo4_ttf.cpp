#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

	void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path filenamePath{ id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();
			std::string defname{ rfpath.string() };

			std::string buffer{};
			if (!utils::ReadFile(path, buffer)) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
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

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			TTFDef& ttf{ ff.data.AllocStreamRef<TTFDef>(ref) };

			ttf.name.name = ctx.HashXHash(defname);
			ttf.filename.name = ctx.HashPathName(filenamePath);
			ttf.file = (byte*)fastfile::linker::memory::POINTER_NEXT;
			ttf.next = nullptr;
			ttf.fileLen = (int32_t)buffer.size();
			ttf.kerningCache = (TTFKerningEntry*)fastfile::linker::memory::POINTER_NEXT;
			ttf.kerningCacheCount = 0x4000;

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ff.data.Align(0x10);
			ff.data.WriteStream(buffer.data(), buffer.size() + 1);
			ff.data.PopStream();

			// alloc cache
			ff.data.PushStream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			ff.data.Align<uint16_t>();
			ff.data.AllocRuntime(ttf.kerningCacheCount * sizeof(TTFKerningEntry));
			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset ttf {} (hash_{:x})", rfpath.string(), ttf.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_TTF };
}