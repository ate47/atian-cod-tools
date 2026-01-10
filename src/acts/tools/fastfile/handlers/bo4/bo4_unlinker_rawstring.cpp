#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class RawStringWorker : public Worker {
		struct RawString {
			XHash name;
			const char* str;
		}; static_assert(sizeof(RawString) == 0x18);
		std::vector<RawString*> assets{};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			assets.push_back((RawString*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			assets.clear();
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (assets.empty()) return; // nothing to dump

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "rawstring" / std::format("{}.csv", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());

			}

			LOG_OPT_INFO("Dump rawstring({}) {}", assets.size(), outFile.string());

			os << "name,string";

			for (RawString* rs : assets) {
				if (rs->str) {
					hashutils::Add(rs->str);
				}
				else {
					rs->str = "";
				}
				os << "\n#" << hashutils::ExtractTmp("hash", rs->name.name) << "," << rs->str;
			}

		}
	};

	utils::MapAdder<RawStringWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_RAWSTRING };
}