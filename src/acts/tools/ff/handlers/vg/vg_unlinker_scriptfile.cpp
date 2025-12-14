#include <includes.hpp>
#include <tools/ff/handlers/handler_game_vg.hpp>
#include <compatibility/xensik_gscbin.hpp>

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

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ScriptFile* asset{ (ScriptFile*)ptr };

			const char* n{ ValidateName(asset->name) };
			std::string_view nv{ n };

			if (!nv.ends_with(".gsc") && asset->nameHashed) {
				// if the file doesn't have an extension, it probably a stripped one (ex: 1968)
				// we use the hashed name instead (scripts/mp/audio.gsc)
				n = hashutils::ExtractPtr(asset->nameHashed);
				if (!n) {
					n = utils::va("hashed/script_%llx.gsc", asset->nameHashed);
				}
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "scriptfile" / n };
			outFile.replace_extension(".gscbin");

			LOG_INFO("Dump scriptfile {} ({})", outFile.string(), hashutils::ExtractTmp("hash", asset->nameHashed));

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