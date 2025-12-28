#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>
#include <decryptutils.hpp>


namespace {
	using namespace fastfile::handlers::cw;

	struct Localize {
		const char* val;
		CWXHash name;
	}; static_assert(sizeof(Localize) == 0x10);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::vector<Localize*> vals{};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			Localize* loc{ (Localize*)ptr };
			if (loc->val) {
				vals.push_back(loc);
			}
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			vals.clear(); // cleanup for errors?
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (vals.empty()) return; // nothing to dump
			std::filesystem::path outFile{ opt.m_output / "cw" / "localize" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			os << "{";
			size_t invalid{};
			size_t i{};
			for (Localize* loc : vals) {
				if (i++) os << ",";
				os << "\n";
				utils::Padding(os, 1) << "\"" << hashutils::ExtractTmp("hash", loc->name) << "\": ";
				if (!loc->val) {
					os << "null";
					invalid++;
				}
				else {
					byte b;
					if (!hook::memory::ReadMemorySafe((void*)loc->val, &b, sizeof(b))) {
						const char* hh{ hashutils::ExtractPtr((uint64_t)loc->val) };
						if (hh) {
							os << "\"ref:" << hh << "\"";
						}
						else {
							os << "\"invalid:" << (void*)loc->val << "\"";
							invalid++;
						}
					}
					else {
						os << "\"" << utils::FormattedStringJson{ (char*)loc->val } << "\"";
					}
				}

			}
			os << "\n}";
			LOG_INFO("Dump localizes {} into {}", vals.size(), outFile.string());
			if (invalid) LOG_ERROR("Found {} invalid entry(ies)", invalid);

			vals.clear(); // cleanup
		}
	};

	utils::MapAdder<ImplWorker, cw::XAssetType, Worker> impl{ GetWorkers(), cw::XAssetType::ASSET_TYPE_LOCALIZEENTRY };
}