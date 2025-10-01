#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6_sp.hpp>

namespace {
	using namespace fastfile::handlers::bo6sp;
	union NetConstString {
		const char* str;
		XHash64 hash;
	};

	struct NetConstStrings {
		uint64_t name;
		uint32_t unk8;
		byte unk0c;
		bool useHash;
		uint32_t unk10;
		uint32_t stringsCount;
		uint32_t unk18;
		NetConstString* strings;
	}; static_assert(sizeof(NetConstStrings) == 0x28);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			NetConstStrings* asset{ (NetConstStrings*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueBool("useHash", asset->useHash);
			// todo
			//if (asset->unk0c) json.WriteFieldValueNumber("unk0c", (int)asset->unk0c);
			//if (asset->unk8) json.WriteFieldValueNumber("unk8", asset->unk8);
			//if (asset->unk10) json.WriteFieldValueNumber("unk10", asset->unk10);
			//if (asset->unk18) json.WriteFieldValueNumber("unk18", asset->unk18);

			json.WriteFieldNameString("strings");
			json.BeginArray();
			if (asset->useHash) {
				for (size_t i = 0; i < asset->stringsCount; i++) {
					json.WriteValueHash(asset->strings[i].hash);
				}
			}
			else {
				for (size_t i = 0; i < asset->stringsCount; i++) {
					json.WriteValueString(asset->strings[i].str);
				}
			}
			json.EndArray();
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6sp" / "source" / "tables" / "netconststrings"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump netconststrings {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_NETCONSTSTRINGS, sizeof(NetConstStrings) };
}