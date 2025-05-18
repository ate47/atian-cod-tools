#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct MaterialArgumentCodeConst {
				unsigned __int16 index;
				byte firstRow;
				byte rowCount;
			};

			union MaterialArgumentDef {
				MaterialArgumentCodeConst codeConst;
				uint32_t codeTexture;
				uint32_t nameHash;
				byte renderTargetId; // linked by the game
			};

			struct MaterialShaderArgument {
				uint16_t type;
				uint16_t location;
				uint16_t size;
				uint16_t buffer;
				MaterialArgumentDef u;
			};
			static_assert(sizeof(MaterialShaderArgument) == 0xc);

			struct MaterialTextureDef {
				GfxImage* image;
				uint32_t nameHash;
				vec2_t uvScale;
				byte texSizeShift[2];
				uint16_t uvOffset[3];
				uint16_t usage;
				byte isMatureContent;
			};
			static_assert(sizeof(MaterialTextureDef) == 0x20);
			struct MaterialSamplerDef
			{
				uint32_t nameHash;
				uint16_t samplerState; // GfxSamplerState
			};
			static_assert(sizeof(MaterialSamplerDef) == 0x8);

			struct MaterialComputeShaderMethod {
				XHash name;
				uint64_t unk10;
				byte* code;
				uint32_t codeSize;
				uint32_t unk24;
				uint32_t groupSize[3];
				uint32_t argCount;
				MaterialShaderArgument* args;
				byte unk40;
				byte unk41;
				byte textureCount;
				byte samplerCount;
				MaterialTextureDef* textureTable;
				MaterialSamplerDef* samplerTable;
			};
			static_assert(sizeof(MaterialComputeShaderMethod) == 0x58);

			struct MaterialComputeShader {
				XHash name;
				MaterialComputeShaderMethod methods[4];
			};
			static_assert(sizeof(MaterialComputeShader) == 0x170);

			struct MaterialComputeShaderSet {
				XHash name;
				uint32_t count;
				MaterialComputeShader** shaders;
			};

			static_assert(sizeof(MaterialComputeShaderSet) == 0x20);

			MaterialComputeShaderSet* asset{ (MaterialComputeShaderSet*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outDir{ opt.m_output / "bo4" / "graphics" / "computeshaderset" };
			std::filesystem::path outFile{ outDir / std::format("{}.json", n) };
			std::filesystem::create_directories(outDir);
			BO4JsonWriter json{};

			LOG_INFO("Dump computeshaderset {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);

			json.WriteFieldNameString("shaders");
			json.BeginArray();
			for (size_t i = 0; i < asset->count; i++) {
				MaterialComputeShader* shader{ asset->shaders[i] };
				if (!shader) {
					json.WriteValueNull();
					continue;
				}
				json.BeginObject();
				json.WriteFieldValueXHash("name", shader->name);

				json.WriteFieldNameString("methods");

				json.BeginArray();
				for (size_t i = 0; i < ARRAYSIZE(shader->methods); i++) {
					MaterialComputeShaderMethod* method{ shader->methods + i };

					if (!method->name) {
						json.WriteValueNull();
						continue;
					}
					json.BeginObject();
					json.WriteFieldValueXHash("name", method->name);
					const char* codeFileName{ 
						utils::va("%s_%s_%s.dxbc", 
							hashutils::ExtractTmp("hash", asset->name),
							hashutils::ExtractTmp("hash", shader->name),
							hashutils::ExtractTmp("hash", method->name)
						)
					};
					std::filesystem::path outCode{ outDir / codeFileName };

					if (!utils::WriteFile(outCode, method->code, method->codeSize)) {
						LOG_ERROR("Can't write {}", outCode.string());
					}
					else {
						LOG_INFO("Dump dxbc {}", outCode.string());
					}
					json.WriteFieldValueString("code", codeFileName);

					json.EndObject();
				}
				json.EndArray();

				json.EndObject();
			}
			json.EndArray();

			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_COMPUTE_SHADER_SET, true };
}