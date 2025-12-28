#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class StreamKeyWorker : public Worker {
		struct XPakEntryInfo {
			uint64_t key;
			int64_t offset;
			uint64_t size;
			unsigned __int64 xpakIndex : 5;
			unsigned __int64 compressed : 1;
			unsigned __int64 valid : 1;
			unsigned __int64 adjacentLeftType : 3;
			unsigned __int64 adjacentRightType : 3;
			unsigned __int64 adjacentLeft : 18;
			unsigned __int64 adjacentRight : 18;
			unsigned __int64 padding : 15;
		}; static_assert(sizeof(XPakEntryInfo) == 0x20);

		enum StreamKeyFlags : unsigned __int8 {
			SKF_STREAMER = 0x0,
			SKF_STREAMER_CPU = 0x2,
			SKF_MEMORY = 0x1,
		};

		struct StreamKey {
			XHash name;
			XPakEntryInfo xpakEntry;
			byte* data;
			uint64_t unk38;
			uint32_t size;
			uint16_t unk44;
			byte unk46;
			StreamKeyFlags keyFlags;
		}; static_assert(sizeof(StreamKey) == 0x48);
		std::vector<StreamKey*> keys{};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			keys.push_back((StreamKey*)ptr);
		}
		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			keys.clear();
		}
		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (keys.empty()) return; // nothing to dump

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "streamkey" / std::format("{}.json", ctx.ffname) };
			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			json.BeginObject();
			json.WriteFieldNameString("keys");
			json.BeginArray();

			for (StreamKey* key : keys) {
				json.BeginObject();
				json.WriteFieldNameString("name");
				json.WriteValueHash(key->name.name);
				json.WriteFieldNameString("xPakEntry");
				{
					json.BeginObject();
					json.WriteFieldNameString("key");
					json.WriteValueHash(key->xpakEntry.key);
					json.WriteFieldNameString("offset");
					json.WriteValueNumber(key->xpakEntry.offset);
					json.WriteFieldNameString("size");
					json.WriteValueNumber(key->xpakEntry.size);
					json.WriteFieldNameString("xpakIndex");
					json.WriteValueNumber(key->xpakEntry.xpakIndex);
					json.WriteFieldNameString("compressed");
					json.WriteValueBool(key->xpakEntry.compressed);
					json.WriteFieldNameString("valid");
					json.WriteValueBool(key->xpakEntry.valid);
					json.WriteFieldNameString("adjacentLeftType");
					json.WriteValueNumber(key->xpakEntry.adjacentLeftType);
					json.WriteFieldNameString("adjacentRightType");
					json.WriteValueNumber(key->xpakEntry.adjacentRightType);
					json.WriteFieldNameString("adjacentLeft");
					json.WriteValueNumber(key->xpakEntry.adjacentLeft);
					json.WriteFieldNameString("adjacentRight");
					json.WriteValueNumber(key->xpakEntry.adjacentRight);
					json.EndObject();
				}
				json.WriteFieldValueNumber("size", key->size);
				if (key->unk38) json.WriteFieldValueNumber("unk38", key->unk38);
				if (key->unk44) json.WriteFieldValueNumber("unk44", key->unk44);
				if (key->unk46) json.WriteFieldValueNumber("unk46", (int)key->unk46);
				json.WriteFieldValueNumber("keyFlags", (size_t)key->keyFlags);

				json.EndObject();
			}

			json.EndArray();
			json.EndObject();

			LOG_INFO("Dump streamkey({}) {}", keys.size(), outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

		}
	};

	utils::MapAdder<StreamKeyWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STREAMKEY };
}