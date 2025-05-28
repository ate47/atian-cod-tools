#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <decryptutils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;


	enum DLogEventUnkFieldMemberType : int32_t {
		DLEMT_INVALID = 0x0,
		DLEMT_BOOL = 0x1,
		DLEMT_FLOAT = 0x2,
		DLEMT_INT8 = 0x3,
		DLEMT_INT16 = 0x4,
		DLEMT_INT32 = 0x5,
		DLEMT_INT64 = 0x6,
		DLEMT_UINT8 = 0x7,
		DLEMT_SHORT = 0x8,
		DLEMT_UINT32 = 0x9,
		DLEMT_UINT64 = 0xa,
		DLEMT_STRING = 0xb,
		DLEMT_UNKc = 0xc,
		DLEMT_XHASH = 0xd,
	};
	const char* DLogEventUnkFieldMemberTypeNames[]{
		"INVALID",
		"BOOL",
		"FLOAT",
		"INT8",
		"INT16",
		"INT32",
		"INT64",
		"UINT8",
		"UINT16",
		"UINT32",
		"UINT64",
		"STRING",
		"UNK12",
		"XHASH",
	};

	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct __declspec(align(8)) DLogEventUnk31
			{
				XHash name;
				uint32_t unk10;
				uint32_t unk14;
				uint32_t unk18;
				uint32_t unk1c;
				bool unk20;
				uint32_t* unk28;
				int32_t unk28_count;
			};
			static_assert(sizeof(DLogEventUnk31) == 0x38);
			struct DLogEventUnk3
			{
				uint32_t unk0;
				uint32_t unk4;
				DLogEventUnk31* unk8;
				uint32_t unk10;
				uint32_t unk14;
				float unk18;
				float unk1c;
				uint64_t unk20;
			};
			static_assert(sizeof(DLogEventUnk3) == 0x28);

			struct DLogEventFieldMember
			{
				XHash name;
				uint32_t scrName;
				DLogEventUnkFieldMemberType type;
				uint64_t len;
				bool unk20;
			};
			static_assert(sizeof(DLogEventFieldMember) == 0x28);

			struct DLogEventFieldVal {
				XHash name;
				const char* unk10;
				DLogEventFieldMember* members;
				int32_t numMembers;
			};
			static_assert(sizeof(DLogEventFieldVal) == 0x28);

			struct DLogEventField {
				XHash name;
				DLogEventFieldVal* members;
				uint64_t arraySize;
			};
			static_assert(sizeof(DLogEventField) == 0x20);

			struct DDLRoot;
			struct DLogEvent {
				char* unk0;
				XHash name;
				byte unk18;
				byte unk19;
				byte unk1a;
				byte unk1b;
				byte unk1c;
				float rate;
				XHash unk28;
				DDLRoot* ddlDef;
				DLogEventField* fields;
				int32_t numFields;
				DLogEventFieldMember* members;
				int32_t numMembers;
				DLogEventUnk3* unk60;
				int32_t unk60_count;
			};

			static_assert(sizeof(DLogEvent) == 0x70);
			DLogEvent* asset{ (DLogEvent*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "dlogevent" / std::format("{}.json", n) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump dlogevent {}", outFile.string());

			auto WriteType = [&json](const char* name, DLogEventUnkFieldMemberType type) {

				if (type < ARRAYSIZE(DLogEventUnkFieldMemberTypeNames)) {
					json.WriteFieldValueString("type", DLogEventUnkFieldMemberTypeNames[type]);
				}
				else {
					json.WriteFieldValueString("type", utils::va("DLEMT_UNK%d", type));
				}
			};

			auto WriteMember = [&WriteType, &json](DLogEventFieldMember* member) {
				json.BeginObject();
				json.WriteFieldValueXHash("name", member->name);
				json.WriteFieldValueHash("scrName", member->scrName);
				WriteType("type", member->type);
				if (member->len) json.WriteFieldValueNumber("len", member->len);
				if (member->unk20) json.WriteFieldValueBool("s111u20", member->unk20);
				json.EndObject();
			};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			//if (asset->unk0) {
			//	//acts::decryptutils::DecryptStringT8(asset->unk0);
			//	//hashutils::Add(asset->unk0);
			//	//json.WriteFieldValueString("unk0", asset->unk0);
			//}
			json.WriteFieldValueXHash("unk28", asset->unk28);
			json.WriteFieldValueXAsset("ddlDef", games::bo4::pool::XAssetType::ASSET_TYPE_DDL, asset->ddlDef);

			json.WriteFieldValueNumber<int>("unk18", asset->unk18);
			json.WriteFieldValueNumber<int>("unk19", asset->unk19);
			json.WriteFieldValueNumber<int>("unk1a", asset->unk1a);
			json.WriteFieldValueNumber<int>("unk1b", asset->unk1b);
			json.WriteFieldValueNumber<int>("unk1c", asset->unk1c);
			json.WriteFieldValueNumber("rate", asset->rate);

			if (asset->fields) {
				json.WriteFieldNameString("fields");
				json.BeginArray();
				for (size_t i = 0; i < asset->numFields; i++) {
					DLogEventField* sub{ asset->fields + i };
					json.BeginObject();
					json.WriteFieldValueXHash("name", sub->name);
					json.WriteFieldValueNumber("arraySize", sub->arraySize);
					if (sub->members) {
						json.WriteFieldNameString("members");
						json.BeginObject();
						DLogEventFieldVal* sub11{ sub->members };

						json.WriteFieldValueXHash("name", sub11->name);
						if (sub11->unk10) json.WriteFieldValueString("s11unk10", sub11->unk10);
						if (sub11->members) {
							json.WriteFieldNameString("members");
							json.BeginArray();
							for (size_t j = 0; j < sub11->numMembers; j++) {
								WriteMember(sub11->members + j);
							}
							json.EndArray();
						}
						json.EndObject();
					}

					json.EndObject();
				}
				json.EndArray();

			}

			if (asset->members) {
				json.WriteFieldNameString("members");
				json.BeginArray();
				for (size_t i = 0; i < asset->numMembers; i++) {
					WriteMember(asset->members + i);
				}
				json.EndArray();
			}

			if (asset->unk60) {
				json.WriteFieldNameString("unk60");
				json.BeginArray();
				for (size_t i = 0; i < asset->unk60_count; i++) {
					DLogEventUnk3* sub{ asset->unk60 + i };
					json.BeginObject();
					/*
			struct __declspec(align(8)) DLogEventUnk31
			{
				XHash name;
				uint64_t unk10;
				uint64_t unk18;
				uint64_t unk20;
				uint32_t* unk28;
				int32_t unk28_count;
			};
			struct DLogEventUnk3
			{
				uint64_t unk0;
				DLogEventUnk31* unk8;
			};*/
					if (sub->unk0)json.WriteFieldValueHash("s3u0", sub->unk0);
					if (sub->unk4)json.WriteFieldValueHash("s3u4", sub->unk4);
					if (sub->unk10)json.WriteFieldValueHash("s3u10", sub->unk10);
					if (sub->unk14)json.WriteFieldValueHash("s3u14", sub->unk14);
					json.WriteFieldValueNumber("s3u18", sub->unk18);
					json.WriteFieldValueNumber("s3u1c", sub->unk1c);
					if (sub->unk20)json.WriteFieldValueHash("s3u20", sub->unk20);

					if (sub->unk8) {
						json.WriteFieldNameString("s3u8");
						DLogEventUnk31* sub31{ sub->unk8 };
						json.BeginObject();
						json.WriteFieldValueHash("s31u0", sub31->name);
						if (sub31->unk10)json.WriteFieldValueHash("s3u10", sub31->unk10);
						if (sub31->unk18)json.WriteFieldValueHash("s3u18", sub31->unk18);
						if (sub31->unk1c)json.WriteFieldValueHash("s3u1c", sub31->unk1c);
						if (sub31->unk20)json.WriteFieldValueBool("s3u20", sub31->unk20);
						if (sub31->unk28) {
							json.WriteFieldNameString("unk28");

							json.BeginArray();
							for (size_t j = 0; j < sub31->unk28_count; j++) {
								json.WriteValueNumber(sub31->unk28[j]);
							}
							json.EndArray();
						}
						json.EndObject();
					}

					json.EndObject();
				}
				json.EndArray();
			}

			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_DLOGEVENT };
}