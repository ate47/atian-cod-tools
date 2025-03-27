#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/compress_utils.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			struct LuaFile {
				uint64_t name;
				uint64_t unk10_count;
				uint64_t* unk10;
				uint64_t unk18;
				uint64_t unk20;
				LuaFile** unk28;
				uint16_t unk28_count;
				uint16_t unk32;
				int32_t len;
				byte* buffer;
				uint64_t unk40;
				uint64_t unk48;
			}; static_assert(sizeof(LuaFile) == 0x50);

			if (asset.HeaderSize != sizeof(LuaFile)) {
				LOG_ERROR("INVALID LUAFILE HEADER SIZE: 0x{:x}", asset.HeaderSize);
				return false;
			}

			LuaFile header;

			if (!proc.ReadMemory(&header, asset.Header, sizeof(header))) {
				LOG_ERROR("Can't read header for {}", hashutils::ExtractTmp("hash", asset.ID));
				return false;
			}
			const char* n{ hashutils::ExtractPtr(header.name) };

			if (!n) {
				n = utils::va("hashed/luafile/file_%llx.lua", header.name);
			}

			std::filesystem::path loc{ ctx.outDir / "luafile" / n };
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			auto buff{ proc.ReadMemoryArrayEx<byte>(reinterpret_cast<uintptr_t>(header.buffer), header.len) };

			if (opt.header) {
				LOG_INFO("header buff {}", (void*)header.buffer);
				if (header.unk10_count) {
					auto arr{ proc.ReadMemoryArrayEx<uint64_t>(reinterpret_cast<uintptr_t>(header.unk10), header.unk10_count) };
					for (size_t i = 0; i < header.unk10_count; i++) {
						LOG_INFO("unk10[{}] = {}", i, hashutils::ExtractTmp("hash", arr[i]));
					}
				}
				if (header.unk28_count) {
					auto arr{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(header.unk28), header.unk28_count) };
					LuaFile dep;
					for (size_t i = 0; i < header.unk28_count; i++) {
						if (!proc.ReadMemory(&dep, arr[i], sizeof(dep))) {
							LOG_ERROR("Can't read sub header {} for {}", i, hashutils::ExtractTmp("hash", asset.ID));
							break;
						}
						LOG_INFO("unk28[{}] = {}", i, hashutils::ExtractTmp("hash", dep.name));
					}
				}

				LOG_INFO("len   {:x}", header.len);
				LOG_INFO("unk18 {:x}", header.unk18);
				LOG_INFO("unk20 {:x}", header.unk20);
				LOG_INFO("unk40 {:x}", header.unk40);
				LOG_INFO("unk48 {:x}", header.unk48);
			}

			std::vector<byte> decomp;
			byte* buffAlloc{ buff.get() };
			int32_t buffSize{ header.len };
			if (opt.decompressLua) {
				decomp = utils::compress::Decompress(utils::compress::COMP_ZLIB, buffAlloc, buffSize, 2);
				buffAlloc = decomp.data();
				buffSize = (int32_t)decomp.size();
			}

			if (!utils::WriteFile(loc, buffAlloc, buffSize)) {
				LOG_ERROR("Can't write file");
				return false;
			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_LUAFILE };
	utils::MapAdder<AssetMapping, bo6::T10RAssetType> sp{ GetSpMap(), bo6::T10R_ASSET_LUAFILE, bo6::T10_ASSET_LUAFILE };
}