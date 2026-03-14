#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_mw19.hpp>

namespace {
	using namespace fastfile::handlers::mw19;

	struct Localize {
		XString name;
		XString val;
	}; static_assert(sizeof(Localize) == 0x10);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::vector<Localize> vals{};

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			vals.emplace_back(*(Localize*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			vals.clear(); // cleanup for errors?
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (vals.empty()) return; // nothing to dump
			std::filesystem::path outFile{ opt.m_output / gamePath / "localize" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			os << "{";
			size_t invalid{};
			for (size_t i = 0; i < vals.size(); i++) {
				Localize* loc{ &vals[i] };
				if (i) os << ",";
				os << "\n";
				utils::Padding(os, 1) << "\"" << ValidateName(loc->name) << "\": ";
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
						os << "\"" << utils::FormattedStringJson{ loc->val } << "\"";
					}
				}

			}
			os << "\n}";
			LOG_OPT_INFO("Dump localizes {} into {}", vals.size(), outFile.string());
			if (invalid) LOG_ERROR("Found {} invalid entry(ies)", invalid);

			vals.clear(); // cleanup
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW8H_ASSET_LOCALIZE, sizeof(Localize) };
}