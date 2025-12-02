#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo7.hpp>

namespace {
	using namespace fastfile::handlers::bo7;

	struct OmnVar {
		XHash64 name;
		XHash64 val;
	}; static_assert(sizeof(OmnVar) == 0x10);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::vector<OmnVar*> vals{};

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			vals.push_back((OmnVar*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			vals.clear(); // cleanup for errors?
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (vals.empty()) return; // nothing to dump
			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "omnvars"
				/ std::format("{}.csv", ctx.ffname) 
			};
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			os << "name,var";
			for (OmnVar* var : vals) {
				os << "\n#" << hashutils::ExtractTmp("hash", var->name) << ",#" << hashutils::ExtractTmp("hash", var->val);
			}

			LOG_INFO("Dump omnvars {} into {}", vals.size(), outFile.string());

			vals.clear(); // cleanup
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_OMNVAR, sizeof(OmnVar) };
}