#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct RawFile {
		uint64_t name;
		uint32_t type;
		uint32_t compressedLen;
		uint32_t uncompressedLen;
		byte* data;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			RawFile data;

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} header ", hashutils::ExtractTmp("hash", asset.ID));
				return false;
			}

			const char* dfilename = hashutils::ExtractPtr(data.name);

			if (!dfilename) dfilename = utils::va("hashed/rawfile/file_%llx.txt", data.name);

			std::filesystem::path loc{ ctx.outDir / "dump" / dfilename };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			uint32_t readLen{ data.compressedLen ? data.compressedLen : (data.uncompressedLen + 1) };


			if (readLen) {
				auto buff{ proc.ReadMemoryArrayEx<byte>(reinterpret_cast<uintptr_t>(data.data), readLen) };


				byte* outBuff{ buff.get() };
				size_t outBuffSize{ readLen };

				std::unique_ptr<byte[]> ubuff; 
				if (data.compressedLen) {
					ubuff = std::make_unique<byte[]>(data.uncompressedLen + 1);

					int ret{ utils::compress::Decompress2(utils::compress::COMP_ZLIB, ubuff.get(), data.uncompressedLen + 1, buff.get(), readLen) };
					if (ret < 0) {
						LOG_ERROR("Can't decompress {}", loc.string());
					}
					else {
						outBuff = ubuff.get();
						outBuffSize = data.uncompressedLen;
					}
				}

				if (!utils::WriteFile(loc, outBuff, outBuffSize)) {
					LOG_ERROR("Can't write {}", loc.string());
					return false;
				}
			}
			else {
				if (!utils::WriteFile(loc, "")) {
					LOG_ERROR("Can't write {}", loc.string());
					return false;
				}
			}


			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_RAWFILE };
	utils::MapAdder<AssetMapping, bo6::T10RAssetType> sp{ GetSpMap(), bo6::T10R_ASSET_RAWFILE, bo6::T10_ASSET_RAWFILE };
}