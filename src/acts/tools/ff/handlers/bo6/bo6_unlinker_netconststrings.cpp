#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
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
		using ScriptBundle = scriptbundle::ScriptBundle;

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
				opt.m_output / "bo6" / "source" / "tables" / "netconststrings"
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

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_NETCONSTSTRINGS, sizeof(NetConstStrings) };
}