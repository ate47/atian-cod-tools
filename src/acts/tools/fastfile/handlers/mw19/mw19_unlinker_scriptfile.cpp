#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_mw19.hpp>
#include <tools/gsc/gsc_iw.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <compatibility/xensik_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::mw19;

	struct ScriptFile {
		XString name;
		int32_t compressedLen;
		int32_t len;
		int32_t bytecodeLen;
		void* buffer;
		void* bytecode;
	};
	static_assert(sizeof(ScriptFile) == 0x28);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ScriptFile* asset{ (ScriptFile*)ptr };

			const char* assetName{ ValidateName(asset->name) };
			std::string_view assetNameView{ assetName };

			const char* n{};
			if (!assetNameView.ends_with(".gsc") && !assetNameView.ends_with(".csc")) {
				// if the file doesn't have an extension, it is probably a stripped one (ex: 1968)
				// we can see if have the token for the script
				uint32_t id{};
				try {
					id = std::strtoul(assetName, nullptr, 10);
				}
				catch (...) {}
				if (id) {
					const char* nn{ tool::gsc::iw::GetOpaqueStringForVm(tool::gsc::opcode::VMI_IW_BIN_MW19, id, false) };
					if (nn) {
						std::string_view nnv{ nn };
						// for some reasons it doesn't work for some scripts
						if (nnv.ends_with(".gsc") || nnv.ends_with(".csc") || nnv.ends_with(".gsh")) {
							n = nn;
						}
					}
				}

				if (!n) {
					n = utils::va("scripts/opaque/%s.gsc", assetName);
				}
			}
			else {
				n = assetName;
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "scriptfile" / n };
			outFile.replace_extension(".gscbin");

			LOG_INFO("Dump scriptfile {} ({})", outFile.string(), assetName);

			if constexpr (compatibility::xensik::decompiler::available) {
				if (!opt.disableScriptsDecomp) {
					std::filesystem::path outSource{ opt.m_output / gamePath / "source" / n };
					if (!outSource.has_extension()) {
						outSource.replace_extension(".gsc");
					}

					compatibility::xensik::decompiler::ScriptFileInformation nfo{};
					nfo.vm = tool::gsc::opcode::VMI_IW_BIN_MW19;
					nfo.FillScriptFile(asset);

					if (!compatibility::xensik::decompiler::DecompileScript(&nfo, outSource)) {
						LOG_ERROR("Error when decompiling script");
					}
				}
			}


			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, false, std::ios::binary };
			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			compatibility::xensik::gscbin::GscBinHeader header;
			header.Magic() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
			header.bytecodeLen = asset->bytecodeLen;
			header.compressedLen = asset->compressedLen;
			header.len = asset->len;

			utils::WriteValue(os, &header, sizeof(header));
			utils::WriteValue(os, asset->buffer, asset->compressedLen);
			utils::WriteValue(os, asset->bytecode, asset->bytecodeLen);
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW8H_ASSET_SCRIPTFILE, sizeof(ScriptFile) };
}