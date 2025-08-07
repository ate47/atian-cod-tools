#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		using Worker::Worker;

		struct Localize {
			uint64_t name;
			const char* val;
		}; static_assert(sizeof(Localize) == 0x10);

		std::vector<Localize*> vals{};

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			vals.push_back((Localize*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			vals.clear(); // cleanup for errors?
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (vals.empty()) return; // nothing to dump
			std::filesystem::path outFile{ opt.m_output / "bo6" / "localize" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			os << "{";
			size_t invalid{};
			for (size_t i = 0; i < vals.size(); i++) {
				Localize* loc{ vals[i] };
				if (i) os << ",";
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
						os << "\"" << utils::FormattedString{ loc->val } << "\"";
					}
				}

			}
			os << "\n}";
			LOG_INFO("Dump localizes {} into {}", vals.size(), outFile.string());
			if (invalid) LOG_ERROR("Found {} invalid entry(ies)", invalid);

			vals.clear(); // cleanup
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_LOCALIZE };
}