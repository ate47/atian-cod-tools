#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct GscDbgEntry {
		uint64_t name;
		uint32_t len;
		void* buffer;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			GscDbgEntry entry{};

			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}

			std::filesystem::path out = ctx.outDir / "gsc" / utils::va("script_%llx.gscdbg", entry.name);
			if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
			std::filesystem::create_directories(out.parent_path());
			LOG_INFO("Dump {} -> {} (0x{:x})", hashutils::ExtractTmpScript(entry.name), out.string(), entry.len);
			auto buff = std::make_unique<byte[]>(entry.len);
			if (!proc.ReadMemory(&buff[0], reinterpret_cast<uintptr_t>(entry.buffer), entry.len)) {
				LOG_ERROR("Can't read buff {}", entry.name);
				return false;
			}
			if (!utils::WriteFile(out, &buff[0], entry.len)) {
				LOG_ERROR("Can't write file {}", out.string());
				return false;
			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_GSCGDB };
	utils::MapAdder<AssetMapping, bo6::T10RAssetType> sp{ GetSpMap(), bo6::T10R_ASSET_GSCGDB, bo6::T10_ASSET_GSCGDB };
}