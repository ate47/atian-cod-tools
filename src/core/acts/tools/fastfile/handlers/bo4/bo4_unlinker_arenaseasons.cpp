#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <hook/error.hpp>

namespace {
	struct ArenaSeasonEvent {
		int32_t eventType;
		int32_t leagueID;
		int32_t startOffset;
		int32_t endOffset;
		bool changesDaily;
		uint32_t unk14;
		XHash category;
		XHash event;
		XHash bucket;
	};
	static_assert(sizeof(ArenaSeasonEvent) == 0x48);

	struct ArenaSeasonSchedule {
		XHash hash;
		uint32_t unk10;
		int32_t eventsCount;
		ArenaSeasonEvent* events;
	};
	static_assert(sizeof(ArenaSeasonSchedule) == 0x20);

	struct ArenaSeasonDef {
		XHash hash;
		int32_t schedulesCount;
		ArenaSeasonSchedule** schedules;
	};
	static_assert(sizeof(ArenaSeasonDef) == 0x20);

	struct ArenaSeason {
		uint32_t timeStart;
		uint32_t timeEnd;
		ArenaSeasonDef* seasonDefinition;
	};
	static_assert(sizeof(ArenaSeason) == 0x10);

	struct ArenaSeasonSkillBucketValue {
		float startRange;
		float endRange;
	};
	static_assert(sizeof(ArenaSeasonSkillBucketValue) == 0x8);

	struct ArenaSeasonSkillBucket {
		XHash hash;
		ArenaSeasonSkillBucketValue* values;
		uint32_t valuesCount;
	};
	static_assert(sizeof(ArenaSeasonSkillBucket) == 0x20);

	struct ArenaSeasons {
		uint64_t unk0;
		XHash name;
		uint32_t unk18;
		int32_t nSeasons;
		ArenaSeason* seasonlist;
		ArenaSeasonSkillBucket* skillBuckets;
		uint32_t skillBucketsCount;
		uint32_t unk34;
	};
	static_assert(sizeof(ArenaSeasons) == 0x38);



	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			ArenaSeasons& asset{ *(ArenaSeasons*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "arenaseasons" / std::format("{}.json", hashutils::ExtractTmp("file", asset.name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump arenaseasons {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueUnknown("unk0", asset.unk0);
			if (asset.unk18) json.WriteFieldValueNumber("unk18", asset.unk18);

			if (asset.seasonlist) {
				json.WriteFieldNameString("seasons");
				json.BeginArray();

				for (size_t i = 0; i < asset.nSeasons; i++) {
					ArenaSeason& season{ asset.seasonlist[i] };
					json.BeginObject();
					json.WriteFieldValueNumber("timeStart", season.timeStart);
					json.WriteFieldValueNumber("timeEnd", season.timeEnd);
					if (season.seasonDefinition) {
						json.WriteFieldNameString("seasonDefinition");
						json.BeginObject();
						json.WriteFieldValueXHash("name", season.seasonDefinition->hash);
						if (season.seasonDefinition->schedules) {
							json.WriteFieldNameString("schedules");
							json.BeginArray();
							for (size_t j = 0; j < season.seasonDefinition->schedulesCount; j++) {
								ArenaSeasonSchedule* sch{ season.seasonDefinition->schedules[j] };

								if (!sch) {
									json.WriteValueNull();
									continue;
								}
								json.BeginObject();
								json.WriteFieldValueXHash("name", sch->hash);
								if (sch->unk10) json.WriteFieldValueNumber("sch:unk10", sch->unk10);
								if (sch->events) {
									json.WriteFieldNameString("events");
									json.BeginArray();
									for (size_t k = 0; k < sch->eventsCount; k++) {
										ArenaSeasonEvent& ev{ sch->events[k] };
										json.BeginObject();
										json.WriteFieldValueXHash("name", ev.event);
										json.WriteFieldValueBool("changesDaily", ev.changesDaily);
										json.WriteFieldValueNumber("eventType", ev.eventType);
										json.WriteFieldValueNumber("leagueID", ev.leagueID);
										json.WriteFieldValueNumber("startOffset", ev.startOffset);
										json.WriteFieldValueNumber("endOffset", ev.endOffset);
										json.WriteFieldValueUnknown("ev:unk14", ev.unk14);
										json.WriteFieldValueXHash("category", ev.category);
										json.WriteFieldValueXHash("bucket", ev.bucket);
										json.EndObject();
									}
									json.EndArray();
								}


								json.EndObject();
							}
							json.EndArray();
						}

						json.EndObject();
					}
					json.EndObject();
				}
				json.EndArray();
			}

			if (asset.skillBuckets) {
				json.WriteFieldNameString("skillBuckets");
				json.BeginArray();

				for (size_t i = 0; i < asset.skillBucketsCount; i++) {
					ArenaSeasonSkillBucket& bucket{ asset.skillBuckets[i] };
					json.BeginObject();

					json.WriteFieldValueXHash("name", bucket.hash);
					if (bucket.values) {
						json.WriteFieldNameString("values");
						json.BeginArray();
						for (size_t j = 0; j < bucket.valuesCount; j++) {
							ArenaSeasonSkillBucketValue& bv{ bucket.values[j] };
							json.BeginObject();
							json.WriteFieldValueNumber("startRange", bv.startRange);
							json.WriteFieldValueNumber("endRange", bv.endRange);
							json.EndObject();
						}
						json.EndArray();
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

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ARENASEASONS };
}