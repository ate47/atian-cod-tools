#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;

	struct CpuOcclusionEntry {
		vec3_t minCorner;
		bool unk0c;
		vec3_t maxCorner;
		ScrString_t unk1c;
		uint64_t verticesCount;
		vec3_t* vertices;
		uint64_t indicesCount;
		uint32_t* indices;
		uint64_t planesCount;
		vec4_t* planes;
	};
	static_assert(sizeof(CpuOcclusionEntry) == 0x50);

	struct CPUOcclusionData {
		XHash name;
		int32_t count;
		uint32_t unk14;
		CpuOcclusionEntry* entries;
		uint64_t unk28_count;
		void* unk28;
		uint64_t unk38_count;
		void* unk38;
		uint64_t unk48_count;
		ScrString_t* unk48;
		// runtime data, allocate count
		vec4_t* runtimeUnk50;
		float* runtimeUnk58;
	};
	static_assert(sizeof(CPUOcclusionData) == 0x60);

	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			CPUOcclusionData* asset{ (CPUOcclusionData*)ptr };

			const char* name{ hashutils::ExtractPtr(asset->name) };
			if (!name) {
				name = utils::va("maps/hashed/file_%llx.d3dbsp", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / name / "cpu_occlusion_data.json" };

			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump cpuocclusiondata  {}", outFile.string());


			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueUnknown("unk14", asset->unk14);
			json.WriteFieldValueNumber("unk28_count", asset->unk28_count);
			json.WriteFieldValueNumber("unk38_count", asset->unk38_count);
			json.WriteFieldValueNumber("unk48_count", asset->unk48_count);
			json.WriteFieldValueNumber("count", asset->count);
			if (asset->unk48) {
				json.WriteFieldNameString("unk48");
				json.BeginArray();
				for (size_t i = 0; i < asset->unk48_count; i++) {
					json.WriteValueScrString(asset->unk48[i]);
				}
				json.EndArray();
			}

			if (asset->entries) {
				json.WriteFieldNameString("entries");
				json.BeginArray();

				for (size_t i = 0; i < asset->count; i++) {
					json.BeginObject();

					CpuOcclusionEntry& sub{ asset->entries[i] };
					json.WriteFieldValueVector("minCorner", sub.minCorner);
					if (sub.unk0c) json.WriteFieldValueBool("sub:unk0c", sub.unk0c);
					json.WriteFieldValueVector("maxCorner", sub.maxCorner);
					json.WriteFieldValueScrString("sub:unk1c", sub.unk1c);
					if (sub.vertices) {
						json.WriteFieldNameString("vertices");
						json.BeginArray();
						for (size_t j = 0; j < sub.verticesCount; j++) {
							json.WriteValueVector(sub.vertices[j]);
						}
						json.EndArray();
					}
					if (sub.indices) {
						json.WriteFieldNameString("indices");
						json.BeginArray();
						for (size_t j = 0; j < sub.indicesCount; j++) {
							json.WriteValueNumber(sub.indices[j]);
						}
						json.EndArray();
					}
					if (sub.planes) {
						json.WriteFieldNameString("planes");
						json.BeginArray();
						for (size_t j = 0; j < sub.planesCount; j++) {
							json.WriteValueVector(sub.planes[j]);
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

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_CPU_OCCLUSION_DATA };
}