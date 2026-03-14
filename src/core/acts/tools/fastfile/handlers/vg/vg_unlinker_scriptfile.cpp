#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_vg.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <compatibility/xensik_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::vg;

	struct ScriptFile {
		XString name;
		XHash64 nameHashed;
		int32_t compressedLen;
		int32_t len;
		int32_t bytecodeLen;
		void* buffer;
		void* bytecode;
	};
	static_assert(sizeof(ScriptFile) == 0x30);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::vector<ScriptFile*> hashed{};

		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			hashed.clear();
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (hashed.empty()) {
				return;
			}


			std::filesystem::path outFile{ opt.m_output / gamePath / "scriptfile" / "names" / std::format("{}.csv", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			for (ScriptFile* sf : hashed) {
				uint64_t id{};
				try {
					id = std::strtoull(sf->name, nullptr, 10);
				}
				catch (...) {}

				if (id) {
					os << sf->name
						<< ",0x" << std::hex << std::setw(4) << std::setfill('0') << id
						<< ",#" << hashutils::ExtractTmp("script", sf->nameHashed) << "\n";
				}
			}
			hashed.clear();
		}

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ScriptFile* asset{ (ScriptFile*)ptr };

			const char* n{ ValidateName(asset->name) };
			std::string_view nv{ n };

			if (!nv.ends_with(".gsc") && !nv.ends_with(".csc") && asset->nameHashed) {
				// if the file doesn't have an extension, it probably a stripped one (ex: 1968)
				// we use the hashed name instead (scripts/mp/audio.gsc)
				hashed.push_back(asset);
				n = hashutils::ExtractPtr(asset->nameHashed);
				if (!n) {
					n = utils::va("hashed/script_%llx.gsc", asset->nameHashed);
				}
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "scriptfile" / n };
			outFile.replace_extension(".gscbin");

			LOG_OPT_INFO("Dump scriptfile {} ({})", outFile.string(), hashutils::ExtractTmp("hash", asset->nameHashed));

			if constexpr (compatibility::xensik::decompiler::available) {
				if (!opt.disableScriptsDecomp) {
					std::filesystem::path outSource{ opt.m_output / gamePath / "source" / n };
					if (!outSource.has_extension()) {
						outSource.replace_extension(".gsc");
					}

					compatibility::xensik::decompiler::ScriptFileInformation nfo{};
					nfo.vm = tool::gsc::opcode::VMI_IW_BIN_VANGUARD;
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