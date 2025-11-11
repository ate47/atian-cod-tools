#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo6;

	struct SndAcoustics;
	struct SndAsset;
	struct SoundSubmix;
	struct SoundSpeakerMap;
	struct SndFutz;
	struct SoundCone;
	struct SndOcclusionSetting;
	struct SndModifier;

	struct SndAliasInfo {
		SoundSubmix* soundSubmix;
		SoundSpeakerMap* soundSpeakerMap;
		SndFutz* sndFutz;
		SoundCone* soundCone;
		SndOcclusionSetting* sndOcclusionSetting;
		uint64_t unk28;
		SndModifier* sndModifier;
		XHash64 type;
		uint64_t __pad[27];
	};
	static_assert(sizeof(SndAliasInfo) == 0x118);

	struct SndBankAliasText {
		XHash64 subtitles;
		XHash64 character;
		XHash64 animName;
		uint64_t unk18;
	};
	static_assert(sizeof(SndBankAliasText) == 0x20);

	struct SndBankAlias {
		uint64_t unk0;
		SndAliasInfo* info;
		SndBankAliasText* text;
		XHash64 name;
		uint64_t unk20;
		SndAsset* sndAsset;
		float unk30;
		uint32_t unk34;
	};
	static_assert(sizeof(SndBankAlias) == 0x38);

	struct SndBankAliases {
		SndBankAliases* unk0;
		SndBankAliases* unk8;
		SndBankAlias* aliases;
		XHash64 name;
		uint64_t unk20;
		uint32_t unk28;
		uint16_t count;
		uint16_t unk2e;
		uint64_t unk30;
		uint64_t unk38;
	};
	static_assert(sizeof(SndBankAliases) == 0x40);

	struct SndBank {
		XHash64 name;
		XHash64 unk8;
		XHash64 language;
		XHash64 type;
		SndBankAliases* aliases;
		uint32_t* unk28;
		uint64_t unk30;
		uint64_t sndMusicSets;
		uint64_t weaponReflectionDefs;
		uint32_t unk48;
		uint32_t weaponReflectionDefsCount;
		uint32_t unk50;
		uint32_t count;
		uint64_t unk58;
	};
	static_assert(sizeof(SndBank) == 0x60);

	class ImplWorker : public Worker {
		bool isTransient;
	public:

		void GenDefaultXHashes(fastfile::FastFileContext* ctx) override {

			// todo: add all languages
			const char* languages[]{ "ens", "eng", "fra" };
			const char* types[]{ "all", "english", "french" };
			if (!ctx) {
				for (auto& type : types) {
					hashutils::AddPrecomputed(hash::Hash64A(type), type, true, false);
				}
				for (auto& language : languages) {
					hashutils::AddPrecomputed(hash::Hash64A(language), language, true, false);
				}
				return;
			}

			for (auto& type : types) {
				const char* str{ utils::va("%s.%s", ctx->ffname, type) };
				hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
			}
		}

		ImplWorker(bool isTransient) : Worker(sizeof(SndBank)), isTransient(isTransient) {};

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			SndBank* asset{ (SndBank*)ptr };
			BO6JsonWriter json{};

			bool anyTranslation{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("unk8", asset->unk8);
			json.WriteFieldValueXHash("language", asset->language);
			json.WriteFieldValueXHash("type", asset->type);

			if (opt.testDump) {
				json.WriteFieldValueUnknown("unk30", asset->unk30);
				json.WriteFieldValueUnknown("unk48", asset->unk48);
				json.WriteFieldValueUnknown("unk50", asset->unk50);
				json.WriteFieldValueUnknown("unk58", asset->unk58);
			}

			if (asset->aliases) {
				json.WriteFieldNameString("aliases");
				json.BeginArray();
				for (size_t i = 0; i < asset->count; i++) {
					SndBankAliases* aliases{ &asset->aliases[i] };

					json.BeginObject();

					json.WriteFieldValueXHash("name", aliases->name);

					if (aliases->aliases) {
						json.WriteFieldNameString("aliases");
						json.BeginArray();
						for (size_t j = 0; j < aliases->count; j++) {
							SndBankAlias* alias{ &aliases->aliases[j] };
							json.BeginObject();
							json.WriteFieldValueXHash("name", alias->name);
							json.WriteFieldValueXAsset("sndAsset", bo6::T10H_ASSET_SNDASSET, alias->sndAsset);
							if (opt.testDump) {
								json.WriteFieldValueUnknown("alias:unk0", alias->unk0);
								json.WriteFieldValueUnknown("alias:unk20", alias->unk20);
								json.WriteFieldValueNumber("alias:unk30", alias->unk30);
								json.WriteFieldValueUnknown("alias:unk34", alias->unk34);
							}

							if (alias->info) {
								SndAliasInfo* info{ alias->info };

								json.WriteFieldValueXHash("type", info->type);
								json.WriteFieldValueXAsset("soundSubmix", bo6::T10H_ASSET_SOUNDSUBMIX, info->soundSubmix);
								json.WriteFieldValueXAsset("soundSpeakerMap", bo6::T10H_ASSET_SOUNDSPEAKERMAP, info->soundSpeakerMap);
								json.WriteFieldValueXAsset("sndFutz", bo6::T10H_ASSET_SNDFUTZ, info->sndFutz);
								json.WriteFieldValueXAsset("soundCone", bo6::T10H_ASSET_SOUNDCONE, info->soundCone);
								json.WriteFieldValueXAsset("sndOcclusionSetting", bo6::T10H_ASSET_SNDOCCLUSIONSETTING, info->sndOcclusionSetting);
								json.WriteFieldValueXAsset("sndModifier", bo6::T10H_ASSET_SNDMODIFIER, info->sndModifier);

								if (opt.testDump) {
									// vector and data shits
									json.WriteFieldValueUnknown("alias.info.unk28", info->unk28);
									for (size_t i = 0; i < ARRAYSIZE(info->__pad); i++) {
										json.WriteFieldValueUnknown(utils::va("alias.info.unk%02x", i * 8 + offsetof(SndAliasInfo, __pad)), info->__pad[i]);
									}
								}
							}
							if (alias->text) {
								if (alias->text->subtitles) {
									json.WriteFieldValueString("subtitles", opt.GetTranslation(alias->text->subtitles));
									anyTranslation = true;
								}
								if (alias->text->character) {
									json.WriteFieldValueString("character", opt.GetTranslation(alias->text->character));
									anyTranslation = true;
								}
								json.WriteFieldValueXHash("animName", alias->text->animName);

								if (opt.testDump) {
									json.WriteFieldValueNumber("alias.text.unk18", alias->text->unk18);
								}
							}
							json.EndObject();
						}

						json.EndArray();
					}

					if (opt.testDump) {
						json.WriteFieldValueUnknown("aliases:unk20", aliases->unk20);
						json.WriteFieldValueUnknown("aliases:unk28", aliases->unk28);
						json.WriteFieldValueUnknown("aliases:unk2e", aliases->unk2e);
						json.WriteFieldValueUnknown("aliases:unk30", aliases->unk30);
						json.WriteFieldValueUnknown("aliases:unk38", aliases->unk38);
					}

					json.EndObject();
				}
				json.EndArray();
			}

			json.EndObject();


			const char* type{ isTransient ? "banktransient" : "bank" };
			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "sound" / type / ctx.ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump sound{} {}", type, outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (opt.translation && anyTranslation) {
				std::filesystem::path outDialogs{ outFile };
				outDialogs.replace_extension(".txt");

				LOG_INFO("Dump sound{} {}", type, outDialogs.string());

				utils::OutFileCE os{ outDialogs };
				if (!os) {
					LOG_ERROR("Error when dumping {}", outDialogs.string());
					return;
				}

				os << "(#name,#sndAsset) Character : Text\n";

				if (asset->aliases) {
					for (size_t i = 0; i < asset->count; i++) {
						SndBankAliases* aliases{ &asset->aliases[i] };
						if (aliases->aliases) {
							for (size_t j = 0; j < aliases->count; j++) {
								SndBankAlias* alias{ &aliases->aliases[j] };
								if ((!alias->text->subtitles && !alias->text->character)) continue; // invalid

								if (alias->text) {
									os 
										<< "("
										<< "#" << hashutils::ExtractTmp("hash", alias->name)
										<< "/#" << hashutils::ExtractTmp("hash", GetXAssetName(bo6::T10HashAssetType::T10H_ASSET_SNDASSET, alias->sndAsset))
										<< ") "
										<< opt.GetTranslation(alias->text->character) << " : "
										<< opt.GetTranslation(alias->text->subtitles) << "\n";
								}
							}
						}
					}
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_SOUNDBANK, false };
	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> implt{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_SOUNDBANKTRANSIENT, true };
}