#include <includes.hpp>
#include <compatibility/xensik_decompiler.hpp>

#ifdef GPL_BUILD
#include <utils/compress_utils.hpp>

#include <xsk/stdinc.hpp>
#include <xsk/gsc/context.hpp>

#include <xsk/gsc/engine/h1.hpp>
#include <xsk/gsc/engine/h2.hpp>
#include <xsk/gsc/engine/iw6_pc.hpp>
#include <xsk/gsc/engine/iw6_ps.hpp>
#include <xsk/gsc/engine/iw6_xb.hpp>
#include <xsk/gsc/engine/iw7.hpp>
#include <xsk/gsc/engine/iw8.hpp>
#include <xsk/gsc/engine/iw9.hpp>
#include <xsk/gsc/engine/s1_pc.hpp>
#include <xsk/gsc/engine/s1_ps.hpp>
#include <xsk/gsc/engine/s1_xb.hpp>
#include <xsk/gsc/engine/s2.hpp>
#include <xsk/gsc/engine/s4.hpp>


namespace compatibility::xensik::decompiler {
	using namespace tool::gsc::opcode;

	namespace {
		std::shared_ptr<xsk::gsc::context> GetXensikContext(ScriptFileInformation* info) {
			constexpr xsk::gsc::instance inst = xsk::gsc::instance::server;
			switch (info->vm) {
			case VMI_IW_BIN_AW:
				switch (info->plt) {
				case PLATFORM_PC: return std::make_shared<xsk::gsc::s1_pc::context>(inst);
				case PLATFORM_PLAYSTATION: return std::make_shared<xsk::gsc::s1_ps::context>(inst);
				case PLATFORM_XBOX: return std::make_shared<xsk::gsc::s1_xb::context>(inst);
				}
				break;
			case VMI_IW_BIN_WW2: return std::make_shared<xsk::gsc::s2::context>(inst);
			case VMI_IW_BIN_VANGUARD: return std::make_shared<xsk::gsc::s4::context>(inst);
			case VMI_IW_BIN_RM_MW1: return std::make_shared<xsk::gsc::h1::context>(inst);
			case VMI_IW_BIN_RM_MW2: return std::make_shared<xsk::gsc::h2::context>(inst);
			case VMI_IW_BIN_GHOSTS:
				switch (info->plt) {
				case PLATFORM_PC: return std::make_shared<xsk::gsc::iw6_pc::context>(inst);
				case PLATFORM_PLAYSTATION: return std::make_shared<xsk::gsc::iw6_ps::context>(inst);
				case PLATFORM_XBOX: return std::make_shared<xsk::gsc::iw6_xb::context>(inst);
				}
				break;
			case VMI_IW_BIN_IW: return std::make_shared<xsk::gsc::iw7::context>(inst);
			case VMI_IW_BIN_MW19: return std::make_shared<xsk::gsc::iw8::context>(inst);
			case VMI_IW_BIN_MW22: return std::make_shared<xsk::gsc::iw9::context>(inst);
			}
			throw std::runtime_error(std::format("Can't find xensik engine for {}/{}", tool::gsc::opcode::VMIdFancyName(info->vm), PlatformName(info->plt)));
		}
	}
	bool DecompileScript(ScriptFileInformation* info, std::filesystem::path fsPath) {
		try {
			std::shared_ptr<xsk::gsc::context> ctx{ GetXensikContext(info) };

			ctx->init(xsk::gsc::build::prod, {});
			ctx->unhash_func([](xsk::u64 hash) -> const char* { return hashutils::ExtractPtr(hash); });

			std::unique_ptr<byte[]> bufferDecomp;
			void* buffer{};

			if (info->len) {
				// extract buffer if required
				bufferDecomp = std::make_unique<byte[]>(info->len);
				buffer = bufferDecomp.get();

				int r{ utils::compress::Decompress2(utils::compress::COMP_ZLIB, buffer, info->len, info->buffer, info->compressedLen) };

				if (r <= 0) {
					throw std::runtime_error(std::format("Can't decompress script buffer {}", utils::compress::DecompressResultName(r)));
					return false;
				}
			}

			xsk::gsc::assembly::ptr assembly{ ctx->disassembler().disassemble((xsk::u8*)info->bytecode, info->bytecodeLen, (xsk::u8*)buffer, info->len) };
			xsk::gsc::program::ptr program{ ctx->decompiler().decompile(*assembly) };

			std::filesystem::create_directories(fsPath.parent_path());
			if (!utils::WriteFile(fsPath, ctx->source().dump(*program))) {
				throw std::runtime_error(std::format("Can't dump to file {}", fsPath.string()));
			}
			LOG_INFO("Decompiled {}", fsPath.string());

			return true;
		}
		catch (std::runtime_error& err) {
			LOG_ERROR("Can't decompile file {}", err.what());
			return false;
		}
	}
}
#else
namespace compatibility::xensik::decompiler {
	bool DecompileScript(ScriptFileInformation* info, std::filesystem::path fsPath) {
		LOG_ERROR("xensik decompiler not added to this build");
		return false;
	}
}
#endif