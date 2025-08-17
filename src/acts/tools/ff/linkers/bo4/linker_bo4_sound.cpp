#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {

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

	struct SndBankStreamKeys {
		StreamKey* streamKey1;
		byte* data1;
		uint32_t len1;
		StreamKey* fileKey;
		byte* fileData;
		uint32_t fileLen;
	};

	struct SndIndexEntry {
		unsigned __int16 value;
		unsigned __int16 next;
	};
	struct SndAlias {
		XHash name;
		XHash subtitle;
		XHash secondaryname;
		XHash stopAliasName;
		ScrString_t facialAnimation;
		XHash assetId;
		XHash assetFileName;
		XHash assetIdSustain;
		XHash assetFileNameSustain;
		XHash assetIdRelease;
		XHash assetFileNameRelease;
		uint64_t flags0;
		uint64_t flags1;
		XHash duck;
		XHash contextType;
		XHash contextValue;
		XHash contextType1;
		XHash contextValue1;
		XHash contextType2;
		XHash contextValue2;
		XHash contextType3;
		XHash contextValue3;
		XHash stopOnPlay;
		XHash futzPatch;
		XHash unk168;
		float reverbSend;
		float centerSend;
		float volMin;
		float volMax;
		float envelopPercentage;
		uint16_t fluxTime;
		uint16_t startDelay;
		uint16_t pitchMin;
		uint16_t pitchMax;
		uint16_t distMin;
		uint16_t distMax;
		uint16_t distReverbMax;
		uint16_t envelopMin;
		uint16_t envelopMax;
		uint16_t distanceLpfMin;
		uint16_t distanceLpfMax;
		int16_t fadeIn;
		int16_t fadeOut;
		int16_t dopplerScale;
		uint8_t minPriorityThreshold;
		uint8_t maxPriorityThreshold;
		uint8_t probability;
		uint8_t occlusionLevel;
		uint8_t minPriority;
		uint8_t maxPriority;
		uint8_t pan;
		uint8_t limitCount;
		uint8_t entityLimitCount;
		uint8_t duckGroup;
		uint8_t bus;
		uint8_t group;
		uint64_t unk178;
	};
	static_assert(sizeof(SndAlias) == 0x1c0);

	struct SndAliasList {
		XHash name;
		SndAlias* aliases;
		int32_t count;
		int32_t sequence;
		float cullDistance;
		bool spatial;
	};
	static_assert(sizeof(SndAliasList) == 0x28);

	typedef float SndFloatAlign32;

	struct SndDuck {
		char name[64];
		XHash id;
		float fadeIn;
		float fadeOut;
		float startDelay;
		float distance;
		float length;
		XHash fadeInCurve;
		XHash fadeOutCurve;
		SndFloatAlign32* attenuation;
		SndFloatAlign32* filter;
		SndFloatAlign32* unk98;
		bool updateWhilePaused;
		bool trackAmplitude;
		bool disableInSplitscreen;
		float aliasAttenuation;
		float aliasLpf;
		XHash duckAlias;
	};
	static_assert(sizeof(SndDuck) == 0xc0);

	struct SndAmbient {
		char name[64];
		XHash id;
		bool defaultRoom;
		XHash reverb;
		char reverbName[64];
		XHash nearVerb;
		char nearVerbName[64];
		XHash farVerb;
		char farVerbName[64];
		float reverbDryLevel;
		float reverbWetLevel;
		float unk230;
		float unk234;
		XHash loop;
		char duck[64];
		XHash duckId;
		XHash entityContextType0;
		XHash entityContextValue0;
		XHash entityContextType1;
		XHash entityContextValue1;
		XHash entityContextType2;
		XHash entityContextValue2;
		XHash globalContextType;
		XHash globalContextValue;
	};
	static_assert(sizeof(SndAmbient) == 0x238);

	struct SndDialogScriptIdLookup {
		XHash scriptId;
		XHash aliasId;
	};
	static_assert(sizeof(SndDialogScriptIdLookup) == 0x20);


	struct SndBank {
		const char* name;
		XHash nameHash;
		const char* zone;
		const char* gameLanguage;
		const char* soundLanguage;
		uint32_t aliasCount;
		SndAliasList* alias;
		SndIndexEntry* aliasIndex;
		uint32_t reverbCount;
		uint64_t reverbs;
		uint32_t duckCount;
		SndDuck* ducks;
		uint32_t ambientCount;
		SndAmbient* ambients;
		uint32_t scriptIdLookupCount;
		SndDialogScriptIdLookup* scriptIdLookups;
		uint32_t unk90_count;
		uint64_t unk90;
		uint32_t unka0_count;
		uint64_t unka0;
		uint32_t unkb0_count;
		uint64_t unkb0;
		uint32_t unkc0_count;
		uint64_t unkc0;
		SndBankStreamKeys streamKeysSABL;
		SndBankStreamKeys streamKeysSABS;
		bool patchZone;
	};
	static_assert(sizeof(SndBank) == 0x130);

	constexpr uint32_t MAGIC = 0x23585532;
	enum SndAssetBankVersion : uint32_t {
		SNDV15 = 0x15,
	};
	enum SndAssetFormat : byte {
		SND_ASSET_FORMAT_PCMS16 = 0x0,
		SND_ASSET_FORMAT_PCMS24 = 0x1,
		SND_ASSET_FORMAT_PCMS32 = 0x2,
		SND_ASSET_FORMAT_IEEE = 0x3,
		SND_ASSET_FORMAT_XMA4 = 0x4,
		SND_ASSET_FORMAT_MP3 = 0x5,
		SND_ASSET_FORMAT_MSADPCM = 0x6,
		SND_ASSET_FORMAT_WMA = 0x7,
		SND_ASSET_FORMAT_FLAC = 0x8,
		SND_ASSET_FORMAT_WIIUADPCM = 0x9,
		SND_ASSET_FORMAT_MPC = 0xA,
		SND_ASSET_FORMAT_COUNT = 0xB,
	};

	const char* SndAssetFormatNames[]{
		"PCMS16",
		"PCMS24",
		"PCMS32",
		"IEEE",
		"XMA4",
		"MP3",
		"MSADPCM",
		"WMA",
		"FLAC",
		"WIIUADPCM",
		"MPC",
	};
	const char* SndAssetFormatName(SndAssetFormat format) {
		if (format < ARRAYSIZE(SndAssetFormatNames)) return SndAssetFormatNames[format];
		return utils::va("Invalid:%d", (int)format);
	}

	SndAssetFormat SndAssetFormatId(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(SndAssetFormatNames); i++) {
			if (!_strcmpi(SndAssetFormatNames[i], name)) {
				return (SndAssetFormat)i;
			}
		}
		return SndAssetFormat::SND_ASSET_FORMAT_COUNT;
	}

	struct SndAssetBankEntryBo4 {
		uint64_t id;
		uint64_t idnull;
		uint64_t offset;
		uint32_t size;
		uint32_t frameCount;
		uint32_t order;
		uint32_t unk24;
		uint8_t frameRateIndex;
		uint8_t channelCount;
		uint8_t looping;
		SndAssetFormat format;
		uint32_t unk3c;
		std::string buffer{};

		void Read(core::bytebuffer::AbstractByteBuffer& buffer) {
			// XHash
			id = buffer.Read<uint64_t>(); // 8
			buffer.Skip<uint64_t>(); // 10
			offset = buffer.Read<uint64_t>(); // 18
			size = buffer.Read<uint32_t>(); // 1c
			frameCount = buffer.Read<uint32_t>(); // 20
			order = buffer.Read<uint32_t>(); // 24
			unk24 = buffer.Read<uint32_t>(); // 28
			frameRateIndex = buffer.Read<uint8_t>(); // 29
			channelCount = buffer.Read<uint8_t>(); // 2a
			looping = buffer.Read<uint8_t>(); // 2b
			format = buffer.Read<SndAssetFormat>(); // 2c
			unk3c = buffer.Read<uint32_t>(); // 30
		}

		void Write(std::vector<byte>& data) const {
			size_t k{ data.size() };
			utils::WriteValue<uint64_t>(data, id); // 8
			utils::WriteValue<uint64_t>(data, 0);// 10
			utils::WriteValue<uint64_t>(data, offset); // 18
			utils::WriteValue<uint32_t>(data, size); // 1c
			utils::WriteValue<uint32_t>(data, frameCount); // 20
			utils::WriteValue<uint32_t>(data, order); // 24
			utils::WriteValue<uint32_t>(data, unk24); // 28
			utils::WriteValue<uint8_t>(data, frameRateIndex); // 29
			utils::WriteValue<uint8_t>(data, channelCount); // 2a
			utils::WriteValue<uint8_t>(data, looping); // 2b
			utils::WriteValue<SndAssetFormat>(data, format); // 2c
			utils::WriteValue<uint32_t>(data, unk3c); // 30
			assert(data.size() - k == 0x30 && "invalid asset write");
		}
	};
	struct SndAssetName {
		uint64_t hash;
		byte pad[0x78];
	};
	static_assert(sizeof(SndAssetName) == 0x80);
	struct SndAssetBankHeader {
		uint32_t magic;
		SndAssetBankVersion version;
		uint32_t entrySize;
		uint32_t checksumSize;
		uint32_t dependencySize;
		uint32_t entryCount;
		uint32_t dependencyCount;
		uint32_t pad32;
		int64_t fileSize;
		int64_t entryOffset;
		int64_t checksumOffset;
		unsigned __int8 checksumChecksum[16];
		char dependencies[512];
		int64_t SourceChecksumOffset;
		int64_t AssetNameOffset;
		char zoneName[64];
		char platform[8];
		char language[2];
		char convertedAssetVersion0[4];
		char padding[1370];
	};
	static_assert(sizeof(SndAssetBankHeader) == 0x800);

	size_t CreateSabFile(const std::filesystem::path& path, const std::string& lang, const std::string& plt, std::vector<SndAssetBankEntryBo4>& entries) {

		std::vector<byte> sab{};

		// alloc header
		utils::Allocate<SndAssetBankHeader>(sab);

		size_t assetNameOffset{ sab.size() };
		utils::AllocateArray<SndAssetName>(sab, entries.size());
		size_t checksumOffset{ sab.size() };
		utils::Allocate(sab, entries.size() * 0x10);
		size_t srcChecksumOffset{ sab.size() };
		utils::Allocate(sab, entries.size() * 0x10);

		for (size_t i = 0; i < entries.size(); i++) {
			SndAssetBankEntryBo4& entry{ entries[i] };
			// write entry
			entry.offset = (uint64_t)utils::WriteValue(sab, entry.buffer.data(), entry.buffer.size());
			entry.size = (uint32_t)entry.buffer.size();
		}

		size_t entryOffsets{ sab.size() };
		for (size_t i = 0; i < entries.size(); i++) {
			const SndAssetBankEntryBo4& entry{ entries[i] };
			// write entry
			entry.Write(sab);
			// write name
			*(uint64_t*)&sab[assetNameOffset + i * sizeof(SndAssetName)] = entry.id;
		}

		// write header
		SndAssetBankHeader& sndHeader{ *(SndAssetBankHeader*)sab.data() };
		sndHeader.magic = MAGIC;
		sndHeader.version = SNDV15;
		*(uint32_t*)(&sndHeader.convertedAssetVersion0[0]) = 0x14;
		sndHeader.pad32 = 2; // idk

		//std::snprintf(sndHeader.zoneName, sizeof(sndHeader.zoneName), "%s", zone.c_str()); // no set?
		// cut at 2 chars fr,al,en,...
		std::memcpy(
			sndHeader.language, lang.data(),
			lang.size() < sizeof(sndHeader.language) ? lang.size() + 1 : sizeof(sndHeader.language)
		);
		std::memcpy(
			sndHeader.platform, plt.data(),
			plt.size() < sizeof(sndHeader.platform) ? plt.size() + 1 : sizeof(sndHeader.platform)
		);

		sndHeader.entrySize = 0x30;
		sndHeader.checksumSize = 0x10;
		sndHeader.dependencySize = 0x40;

		sndHeader.dependencyCount = 0;
		sndHeader.entryCount = (uint32_t)entries.size();
		sndHeader.entryOffset = (int64_t)entryOffsets;
		sndHeader.AssetNameOffset = (int64_t)assetNameOffset;
		sndHeader.SourceChecksumOffset = (int64_t)srcChecksumOffset;
		sndHeader.checksumOffset = (int64_t)checksumOffset;
		sndHeader.fileSize = (int64_t)sab.size();



		if (!utils::WriteFile(path, sab)) {
			LOG_ERROR("Can't write {}", path.string());
			return 0;
		}

		LOG_INFO("Created sab {} with {} entry(ies)", path.string(), entries.size());

		return sab.size();
	}

	void WriteStreamKey(BO4FFContext& ffctx, StreamKey* key) {
		ffctx.data.PushStream(XFILE_BLOCK_TEMP);
		ffctx.data.WriteData(*key);

		ffctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

		int flags{ key->keyFlags };
		// Load_StreamKeyData
		switch (flags & 3) {
		case 0: // stream
			ffctx.data.PushStream(XFILE_BLOCK_STREAMER);
			ffctx.data.Align(0x10000);
			ffctx.data.AllocRuntimeData(key->size);
			ffctx.data.PopStream();
			break;
		case 1: // stream cpu
			ffctx.data.PushStream(XFILE_BLOCK_STREAMER_CPU);
			ffctx.data.Align(0x10000);
			ffctx.data.AllocRuntimeData(key->size);
			ffctx.data.PopStream();
			break;
		case 2:
		case 3:
			ffctx.data.Align(0x10000);
			ffctx.data.AllocData(key->size);
			break;
		}
		ffctx.data.PopStream();

		ffctx.data.PopStream();
		LOG_INFO("Added subasset streamkey hash_{:x}", key->name.name);
	}

	void WriteStreamKeys(BO4FFContext& ffctx, SndBankStreamKeys& key) {
		if (key.streamKey1) {
			WriteStreamKey(ffctx, key.streamKey1);
		}
		if (key.data1) {
			ffctx.data.AllocData(key.len1);
		}
		if (key.fileKey) {
			WriteStreamKey(ffctx, key.fileKey);
		}
		if (key.fileData) {
			ffctx.data.AllocData(key.fileLen);
		}
	}

	class SoundWorker : public LinkerWorker {
	public:
		SoundWorker() : LinkerWorker("Sound") {}

		void Compute(BO4LinkContext& ctx) override {
			const char* sndPlatform{ ctx.linkCtx.zone.GetConfig("snd.platform", "pc") };
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["sound"]) {
				assval.handled = true;
				std::filesystem::path path{ ctx.linkCtx.input / assval.value };
				core::config::Config scfg{ path };
				if (!scfg.SyncConfig(false)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				std::string gameLanguage{ scfg.GetString("gameLanguage", "all") };
				std::string soundLanguage{ scfg.GetString("soundLanguage", "all") };

				BO4FFContext* pffctx;
				if (soundLanguage == "all") {
					pffctx = &ctx.mainFF;
				}
				else {
					pffctx = &ctx.GetFFContext(utils::va("%s_", soundLanguage.c_str()));
				}

				BO4FFContext& ffctx{ *pffctx };

				// shouldn't be set, but whatever
				std::string zone{ scfg.GetString("zone", ctx.linkCtx.mainFFName) };
				std::string name{ scfg.GetString("name", std::format("{}.{}", zone, soundLanguage)) };

				std::vector<SndAssetBankEntryBo4> streamEntries{};
				std::vector<SndAssetBankEntryBo4> loadEntries{};

				auto itLoaded{ scfg.main.FindMember("loaded") };
				if (itLoaded != scfg.main.MemberEnd()) {
					if (!itLoaded->value.IsArray()) {
						LOG_ERROR("{}: loaded field must be an array", assval.value);
						ctx.error = true;
						continue;
					}
					for (auto& v : itLoaded->value.GetArray()) {
						if (!v.IsObject()) {
							LOG_WARNING("{}: a loaded member isn't an object", assval.value);
							continue;
						}
						auto obj{ v.GetObj() };


						if (!obj.HasMember("file") || !obj["file"].IsString()) {
							LOG_WARNING("{}: a loaded member doesn't have a file name", assval.value);
							continue;
						}
						const char* file{ obj["file"].GetString() };
						if (!obj.HasMember("format") || !obj["format"].IsString()) {
							LOG_WARNING("{}: {} doesn't have a file format", assval.value, file);
							continue;
						}
						const char* format{ obj["format"].GetString() };
						int order{ obj.HasMember("order") ? obj["order"].GetInt() : 0 };
						int channelCount{ obj.HasMember("channelCount") ? obj["channelCount"].GetInt() : 0 };
						bool looping{ obj.HasMember("looping") ? obj["looping"].GetBool() : false };

						SndAssetBankEntryBo4 ent{};
						ent.id = ctx.HashXHash(file, true);
						ent.format = SndAssetFormatId(format);
						if (ent.format == SND_ASSET_FORMAT_COUNT) {
							LOG_WARNING("{}: {} doesn't have a valid file format '{}'", assval.value, file, format);
							continue;
						}

						std::filesystem::path path{ ctx.linkCtx.input / file };
						if (!utils::ReadFile(path, ent.buffer)) {
							LOG_ERROR("Can't read {}", path.string());
							ctx.error = true;
							return;
						}

						LOG_INFO("Loaded sab entry {} (hash_{:x}/{})", path.string(), ent.id, format);
						loadEntries.emplace_back(std::move(ent));
					}
				}

				LOG_TRACE("load sndbank {}", path.string());

				// todo: add that to the linker context instead of here
				std::filesystem::path dir{ ctx.linkCtx.opt.m_output / "zone" / "snd" / soundLanguage };
				std::filesystem::path sabs{ dir / std::format("{}.sabs", name) };
				std::filesystem::path sabl{ dir / std::format("{}.sabl", name) };
				std::filesystem::create_directories(dir);

				size_t sablSize{ CreateSabFile(sabl, soundLanguage, sndPlatform, loadEntries) };

				if (!sablSize) {
					ctx.error = true;
					continue;
				}

				ffctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SOUND, fastfile::linker::data::POINTER_NEXT);

				ffctx.data.PushStream(XFILE_BLOCK_TEMP);
				SndBank sndbank{};
				sndbank.nameHash.name = ctx.HashXHash(name);
				sndbank.name = (const char*)fastfile::ALLOC_PTR;
				sndbank.zone = (const char*)fastfile::ALLOC_PTR;
				sndbank.gameLanguage = (const char*)fastfile::ALLOC_PTR;
				sndbank.soundLanguage = (const char*)fastfile::ALLOC_PTR;

				StreamKey loadStreamKey1{};
				StreamKey loadStreamKeyFile{};
				loadStreamKey1.name.name = ctx.HashXHash(std::format("{}_load_key1", name));
				loadStreamKey1.xpakEntry.key = hash::Hash64("k", loadStreamKey1.name.name);
				loadStreamKey1.data = (byte*)fastfile::ALLOC_PTR;
				loadStreamKey1.keyFlags = (StreamKeyFlags)3;
				loadStreamKey1.unk44 = 512;
				loadStreamKey1.unk46 = 5;
				loadStreamKey1.size = 0x1000;
				sndbank.streamKeysSABL.streamKey1 = (StreamKey*)fastfile::ALLOC_PTR; 
				sndbank.streamKeysSABL.data1 = (byte*)fastfile::ALLOC_PTR;
				sndbank.streamKeysSABL.len1 = 0x1000;

				loadStreamKeyFile.name.name = ctx.HashXHash(std::format("{}_load_keyfile", name));
				loadStreamKeyFile.xpakEntry.key = hash::Hash64("k", loadStreamKeyFile.name.name);
				loadStreamKeyFile.data = (byte*)fastfile::ALLOC_PTR;
				loadStreamKeyFile.keyFlags = (StreamKeyFlags)1;
				loadStreamKeyFile.unk44 = 512;
				loadStreamKeyFile.unk46 = 5;
				loadStreamKeyFile.size = (uint32_t)sablSize;
				sndbank.streamKeysSABL.fileKey = (StreamKey*)fastfile::ALLOC_PTR;
				sndbank.streamKeysSABL.fileData = (byte*)fastfile::ALLOC_PTR;
				sndbank.streamKeysSABL.fileLen = (uint32_t)sablSize;

				sndbank.patchZone = scfg.GetBool("patchZone");

				ffctx.data.WriteData(sndbank);

				ffctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

				ffctx.data.WriteData(name);
				ffctx.data.WriteData(zone);
				ffctx.data.WriteData(gameLanguage);
				ffctx.data.WriteData(soundLanguage);

				// alias

				// alias index

				// reverbs

				// ducks

				// ambients

				// scriptIdLookups

				// unk90

				// unka0

				// unkb0

				// unkc0

				// stream keys

				sndbank.streamKeysSABL.streamKey1 = &loadStreamKey1;
				sndbank.streamKeysSABL.fileKey = &loadStreamKeyFile;
				WriteStreamKeys(ffctx, sndbank.streamKeysSABL);
				WriteStreamKeys(ffctx, sndbank.streamKeysSABS);

				ffctx.data.PopStream();

				ffctx.data.PopStream();



				LOG_INFO("Added asset sound {} (hash_{:x})", name, sndbank.nameHash.name);
			}
		}
	};

	utils::ArrayAdder<SoundWorker, LinkerWorker> impl{ GetWorkers() };
}