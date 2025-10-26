#include <includes.hpp>
#include <tools/ff/handlers/handler_game_mwii.hpp>
#include <compatibility/xensik_gscbin.hpp>

namespace {
	using namespace fastfile::handlers::mwii;

	struct ScriptFile {
		XHash64 name;
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

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/scriptfile/file_%llx.gscbin", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "scriptfile" / n };
			outFile.replace_extension(".gscbin");

			LOG_INFO("Dump scriptfile {}", outFile.string());

			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, false, std::ios::binary };
			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
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

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW9H_ASSET_SCRIPTFILE, sizeof(ScriptFile) };
}