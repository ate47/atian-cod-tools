#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <utils/decrypt.hpp>


namespace {
	using namespace fastfile::handlers::bo4;

	struct Localize {
		const char* val;
		XHash name;
	}; static_assert(sizeof(Localize) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::vector<Localize*> vals{};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			vals.push_back((Localize*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			vals.clear(); // cleanup for errors?
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (vals.empty()) return; // nothing to dump
			std::filesystem::path outFile{ opt.m_output / "bo4" / "localize" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile, true };

			os << "{";
			size_t invalid{};
			size_t i{};
			for (Localize* loc : vals) {
				if (!loc->val) {
					continue;
				}
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
						os << "\"" << utils::FormattedStringJson{ decrypt::DecryptString((char*)loc->val)} << "\"";
					}
				}

			}
			os << "\n}";
			LOG_INFO("Dump localizes {} into {}", vals.size(), outFile.string());
			if (invalid) LOG_ERROR("Found {} invalid entry(ies)", invalid);

			vals.clear(); // cleanup
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY };
}