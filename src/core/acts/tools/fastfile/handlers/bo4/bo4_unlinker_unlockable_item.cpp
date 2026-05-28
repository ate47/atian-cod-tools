#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;


    static const char* itemGroupNames[] = {
        "weapon_smg",
        "weapon_assault",
        "weapon_tactical",
        "weapon_cqb",
        "weapon_lmg",
        "weapon_sniper",
        "weapon_pistol",
        "weapon_launcher",
        "weapon_knife",
        "weapon_special",
        "weapon_shotgun",
        "weapon_explosive",
        "weapon_grenade",
        "weapon_masterkey",
        "weapon_grenadelauncher",
        "weapon_flamethrower",
        "specialty",
        "talisman",
        "specialgrenade",
        "miscweapon",
        "feature",
        "bonuscard",
        "killstreak",
        "hero",
        "talent",
        "bubblegum",
        "bubblegum_consumable",
        "character",
    };

    static const char* attachmentsNames[] = {
        "none",
        "acog",
        "adsreload",
        "barrelchoke",
        "clantag",
        "custom1",
        "custom2",
        "custom3",
        "custom4",
        "custom5",
        "damage",
        "damage2",
        "dualoptic",
        "dw",
        "dynzoom",
        "elo",
        "extbarrel",
        "extbarrel2",
        "extclip",
        "extclip2",
        "extrapellets",
        "fastlockon",
        "fastreload",
        "fastreload2",
        "fmj",
        "fmj2",
        "gl",
        "grip",
        "grip2",
        "hipgrip",
        "holdbreath",
        "holo",
        "ir",
        "is",
        "killcounter",
        "mixclip",
        "mk",
        "mms",
        "null",
        "pistolscope",
        "quickdraw",
        "quickdraw2",
        "rangefinder",
        "reddot",
        "reflex",
        "rf",
        "sf",
        "speedreloader",
        "stackfire",
        "stalker",
        "stalker2",
        "steadyaim",
        "steadyaim2",
        "supply",
        "suppressed",
        "swayreduc",
        "tacknife",
        "uber",
        "vzoom"
    };

    static const char* loadoutSlotNames[]{
        "primary",
        "primaryattachment1",
        "primaryattachment2",
        "primaryattachment3",
        "primaryattachment4",
        "primaryattachment5",
        "primaryattachment6",
        "primaryattachment7",
        "primarycamo",
        "primaryreticle",
        "primarypaintjobslot",
        "primaryweaponmodelslot",
        "primarycharm",
        "primarydeathfx",
        "secondary",
        "secondaryattachment1",
        "secondaryattachment2",
        "secondaryattachment3",
        "secondaryattachment4",
        "secondaryattachment5",
        "secondaryattachment6",
        "secondaryattachment7",
        "secondarycamo",
        "secondaryreticle",
        "secondarypaintjobslot",
        "secondaryweaponmodelslot",
        "secondarycharm",
        "secondarydeathfx",
        "primarygrenade",
        "primarygrenadecount",
        "specialgrenade",
        "specialgrenadecount",
        "herogadget",
        "specialty1",
        "specialty2",
        "specialty3",
        "specialty4",
        "specialty5",
        "specialty6",
        "talisman1",
        "tacticalgear",
        "talent1",
        "talent2",
        "talent3",
        "talent4",
        "talent5",
        "talent6",
        "bonuscard1",
        "bonuscard2",
        "bonuscard3",
        "equippedBubbleGumPack",
        "killStreak1",
        "killStreak2",
        "killStreak3",
        "killStreak4",
        "firstSelectedCharacterIndex",
    };

    enum itemGroup_t : uint32_t {};
    enum loadoutSlot_t : uint32_t {};
    enum eAttachment : uint32_t {};

    struct UnlockableItem {
        const char* nameStr;
        XHash name;
        XHash displayName;
        XHash displayNameShort;
        XHash description;
        XHash wzDescription;
        XHash zmDescription;
        GfxImage* previewImage;
        GfxImage* previewImageLarge;
        GfxImage* wzPreviewImage;
        GfxImage* wzPreviewImageLarge;
        GfxImage* zmPreviewImage;
        GfxImage* zmPreviewImageLarge;
        uint32_t globalItemIndex;
        uint32_t itemIndex;
        int32_t scoreToUnlock;
        int32_t lowestScoreToUnlockAllowed;
        uint64_t unka8;
        uint32_t unkb0;
        int32_t allocation;
        int32_t modes;
        int32_t unkbc[4];
        bool isNullItem;
        bool isPassive;
        bool isValid;
        bool isLootItem;
        bool attributeItems;
        uint32_t unkd4;
        loadoutSlot_t loadoutSlotIndex;
        itemGroup_t itemGroupIndex;
        uint32_t nameHash;
        uint32_t refHash;
        uint32_t unke8;
        uint32_t unkec;
        uint32_t attachmentsCount;
        uint32_t unkf4;
        eAttachment* attachments;
        uint32_t specialtiesCount;
        ScrString_t* specialties;
        uint32_t unk0xB7DA65F4_count;
        ScrString_t* unk0xB7DA65F4;
        scriptbundle::SB_ObjectsArray bundle;
    };
    static_assert(sizeof(UnlockableItem) == 0x140);


    struct UnlockableItemTable {
        XHash name;
        uint32_t unk10;
        uint32_t count1;
        UnlockableItem** elements1;
        uint32_t unk20;
        uint32_t gadgets;
        uint32_t bubblegumpack;
        uint32_t speciality;
        uint32_t talisman;
        uint32_t talent;
        uint32_t bonuscard;
        uint32_t unk3c;
        uint32_t count2;
        uint32_t unk44;
        UnlockableItem** elements2;
    };
    static_assert(sizeof(UnlockableItemTable) == 0x50);

    class ImplWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            UnlockableItem& asset{ *(UnlockableItem*)ptr };


            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "unlockableitem"
                / std::format("{}.json", hashutils::ExtractTmp("file", asset.name.name)) };
            std::filesystem::create_directories(outFile.parent_path());

            BO4JsonWriter json{};

            LOG_OPT_INFO("Dump playeroutfit {}", outFile.string());


            json.BeginObject();

            json.WriteFieldValueXHash("name", asset.name);
            json.WriteFieldValueString("nameStr", asset.nameStr);
            json.WriteFieldValueXHash("displayName", asset.displayName);
            json.WriteFieldValueXHash("displayNameShort", asset.displayNameShort);
            json.WriteFieldValueXHash("description", asset.description);
            json.WriteFieldValueXHash("wzDescription", asset.wzDescription);
            json.WriteFieldValueXHash("zmDescription", asset.zmDescription);
            if (asset.nameHash) json.WriteFieldValueHash("nameHash", asset.nameHash);
            if (asset.refHash) json.WriteFieldValueHash("refHash", asset.refHash);
            json.WriteFieldValueXAsset("previewImage", XAssetType::ASSET_TYPE_IMAGE, asset.previewImage);
            json.WriteFieldValueXAsset("previewImageLarge", XAssetType::ASSET_TYPE_IMAGE, asset.previewImageLarge);
            json.WriteFieldValueXAsset("wzPreviewImage", XAssetType::ASSET_TYPE_IMAGE, asset.wzPreviewImage);
            json.WriteFieldValueXAsset("wzPreviewImageLarge", XAssetType::ASSET_TYPE_IMAGE, asset.wzPreviewImageLarge);
            json.WriteFieldValueXAsset("zmPreviewImage", XAssetType::ASSET_TYPE_IMAGE, asset.zmPreviewImage);
            json.WriteFieldValueXAsset("zmPreviewImageLarge", XAssetType::ASSET_TYPE_IMAGE, asset.zmPreviewImageLarge);
            json.WriteFieldValueNumber("globalItemIndex", asset.globalItemIndex);
            json.WriteFieldValueNumber("itemIndex", asset.itemIndex);
            json.WriteFieldValueNumber("scoreToUnlock", asset.scoreToUnlock);
            json.WriteFieldValueNumber("lowestScoreToUnlockAllowed", asset.lowestScoreToUnlockAllowed);
            json.WriteFieldValueNumber("allocation", asset.allocation);
            json.WriteFieldValueNumber("modes", asset.modes);
            json.WriteFieldValueString("loadoutSlotIndex", asset.loadoutSlotIndex >= ACTS_ARRAYSIZE(loadoutSlotNames) ? "<invalid>" : loadoutSlotNames[asset.loadoutSlotIndex]);
            json.WriteFieldValueBool("isNullItem", asset.isNullItem);
            json.WriteFieldValueBool("isPassive", asset.isPassive);
            json.WriteFieldValueBool("isValid", asset.isValid);
            json.WriteFieldValueBool("isLootItem", asset.isLootItem);
            json.WriteFieldValueBool("attributeItems", asset.attributeItems);
            if (asset.unke8) json.WriteFieldValueNumber("unke8", asset.unke8);
            if (asset.unkec) json.WriteFieldValueNumber("unkec", asset.unkec);

            for (size_t i = 0; i < eModes::MODE_COUNT; i++) {
                if (asset.unkbc[i]) {
                    json.WriteFieldValueNumber(utils::va("unkbc_%s", GetEModeName((eModes)i)), asset.unkbc[i]);
                }
            }


            json.WriteFieldValueUnknown("unka8", asset.unka8);
            json.WriteFieldValueUnknown("unkb0", asset.unkb0);
            json.WriteFieldValueUnknown("unkd4", asset.unkd4);
            json.WriteFieldValueUnknown("unkf4", asset.unkf4);

            json.WriteFieldValueString("itemGroupIndex", asset.itemGroupIndex >= ACTS_ARRAYSIZE(itemGroupNames) ? "<invalid>" : itemGroupNames[asset.itemGroupIndex]);

            if (asset.attachments) {
                json.WriteFieldNameString("attachments");
                json.BeginArray();
                for (size_t i = 0; i < asset.attachmentsCount; i++) {
                    json.WriteValueString(asset.attachments[i] >= ACTS_ARRAYSIZE(attachmentsNames) ? "<invalid>" : attachmentsNames[asset.attachments[i]]);
                }
                json.EndArray();
            }

            json.WriteFieldValueScrStringArray("specialties", asset.specialtiesCount, asset.specialties);
            json.WriteFieldValueScrStringArray(0xB7DA65F4, asset.unk0xB7DA65F4_count, asset.unk0xB7DA65F4);

            scriptbundle::WriteObject(json, "bundle", asset.bundle);

            json.EndObject();

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class ImplTableWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            UnlockableItemTable& asset{ *(UnlockableItemTable*)ptr };


            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "unlockableitem" / "table"
                / std::format("{}.json", hashutils::ExtractTmp("file", asset.name.name)) };
            std::filesystem::create_directories(outFile.parent_path());

            BO4JsonWriter json{};

            LOG_OPT_INFO("Dump playeroutfit {}", outFile.string());


            json.BeginObject();

            json.WriteFieldValueXHash("name", asset.name);
            json.WriteFieldValueNumber("gadgets", asset.gadgets);
            json.WriteFieldValueNumber("bubblegumpack", asset.bubblegumpack);
            json.WriteFieldValueNumber("speciality", asset.speciality);
            json.WriteFieldValueNumber("talisman", asset.talisman);
            json.WriteFieldValueNumber("talent", asset.talent);
            json.WriteFieldValueNumber("bonuscard", asset.bonuscard);
            
            json.WriteFieldValueNumber("unk10", asset.unk10);
            json.WriteFieldValueNumber("unk3c", asset.unk3c);
            json.WriteFieldValueUnknown("unk20", asset.unk20);
            json.WriteFieldValueUnknown("unk44", asset.unk44);

            json.WriteFieldValueXAssetArray("elements1", XAssetType::ASSET_TYPE_UNLOCKABLE_ITEM, asset.count1, asset.elements1);
            json.WriteFieldValueXAssetArray("elements2", XAssetType::ASSET_TYPE_UNLOCKABLE_ITEM, asset.count2, asset.elements2);
            json.EndObject();

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_UNLOCKABLE_ITEM };
	utils::MapAdder<ImplTableWorker, XAssetType, Worker> implt{ GetWorkers(), XAssetType::ASSET_TYPE_UNLOCKABLE_ITEM_TABLE };
}