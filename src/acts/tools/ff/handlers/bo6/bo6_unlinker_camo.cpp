#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

	struct Camo {
		XHash64 name;
		GfxImage* image1[7];
		GfxImage* image2[7];
		GfxImage* image3[7];
		GfxImage* image4[7];
		GfxImage* image5[7];
		GfxImage* image;
		MaterialAnimation* materialAnimation;
		ParticleSystem* particleSystem1;
		ParticleSystem* particleSystem2;
		ScrString_t scrName;
		uint32_t unk144;
		uint64_t unk148;
		uint64_t unk150;
		uint64_t unk158;
		uint64_t unk160;
		uint64_t unk168;
		uint64_t unk170;
		uint64_t unk178;
		uint64_t unk180;
		uint64_t unk188;
		uint64_t unk190;
		uint64_t unk198;
		uint64_t unk1a0;
		uint64_t unk1a8;
		uint64_t unk1b0;
		uint64_t unk1b8;
		uint64_t unk1c0;
		uint64_t unk1c8;
		uint64_t unk1d0;
		uint64_t unk1d8;
		uint64_t unk1e0;
		uint64_t unk1e8;
		uint64_t unk1f0;
		uint64_t unk1f8;
		uint64_t unk200;
		uint64_t unk208;
		uint64_t unk210;
		uint64_t unk218;
		uint64_t unk220;
		uint64_t unk228;
		uint64_t unk230;
		uint64_t unk238;
		uint64_t unk240;
		uint64_t unk248;
		uint64_t unk250;
		uint64_t unk258;
		uint64_t unk260;
		uint64_t unk268;
		uint64_t unk270;
		uint64_t unk278;
		uint64_t unk280;
		uint64_t unk288;
		uint64_t unk290;
		uint64_t unk298;
		uint64_t unk2a0;
		uint64_t unk2a8;
		uint64_t unk2b0;
		uint64_t unk2b8;
		uint64_t unk2c0;
		uint64_t unk2c8;
		uint64_t unk2d0;
		uint64_t unk2d8;
		uint64_t unk2e0;
		uint64_t unk2e8;
		uint64_t unk2f0;
		uint64_t unk2f8;
		uint64_t unk300;
		uint64_t unk308;
		uint64_t unk310;
		uint64_t unk318;
		uint64_t unk320;
		uint64_t unk328;
		uint64_t unk330;
		uint64_t unk338;
		uint64_t unk340;
		uint64_t unk348;
		uint64_t unk350;
		uint64_t unk358;
		uint64_t unk360;
		uint64_t unk368;
		uint64_t unk370;
		uint64_t unk378;
		uint64_t unk380;
		uint64_t unk388;
		uint64_t unk390;
		uint64_t unk398;
		uint64_t unk3a0;
		uint64_t unk3a8;
		uint64_t unk3b0;
		uint64_t unk3b8;
		uint64_t unk3c0;
		uint64_t unk3c8;
		uint64_t unk3d0;
		uint64_t unk3d8;
		uint64_t unk3e0;
		uint64_t unk3e8;
		uint64_t unk3f0;
		uint64_t unk3f8;
		uint64_t unk400;
		uint64_t unk408;
		uint64_t unk410;
		uint64_t unk418;
		uint64_t unk420;
		uint64_t unk428;
		uint64_t unk430;
		uint64_t unk438;
		uint64_t unk440;
		uint64_t unk448;
		uint64_t unk450;
		uint64_t unk458;
		uint64_t unk460;
		uint64_t unk468;
		uint64_t unk470;
		uint64_t unk478;
		uint64_t unk480;
		uint64_t unk488;
		uint64_t unk490;
		uint64_t unk498;
		uint64_t unk4a0;
		uint64_t unk4a8;
		uint64_t unk4b0;
		uint64_t unk4b8;
		uint64_t unk4c0;
		uint64_t unk4c8;
		uint64_t unk4d0;
		uint64_t unk4d8;
	};
	static_assert(sizeof(Camo) == 0x4e0);

	class ImplWorker : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			Camo* asset{ (Camo*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueScrString("scrName", asset->scrName);
			json.WriteFieldValueXAsset("image", T10H_ASSET_IMAGE, asset->image);
			json.WriteFieldValueXAssetArray("image1", T10H_ASSET_IMAGE, ARRAYSIZE(asset->image1), asset->image1);
			json.WriteFieldValueXAssetArray("image2", T10H_ASSET_IMAGE, ARRAYSIZE(asset->image2), asset->image2);
			json.WriteFieldValueXAssetArray("image3", T10H_ASSET_IMAGE, ARRAYSIZE(asset->image3), asset->image3);
			json.WriteFieldValueXAssetArray("image4", T10H_ASSET_IMAGE, ARRAYSIZE(asset->image4), asset->image4);
			json.WriteFieldValueXAssetArray("image5", T10H_ASSET_IMAGE, ARRAYSIZE(asset->image5), asset->image5);
			json.WriteFieldValueXAsset("particleSystem1", T10H_ASSET_PARTICLESYSTEM, asset->particleSystem1);
			json.WriteFieldValueXAsset("particleSystem2", T10H_ASSET_PARTICLESYSTEM, asset->particleSystem2);
			json.WriteFieldValueXAsset("materialAnimation", T10H_ASSET_MATERIALANIMATION, asset->materialAnimation);

			
			if (opt.testDump) {
				json.WriteFieldValueUnknown("unk144", asset->unk144);
				json.WriteFieldValueUnknown("unk148", asset->unk148);
				json.WriteFieldValueUnknown("unk150", asset->unk150);
				json.WriteFieldValueUnknown("unk158", asset->unk158);
				json.WriteFieldValueUnknown("unk160", asset->unk160);
				json.WriteFieldValueUnknown("unk168", asset->unk168);
				json.WriteFieldValueUnknown("unk170", asset->unk170);
				json.WriteFieldValueUnknown("unk178", asset->unk178);
				json.WriteFieldValueUnknown("unk180", asset->unk180);
				json.WriteFieldValueUnknown("unk188", asset->unk188);
				json.WriteFieldValueUnknown("unk190", asset->unk190);
				json.WriteFieldValueUnknown("unk198", asset->unk198);
				json.WriteFieldValueUnknown("unk1a0", asset->unk1a0);
				json.WriteFieldValueUnknown("unk1a8", asset->unk1a8);
				json.WriteFieldValueUnknown("unk1b0", asset->unk1b0);
				json.WriteFieldValueUnknown("unk1b8", asset->unk1b8);
				json.WriteFieldValueUnknown("unk1c0", asset->unk1c0);
				json.WriteFieldValueUnknown("unk1c8", asset->unk1c8);
				json.WriteFieldValueUnknown("unk1d0", asset->unk1d0);
				json.WriteFieldValueUnknown("unk1d8", asset->unk1d8);
				json.WriteFieldValueUnknown("unk1e0", asset->unk1e0);
				json.WriteFieldValueUnknown("unk1e8", asset->unk1e8);
				json.WriteFieldValueUnknown("unk1f0", asset->unk1f0);
				json.WriteFieldValueUnknown("unk1f8", asset->unk1f8);
				json.WriteFieldValueUnknown("unk200", asset->unk200);
				json.WriteFieldValueUnknown("unk208", asset->unk208);
				json.WriteFieldValueUnknown("unk210", asset->unk210);
				json.WriteFieldValueUnknown("unk218", asset->unk218);
				json.WriteFieldValueUnknown("unk220", asset->unk220);
				json.WriteFieldValueUnknown("unk228", asset->unk228);
				json.WriteFieldValueUnknown("unk230", asset->unk230);
				json.WriteFieldValueUnknown("unk238", asset->unk238);
				json.WriteFieldValueUnknown("unk240", asset->unk240);
				json.WriteFieldValueUnknown("unk248", asset->unk248);
				json.WriteFieldValueUnknown("unk250", asset->unk250);
				json.WriteFieldValueUnknown("unk258", asset->unk258);
				json.WriteFieldValueUnknown("unk260", asset->unk260);
				json.WriteFieldValueUnknown("unk268", asset->unk268);
				json.WriteFieldValueUnknown("unk270", asset->unk270);
				json.WriteFieldValueUnknown("unk278", asset->unk278);
				json.WriteFieldValueUnknown("unk280", asset->unk280);
				json.WriteFieldValueUnknown("unk288", asset->unk288);
				json.WriteFieldValueUnknown("unk290", asset->unk290);
				json.WriteFieldValueUnknown("unk298", asset->unk298);
				json.WriteFieldValueUnknown("unk2a0", asset->unk2a0);
				json.WriteFieldValueUnknown("unk2a8", asset->unk2a8);
				json.WriteFieldValueUnknown("unk2b0", asset->unk2b0);
				json.WriteFieldValueUnknown("unk2b8", asset->unk2b8);
				json.WriteFieldValueUnknown("unk2c0", asset->unk2c0);
				json.WriteFieldValueUnknown("unk2c8", asset->unk2c8);
				json.WriteFieldValueUnknown("unk2d0", asset->unk2d0);
				json.WriteFieldValueUnknown("unk2d8", asset->unk2d8);
				json.WriteFieldValueUnknown("unk2e0", asset->unk2e0);
				json.WriteFieldValueUnknown("unk2e8", asset->unk2e8);
				json.WriteFieldValueUnknown("unk2f0", asset->unk2f0);
				json.WriteFieldValueUnknown("unk2f8", asset->unk2f8);
				json.WriteFieldValueUnknown("unk300", asset->unk300);
				json.WriteFieldValueUnknown("unk308", asset->unk308);
				json.WriteFieldValueUnknown("unk310", asset->unk310);
				json.WriteFieldValueUnknown("unk318", asset->unk318);
				json.WriteFieldValueUnknown("unk320", asset->unk320);
				json.WriteFieldValueUnknown("unk328", asset->unk328);
				json.WriteFieldValueUnknown("unk330", asset->unk330);
				json.WriteFieldValueUnknown("unk338", asset->unk338);
				json.WriteFieldValueUnknown("unk340", asset->unk340);
				json.WriteFieldValueUnknown("unk348", asset->unk348);
				json.WriteFieldValueUnknown("unk350", asset->unk350);
				json.WriteFieldValueUnknown("unk358", asset->unk358);
				json.WriteFieldValueUnknown("unk360", asset->unk360);
				json.WriteFieldValueUnknown("unk368", asset->unk368);
				json.WriteFieldValueUnknown("unk370", asset->unk370);
				json.WriteFieldValueUnknown("unk378", asset->unk378);
				json.WriteFieldValueUnknown("unk380", asset->unk380);
				json.WriteFieldValueUnknown("unk388", asset->unk388);
				json.WriteFieldValueUnknown("unk390", asset->unk390);
				json.WriteFieldValueUnknown("unk398", asset->unk398);
				json.WriteFieldValueUnknown("unk3a0", asset->unk3a0);
				json.WriteFieldValueUnknown("unk3a8", asset->unk3a8);
				json.WriteFieldValueUnknown("unk3b0", asset->unk3b0);
				json.WriteFieldValueUnknown("unk3b8", asset->unk3b8);
				json.WriteFieldValueUnknown("unk3c0", asset->unk3c0);
				json.WriteFieldValueUnknown("unk3c8", asset->unk3c8);
				json.WriteFieldValueUnknown("unk3d0", asset->unk3d0);
				json.WriteFieldValueUnknown("unk3d8", asset->unk3d8);
				json.WriteFieldValueUnknown("unk3e0", asset->unk3e0);
				json.WriteFieldValueUnknown("unk3e8", asset->unk3e8);
				json.WriteFieldValueUnknown("unk3f0", asset->unk3f0);
				json.WriteFieldValueUnknown("unk3f8", asset->unk3f8);
				json.WriteFieldValueUnknown("unk400", asset->unk400);
				json.WriteFieldValueUnknown("unk408", asset->unk408);
				json.WriteFieldValueUnknown("unk410", asset->unk410);
				json.WriteFieldValueUnknown("unk418", asset->unk418);
				json.WriteFieldValueUnknown("unk420", asset->unk420);
				json.WriteFieldValueUnknown("unk428", asset->unk428);
				json.WriteFieldValueUnknown("unk430", asset->unk430);
				json.WriteFieldValueUnknown("unk438", asset->unk438);
				json.WriteFieldValueUnknown("unk440", asset->unk440);
				json.WriteFieldValueUnknown("unk448", asset->unk448);
				json.WriteFieldValueUnknown("unk450", asset->unk450);
				json.WriteFieldValueUnknown("unk458", asset->unk458);
				json.WriteFieldValueUnknown("unk460", asset->unk460);
				json.WriteFieldValueUnknown("unk468", asset->unk468);
				json.WriteFieldValueUnknown("unk470", asset->unk470);
				json.WriteFieldValueUnknown("unk478", asset->unk478);
				json.WriteFieldValueUnknown("unk480", asset->unk480);
				json.WriteFieldValueUnknown("unk488", asset->unk488);
				json.WriteFieldValueUnknown("unk490", asset->unk490);
				json.WriteFieldValueUnknown("unk498", asset->unk498);
				json.WriteFieldValueUnknown("unk4a0", asset->unk4a0);
			}

			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "camo"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump camo {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_CAMO, sizeof(Camo) };
}