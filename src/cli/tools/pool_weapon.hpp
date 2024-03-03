#pragma once
#include <pool.hpp>

namespace tool::pool {

	enum eModes : INT32 {
		MODE_ZOMBIES = 0x0,
		MODE_MULTIPLAYER = 0x1,
		MODE_CAMPAIGN = 0x2,
		MODE_WARZONE = 0x3,
		MODE_COUNT = 0x4,
		MODE_INVALID = 0x4,
		MODE_FIRST = 0x0,
	};
	typedef float vec2_t[2];
	enum weapInventoryType_t : uint32_t
	{
		WEAPINVENTORY_PRIMARY = 0x0,
		WEAPINVENTORY_OFFHAND = 0x1,
		WEAPINVENTORY_ITEM = 0x2,
		WEAPINVENTORY_ALTMODE = 0x3,
		WEAPINVENTORY_MELEE = 0x4,
		WEAPINVENTORY_DWLEFTHAND = 0x5,
		WEAPINVENTORY_ABILITY = 0x6,
		WEAPINVENTORY_HEAVY = 0x7,
		WEAPINVENTORYCOUNT = 0x8,
	};
	enum gadgetType_e : uint32_t
	{
		GADGET_TYPE_NONE = 0x0,
		GADGET_TYPE_OTHER = 0x1,
		GADGET_TYPE_OPTIC_CAMO = 0x2,
		GADGET_TYPE_ARMOR_REGEN = 0x3,
		GADGET_TYPE_ARMOR = 0x4,
		GADGET_TYPE_DRONE = 0x5,
		GADGET_TYPE_VISION_PULSE = 0x6,
		GADGET_TYPE_MULTI_ROCKET = 0x7,
		GADGET_TYPE_TURRET_DEPLOY = 0x8,
		GADGET_TYPE_GRENADE = 0x9,
		GADGET_TYPE_JUKE = 0xA,
		GADGET_TYPE_HACKER = 0xB,
		GADGET_TYPE_INFRARED = 0xC,
		GADGET_TYPE_SPEED_BURST = 0xD,
		GADGET_TYPE_HERO_WEAPON = 0xE,
		GADGET_TYPE_COMBAT_EFFICIENCY = 0xF,
		GADGET_TYPE_FLASHBACK = 0x10,
		GADGET_TYPE_CLEANSE = 0x11,
		GADGET_TYPE_SYSTEM_OVERLOAD = 0x12,
		GADGET_TYPE_SERVO_SHORTOUT = 0x13,
		GADGET_TYPE_EXO_BREAKDOWN = 0x14,
		GADGET_TYPE_SURGE = 0x15,
		GADGET_TYPE_RAVAGE_CORE = 0x16,
		GADGET_TYPE_REMOTE_HIJACK = 0x17,
		GADGET_TYPE_IFF_OVERRIDE = 0x18,
		GADGET_TYPE_CACOPHONY = 0x19,
		GADGET_TYPE_FORCED_MALFUNCTION = 0x1A,
		GADGET_TYPE_CONCUSSIVE_WAVE = 0x1B,
		GADGET_TYPE_OVERDRIVE = 0x1C,
		GADGET_TYPE_UNSTOPPABLE_FORCE = 0x1D,
		GADGET_TYPE_RAPID_STRIKE = 0x1E,
		GADGET_TYPE_ACTIVE_CAMO = 0x1F,
		GADGET_TYPE_SENSORY_OVERLOAD = 0x20,
		GADGET_TYPE_ES_STRIKE = 0x21,
		GADGET_TYPE_IMMOLATION = 0x22,
		GADGET_TYPE_FIREFLY_SWARM = 0x23,
		GADGET_TYPE_SMOKESCREEN = 0x24,
		GADGET_TYPE_MISDIRECTION = 0x25,
		GADGET_TYPE_MRPUKEY = 0x26,
		GADGET_TYPE_SHOCK_FIELD = 0x27,
		GADGET_TYPE_RESURRECT = 0x28,
		GADGET_TYPE_HEAT_WAVE = 0x29,
		GADGET_TYPE_CLONE = 0x2A,
		GADGET_TYPE_ROULETTE = 0x2B,
		GADGET_TYPE_THIEF = 0x2C,
		GADGET_TYPE_DISRUPTOR = 0x2D,
		GADGET_TYPE_HORNET_SWARM = 0x2E,
		GADGET_TYPE_GROUP_REVIVE = 0x2F,
		GADGET_TYPE_XRAY_EYES = 0x30,
		GADGET_TYPE_EMERGENCY_MELEE = 0x31,
		GADGET_TYPE_SHOULDER_GUN = 0x32,
		GADGET_TYPE_GRAPPLE = 0x33,
		GADGET_TYPE_INVULNERABLE = 0x34,
		GADGET_TYPE_SPRINT_BOOST = 0x35,
		GADGET_TYPE_HEALTH_REGEN = 0x36,
		GADGET_TYPE_SELF_DESTRUCT = 0x37,
		GADGET_TYPE_BARRIER_BUILDER = 0x38,
		GADGET_TYPE_SPAWN_BEACON = 0x39,
		GADGET_TYPE_COUNT = 0x3A,
	};
	enum guidedMissileType_t : uint32_t
	{
		MISSILE_GUIDANCE_NONE = 0x0,
		MISSILE_GUIDANCE_SIDEWINDER = 0x1,
		MISSILE_GUIDANCE_HELLFIRE = 0x2,
		MISSILE_GUIDANCE_JAVELIN = 0x3,
		MISSILE_GUIDANCE_BALLISTIC = 0x4,
		MISSILE_GUIDANCE_WIREGUIDED = 0x5,
		MISSILE_GUIDANCE_TVGUIDED = 0x6,
		MISSILE_GUIDANCE_DRONE = 0x7,
		MISSILE_GUIDANCE_HEATSEEKING = 0x8,
		MISSILE_GUIDANCE_ROBOTECH = 0x9,
		MISSILE_GUIDANCE_DYNAMICIMPACTPOINT = 0xA,
		MISSILE_GUIDANCE_COUNT = 0xB,
	};

	enum weapType_t : uint32_t
	{
		WEAPTYPE_BULLET = 0x0,
		WEAPTYPE_GRENADE = 0x1,
		WEAPTYPE_PROJECTILE = 0x2,
		WEAPTYPE_BINOCULARS = 0x3,
		WEAPTYPE_GAS = 0x4,
		WEAPTYPE_BOMB = 0x5,
		WEAPTYPE_MINE = 0x6,
		WEAPTYPE_MELEE = 0x7,
		WEAPTYPE_RIOTSHIELD = 0x8,
		WEAPTYPE_SCRIPT = 0x9,
		WEAPTYPE_NUM = 0xa,
	};

	enum lockOnType_t : uint32_t
	{
		LOCK_ON_TYPE_NONE = 0x0,
		LOCK_ON_TYPE_LEGACY_SINGLE = 0x1,
		LOCK_ON_TYPE_AP_SINGLE = 0x2,
		LOCK_ON_TYPE_AP_MULTI = 0x3,
		LOCK_ON_TYPE_COUNT = 0x4,
	};

	enum weapClass_t : uint32_t {
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_MG = 0x1,
		WEAPCLASS_SMG = 0x2,
		WEAPCLASS_SPREAD = 0x3,
		WEAPCLASS_PISTOL = 0x4,
		WEAPCLASS_GRENADE = 0x5,
		WEAPCLASS_ROCKETLAUNCHER = 0x6,
		WEAPCLASS_TURRET = 0x7,
		WEAPCLASS_NON_PLAYER = 0x8,
		WEAPCLASS_GAS = 0x9,
		WEAPCLASS_ITEM = 0xA,
		WEAPCLASS_MELEE = 0xB,
		WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
		WEAPCLASS_PISTOL_SPREAD = 0xD,
		WEAPCLASS_BALL = 0xE,
		WEAPCLASS_NUM = 0xF,
	};
	enum projExplosionType_t : uint32_t {
		PROJEXP_NONE = 0,
		PROJEXP_GRENADE = 1,
		PROJEXP_ROCKET = 2,
		PROJEXP_FLASHBANG = 3,
		PROJEXP_DUD = 4,
		PROJEXP_SMOKE = 5,
		PROJEXP_HEAVY_EXPLOSIVE = 6,
		PROJEXP_FIRE = 7,
		PROJEXP_NAPALM_BLOB = 8,
		PROJEXP_BOLD = 9,
		PROJEXP_SHRAPNEL_SPAN = 0xa,
		PROJEXP_NUM = 0xb,
	};

	enum OffhandClass : uint32_t
	{
		OFFHAND_CLASS_NONE = 0x0,
		OFFHAND_CLASS_FRAG_GRENADE = 0x1,
		OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
		OFFHAND_CLASS_FLASH_GRENADE = 0x3,
		OFFHAND_CLASS_GEAR = 0x4,
		OFFHAND_CLASS_SUPPLYDROP_MARKER = 0x5,
		OFFHAND_CLASS_GADGET = 0x6,
		OFFHAND_CLASS_COUNT = 0x7,
	};
	enum OffhandSlot : uint32_t
	{
		OFFHAND_SLOT_NONE = 0x0,
		OFFHAND_SLOT_LETHAL_GRENADE = 0x1,
		OFFHAND_SLOT_TACTICAL_GRENADE = 0x2,
		OFFHAND_SLOT_EQUIPMENT = 0x3,
		OFFHAND_SLOT_SPECIFIC_USE = 0x4,
		OFFHAND_SLOT_GADGET = 0x5,
		OFFHAND_SLOT_SPECIAL = 0x6,
		OFFHAND_SLOT_HERO_WEAPON = 0x7,
		OFFHAND_SLOT_TAUNT = 0x8,
		OFFHAND_SLOT_SCRIPTED = 0x9,
		OFFHAND_SLOT_COUNT = 0xa,
	};

	const char* WeapInventoryTypeName(weapInventoryType_t t);
	const char* WeapTypeName(weapType_t t);
	const char* GadgetTypeName(gadgetType_e t);
    const char* GuidedMissileTypeName(guidedMissileType_t t);
    const char* OffhandClassName(OffhandClass t);
    const char* OffhandSlotName(OffhandSlot t);
    const char* LockOnTypeName(lockOnType_t t);
    const char* WeapClassName(weapClass_t t);
    const char* ProjExplosionTypeName(projExplosionType_t t);

	
#pragma once

    // void __fastcall sub_6C12E0(scriptInstance_t inst, WeaponDef * weap, scr_weapon_field_t * field)

    typedef uint32_t bitflag;
    typedef uint64_t bitflag64;
    typedef uint32_t ScrString_t;
    struct WeaponDefObj21329beb
    {
        uintptr_t model; // XModel*
        uint64_t unk8;
        uint64_t unk10;
        uint64_t unk18;
        uint64_t unk20;
        uint64_t unk28;
        uint64_t unk30;
        uint64_t unk38;
        uint64_t unk40;
    };

    struct WeaponDef
    {
        XHash name;
        XHash baseWeapon;
        XHash displayname;
        XHash unk30;
        XHash unk40;
        XHash unk50;
        XHash unk60;
        XHash unk70;
        XHash unk80;
        XHash sound90;
        XHash sounda0;
        XHash soundb0;
        XHash soundc0;
        XHash firesounddistant; // 0xd0
        XHash firesound;        // 0xe0
        XHash firesoundplayer;  // 0xf0
        XHash sound100;
        XHash sound110;
        XHash sound120;
        XHash sound130;
        XHash sound140;
        XHash sound150;
        XHash sound160;
        XHash sound170;
        XHash sound180;
        XHash sound190;
        XHash sound1a0;
        XHash sound1b0;
        XHash sound1c0;
        XHash sound1d0;
        XHash sound1e0;
        XHash sound1f0;
        XHash sound200;
        XHash sound210;
        XHash sound220;
        XHash sound230;
        XHash sound240;
        XHash sound250;
        XHash sound260;
        XHash sound270;
        XHash sound280;
        XHash sound290;
        XHash sound2a0;
        XHash sound2b0;
        XHash sound2c0;
        XHash sound2d0;
        XHash sound2e0;
        XHash sound2f0;
        XHash sound300;
        XHash sound310;
        XHash sound320;
        XHash sound330;
        XHash sound340;
        XHash sound350;
        XHash sound360;
        XHash sound370;
        XHash sound380;
        XHash gadgetreadysound; // 0x390
        XHash var_1f7ccc3b;     // 0x3a0
        XHash sound3b0;
        XHash sound3c0;
        XHash sound3d0;
        XHash sound3e0;
        XHash sound3f0;
        XHash sound400;
        XHash sound410;
        XHash gadgetreadysoundplayer; // 0x420
        XHash var_fb22040b;           // 0x430
        XHash sound440;
        XHash sound450;
        XHash sound460;
        XHash sound470;
        XHash sound480;
        XHash sound490;
        XHash sound4a0;
        XHash sound4b0;
        XHash sound4c0;
        XHash sound4d0;
        XHash sound4e0;
        XHash sound4f0;
        XHash sound500;
        XHash sound510;
        XHash sound520;
        XHash sound530;
        XHash sound540;
        XHash sound550;
        XHash sound560;
        XHash sound570;
        XHash sound580;
        XHash sound590;
        XHash sound5a0;
        XHash sound5b0;
        XHash sound5c0;
        XHash sound5d0;
        XHash sound5e0;
        XHash sound5f0;
        XHash sound600;
        XHash sound610;
        XHash sound620;
        XHash sound630;
        XHash sound640;
        XHash sound650;
        XHash sound660;
        XHash sound670;
        XHash sound680;
        XHash sound690;
        XHash sound6a0;
        XHash sound6b0;
        XHash sound6c0;
        XHash lockonseekersearchsound;  // 0x6d0
        XHash lockonseekerlockedsound;  // 0x6e0
        XHash lockontargetlockedsound;  // 0x6f0
        XHash lockontargetfiredonsound; // 0x700
        XHash sound710;
        XHash sound720;
        XHash sound730;
        XHash sound740;
        XHash var_8a03df2b; // 0x750
        XHash var_2f3ca476; // 0x760
        XHash var_5c29f743; // 0x770
        XHash projexplosionsound; // 0x780
        XHash projexplosionsoundplayer; // 0x790
        XHash projsmokestartsound; // 0x7a0
        XHash projsmokeloopsound;  // 0x7b0
        XHash projsmokeendsound;   // 0x7c0
        XHash sound7d0;
        XHash sound7e0;
        XHash sound7f0;
        XHash sound800;
        XHash sound810;
        XHash sound820;
        XHash sound830;
        XHash sound840;
        XHash sound850;
        XHash sound860;
        XHash hitsound; // 0x870
        XHash unk880; // weapon name
        uint64_t unk890;
        uint64_t unk898;
        uint32_t unk8a0;
        uint32_t unk8a4;
        uint32_t unk8a8;
        uint32_t unk8ac;
        uintptr_t unk8b0; // const char*
        uintptr_t unk8b8;
        uintptr_t unk8c0;
        uintptr_t unk8c8;
        uintptr_t unk8d0;
        uintptr_t unk8d8;
        uint32_t unk8e0;
        uint32_t unk8e4;
        uintptr_t unk8e8;
        uintptr_t unk8f0;
        uintptr_t unk8f8;
        uint32_t unk900;
        uint32_t unk904;
        uint64_t unk908;
        uint64_t unk910;
        uint64_t unk918;
        uint64_t unk920;
        uint64_t unk928;
        uint64_t unk930;
        vec2_t* accuracyGraphKnots[2];
        uint64_t unk948;
        uintptr_t unk950;
        uintptr_t unk958;
        uint64_t unk960;
        uintptr_t unk968;
        int32_t var_6566504b; // 0x970
        uint32_t unk974;
        uintptr_t unk978;
        uint64_t unk980;
        uint64_t unk988;
        uint64_t unk990;
        uint64_t unk998;
        uint64_t unk9a0;
        uint64_t unk9a8;
        uint64_t unk9b0;
        uint64_t unk9b8;
        uint64_t unk9c0;
        uint64_t unk9c8;
        uint64_t unk9d0;
        uint64_t unk9d8;
        uint64_t unk9e0;
        uint64_t unk9e8;
        uint64_t unk9f0;
        uint64_t unk9f8;
        uint64_t unka00;
        uint64_t unka08;
        uint64_t unka10;
        uint64_t unka18;
        uintptr_t unka20;
        uintptr_t unka28;
        uintptr_t unka30;
        uint64_t unka38;
        uintptr_t attachments; // WeaponAttachmentPtr*
        uintptr_t attachmentUniques; // WeaponAttachmentUniquePtr*
        uint64_t unka50;
        uintptr_t properties; // WeaponDefProperties*
        uint64_t unka60;
        uint64_t unka68;
        uintptr_t viewmodel; // XModel*
        uintptr_t frontendmodel; // XModel*
        uint64_t unka80;
        uint64_t unka88;
        uint64_t unka90;
        uint64_t unka98;
        uint64_t unkaa0;
        uint64_t unkaa8;
        uint64_t unkab0;
        uint64_t unkab8;
        uint64_t unkac0;
        uint64_t unkac8;
        uint64_t unkad0;
        uint64_t unkad8;
        uint64_t unkae0;
        uint64_t unkae8;
        uint64_t unkaf0;
        uint64_t unkaf8;
        uint64_t unkb00;
        uint64_t unkb08;
        uint64_t unkb10;
        uint64_t unkb18;
        uint64_t unkb20;
        uint64_t unkb28;
        uint64_t unkb30;
        uint64_t unkb38;
        uint64_t unkb40;
        uint64_t unkb48;
        uint64_t unkb50;
        uint64_t unkb58;
        uint64_t unkb60;
        uint64_t unkb68;
        uint64_t unkb70;
        uint64_t unkb78;
        uint64_t unkb80;
        uint64_t unkb88;
        uint64_t unkb90;
        uint64_t unkb98;
        uint64_t unkba0;
        uint64_t unkba8;
        uint64_t unkbb0;
        uint64_t unkbb8;
        uint64_t unkbc0;
        uint64_t unkbc8;
        uint64_t unkbd0;
        uint64_t unkbd8;
        uint64_t unkbe0;
        uint64_t unkbe8;
        uint64_t unkbf0;
        uint64_t unkbf8;
        uint64_t unkc00;
        uint64_t unkc08;
        uintptr_t unkc10; // SharedWeaponSoundsPtr
        uint64_t unkc18;
        uint64_t unkc20;
        uint64_t unkc28;
        uint64_t unkc30;
        uint64_t unkc38;
        uint64_t unkc40;
        uint64_t unkc48;
        uint64_t unkc50;
        uint64_t unkc58;
        uintptr_t gadgetIconAvailable; // GfxImageHandle
        uintptr_t gadgetIconUnavailable; // GfxImageHandle
        uintptr_t unkc70; // GfxImageHandle
        uintptr_t unkc78; // GfxImageHandle
        uintptr_t unkc80; // fx*
        uintptr_t unkc88; // fx*
        uintptr_t unkc90; // fx*
        uintptr_t unkc98; // fx*
        uint64_t unkca0; // empty
        uint64_t unkca8; // ??
        uint64_t unkcb0; // wtf?
        uint64_t unkcb8; // ??
        uint64_t unkcc0; // ??
        uintptr_t worldModel; // XModel**
        uint64_t unkcd0; // XModel**
        uintptr_t stowedmodel; // XModel*
        uintptr_t clipmodel; // XModel**
        uint64_t unkce8;
        uint64_t unkcf0;
        uint64_t unkcf8;
        uint64_t unkd00;
        uint64_t unkd08;
        uint64_t unkd10;
        uintptr_t unkd18; // ??
        uintptr_t unkd20; // ??
        uintptr_t unkd28; // XModel*
        uintptr_t var_22082a57; // XModel*
        uint64_t unkd38;
        uint64_t unkd40;
        uint64_t unkd48;
        uint64_t unkd50;
        uint64_t unkd58;
        uint64_t unkd60;
        uint64_t unkd68;
        uint64_t unkd70;
        uint64_t unkd78;
        uint64_t unkd80;
        uint32_t unkd88;
        uint32_t unkd8c;
        uint64_t unkd90;
        uint64_t unkd98;
        uint64_t unkda0;
        uint64_t unkda8;
        uint64_t unkdb0;
        uint64_t unkdb8;
        uint64_t unkdc0;
        uint64_t unkdc8;
        uint64_t unkdd0;
        uint64_t unkdd8;
        uint64_t unkde0;
        uint64_t unkde8;
        uint64_t unkdf0;
        uint64_t unkdf8;
        uint64_t unke00;
        uint64_t unke08;
        uint64_t unke10;
        uint64_t unke18;
        uint64_t unke20;
        uint64_t unke28;
        uint64_t unke30;
        uint64_t unke38;
        uint64_t unke40;
        uint64_t unke48;
        uint64_t unke50;
        uint64_t unke58;
        uint64_t unke60;
        uint64_t unke68;
        uint64_t unke70;
        uint64_t unke78;
        uint64_t unke80;
        uint64_t unke88;
        uint64_t unke90;
        uint64_t unke98;
        uint64_t unkea0;
        uint64_t unkea8;
        uint64_t unkeb0;
        uintptr_t weaponHeadObjective; //ObjectivePtr 
        uint64_t unkec0;
        uintptr_t projectilemodel; // XModel*
        uint64_t unked0;
        uint64_t unked8;
        uint64_t unkee0;
        uint64_t unkee8;
        uint64_t unkef0;
        uintptr_t var_4bcd08b0; // XModel*
        uint64_t unkf00;
        uint64_t unkf08;
        uint64_t unkf10;
        uint64_t unkf18;
        uint64_t unkf20;
        uint64_t unkf28;
        uint64_t unkf30;
        uint64_t unkf38;
        uint64_t unkf40;
        uint64_t unkf48;
        uint64_t unkf50;
        uint64_t unkf58;
        uint64_t unkf60;
        uint64_t unkf68;
        uint64_t unkf70;
        uintptr_t var_96850284; // XHash*
        uintptr_t var_26f68e75; // XHash*
        uint64_t unkf88;
        uint64_t unkf90;
        uint64_t unkf98;
        uint64_t unkfa0;
        uint64_t unkfa8;
        uint64_t unkfb0;
        uint64_t unkfb8;
        uint64_t unkfc0;
        uint64_t unkfc8;
        uint64_t unkfd0;
        uint64_t unkfd8;
        uint64_t unkfe0;
        uint64_t unkfe8;
        uint64_t unkff0;
        uint64_t unkff8;
        uint64_t unk1000;
        uint64_t unk1008;
        uint64_t unk1010;
        uint64_t unk1018;
        uintptr_t  customsettings;   // ScriptBundle*
        uintptr_t  shrapnelsettings; // ScriptBundle*
        uintptr_t  var_2e4a8800; // ScriptBundle*
        uint64_t unk1038;
        uint64_t unk1040;
        uint64_t unk1048;
        uintptr_t var_8456d4d; // ScriptBundle*
        uint64_t unk1058;
        uint64_t unk1060;
        uint64_t unk1068;
        uint64_t unk1070;
        uint64_t unk1078;
        uint64_t unk1080;
        uint64_t unk1088;
        uint64_t unk1090;
        uint64_t unk1098;
        uint64_t unk10a0;
        uint64_t unk10a8;
        uint64_t unk10b0;
        uint64_t unk10b8;
        uint64_t unk10c0;
        uint64_t unk10c8;
        uint64_t unk10d0;
        uint64_t unk10d8;
        uint64_t unk10e0;
        uint64_t unk10e8;
        uint64_t unk10f0;
        uint64_t unk10f8;
        uint64_t unk1100;
        uint64_t unk1108;
        uint64_t unk1110;
        uint64_t unk1118;
        uint64_t unk1120;
        uint64_t unk1128;
        uint64_t unk1130;
        uint64_t unk1138;
        uint64_t unk1140;
        uint64_t unk1148;
        uint64_t unk1150;
        uint64_t unk1158;
        uint64_t unk1160;
        uint64_t unk1168;
        uint64_t unk1170;
        uint64_t unk1178;
        uint64_t unk1180;
        uint64_t unk1188;
        uint64_t unk1190;
        uint64_t unk1198;
        uint64_t unk11a0;
        uint64_t unk11a8;
        uint64_t unk11b0;
        uint64_t unk11b8;
        uint64_t unk11c0;
        uint64_t unk11c8;
        uint64_t unk11d0;
        uint64_t unk11d8;
        uint64_t unk11e0;
        uint64_t unk11e8;
        uint64_t unk11f0;
        uint64_t unk11f8;
        uint64_t unk1200;
        uint64_t unk1208;
        uint64_t unk1210;
        uint64_t unk1218;
        uint64_t unk1220;
        uint64_t unk1228;
        uint64_t unk1230;
        uint64_t unk1238;
        uint64_t unk1240;
        uint64_t unk1248;
        uint64_t unk1250;
        uint64_t unk1258;
        uint64_t unk1260;
        uint64_t unk1268;
        uint64_t unk1270;
        uint64_t unk1278;
        uint64_t unk1280;
        uint64_t unk1288;
        uint64_t unk1290;
        uint64_t unk1298;
        uint64_t unk12a0;
        uint64_t unk12a8;
        uint64_t unk12b0;
        uint64_t unk12b8;
        uint64_t unk12c0;
        uint64_t unk12c8;
        uint64_t unk12d0;
        uint64_t unk12d8;
        uint64_t unk12e0;
        uint64_t unk12e8;
        uint64_t unk12f0;
        uint64_t unk12f8;
        uint32_t unk1300;
        float weaponheadobjectiveheight; // 0x1304
        uint64_t unk1308;
        uint64_t unk1310;
        uint64_t unk1318;
        uint64_t unk1320;
        uint64_t unk1328;
        uint64_t unk1330;
        uint64_t unk1338;
        eModes sessionMode;
        uint32_t unk1344;
        uint64_t unk1348;
        int32_t var_5b73038c; // 0x1350
        uint32_t unk1354;
        int itemIndex;
        uint32_t unk135c;
        uint64_t unk1360;
        uint32_t unk1368;
        int32_t weaponstarthitpoints; // 0x136c
        uint64_t unk1370;
        int32_t soundrattlerangemin; // 0x1378
        int32_t soundrattlerangemax; // 0x137c
        uint32_t unk1380;
        int accuracyGraphKnotCount[2];
        uint64_t unk1390;
        uint64_t unk1398;
        uint64_t unk13a0;
        uint64_t unk13a8;
        uint32_t unk13b0;
        uint32_t grenadeweapon;
        uint32_t unk13b8;
        uint32_t dualwieldweapon;
        uint32_t unk13c0;
        uint32_t dualWieldWeaponIndex;
        uint32_t altWeaponIndex;
        uint32_t unk13cc;
        uint32_t unk13d0;
        byte var_50d2316b; // 0x13d4
        byte var_83b1dc1;  // 0x13d5
        uint16_t unk13d6;
        byte unk13d8;
        byte drawoffhandmodelinhand; // 0x13d9
        uint16_t unk13da;
        byte lockonseekersearchsoundloops;  // 0x13dc
        byte lockonseekerlockedsoundloops;  // 0x13dd
        byte lockontargetlockedsoundloops;  // 0x13de
        byte lockontargetfiredonsoundloops; // 0x13df
        uint32_t unk13e0;
        byte var_965cc0b3; // 0x13e4
        byte unk13e5;
        uint16_t unk13e6;
        uint16_t unk13e8;
        byte forcedamageshellshockandrumble; // 0x13ea
        byte unk13eb;
        uint32_t unk13ec;
        uint32_t unk13f0;
        byte var_2cf49821; // 0x13f4
        byte var_d2751f9d; // 0x13f5
        byte var_554be9f7; // 0x13f6
        byte unk13f7;
        uint64_t unk13f8;
        uint64_t unk1400;
        uint64_t unk1408;
        uint64_t unk1410;
        uint64_t unk1418;
        uint64_t unk1420;
        uint64_t unk1428;
        WeaponDefObj21329beb var_21329beb[4];
    };
    struct WeaponDefProperties
    {
        uint64_t unk0;
        uint64_t unk8;
        uint64_t unk10;
        uint64_t unk18;
        uintptr_t var_f56ac2bd;    // const char*
        uintptr_t spawninfluencer; // const char*
        XHash var_77b46a8c;          // 0x30
        uint64_t unk40;
        uint64_t unk48;
        uint64_t unk50;
        uint64_t unk58;
        uint64_t unk60;
        uint64_t unk68;
        uint64_t unk70;
        uint64_t unk78;
        uint64_t unk80;
        uint64_t unk88;
        uint64_t unk90;
        uint64_t unk98;
        uint64_t unka0;
        uint64_t unka8;
        uint64_t unkb0;
        uint64_t unkb8;
        uint64_t unkc0;
        uint64_t unkc8;
        uint64_t unkd0;
        uint64_t unkd8;
        uint64_t unke0;
        uint64_t unke8;
        uint64_t unkf0;
        uint64_t unkf8;
        uint64_t unk100;
        uint64_t unk108;
        uint64_t unk110;
        uint64_t unk118;
        uint64_t unk120;
        uint64_t unk128;
        uint64_t unk130;
        uint64_t unk138;
        uint64_t unk140;
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
        ScrString_t hackertriggerorigintag; // 0x208
        uint32_t unk20c;
        uint64_t unk210;
        uint64_t unk218;
        uint32_t unk220;
        uint32_t unk224;
        uint64_t unk228;
        int32_t explosioninnerdamage; // 0x230
        uint32_t unk234;
        uint64_t unk238;
        uint64_t unk240;
        int32_t explosionouterdamage; // 0x248
        uint32_t unk24c;
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
        uint32_t unk2a8;
        float gadget_shockfield_radius;    // 0x2ac
        float gadget_shockfield_damage;    // 0x2b0
        int32_t gadget_flickerondamage;    // 0x2b4
        int32_t gadget_flickeronpowerloss; // 0x2b8
        int32_t var_e4109b63;              // 0x2bc
        uint32_t unk2c0;
        int32_t gadget_max_hitpoints; // 0x2c4
        uint64_t unk2c8;
        uint64_t unk2d0;
        uint64_t unk2d8;
        uint32_t unk2e0;
        int32_t var_c4d4d2a9; // 0x2e4
        uint32_t unk2e8;
        float gadget_powerofflossondamage; // 0x2ec
        uint64_t unk2f0;
        uint32_t unk2f8;
        int32_t gadget_powergainonretrieve; // 0x2fc
        float gadget_powergainscorefactor;  // 0x300
        float var_c15c6b39;                 // 0x304
        int32_t var_6a864cad;               // 0x308
        uint32_t unk30c;
        uint64_t unk310;
        int32_t gadget_powermoveloss;     // 0x318
        float gadget_powermovespeed;      // 0x31c
        float gadget_poweronlossondamage; // 0x320
        uint32_t unk324;
        byte var_58543a1c; // 0x328
        byte unk329;
        uint16_t unk32a;
        int32_t var_68a0f062; // 0x32c
        float var_abb570e0;   // 0x330
        int32_t var_829de2ac; // 0x334
        uint32_t unk338;
        float var_e4d4fa7e;                            // 0x33c
        float gadget_powerreplenishfactor;             // 0x340
        int32_t gadget_power_round_end_active_penalty; // 0x344
        int32_t gadget_powersprintloss;                // 0x348
        int32_t var_d911d477;                          // 0x34c
        uint64_t unk350;
        uint64_t unk358;
        float gadget_power_usage_rate; // 0x360
        float var_e1811962;            // 0x364
        float var_66103577;            // 0x368
        float var_f9eec1ec;            // 0x36c
        int32_t var_6821283d;          // 0x370
        int32_t var_81683931;          // 0x374
        int32_t var_1e89f40;           // 0x378
        int32_t gadget_pulse_duration; // 0x37c
        uint32_t unk380;
        int32_t var_4d88a1ff; // 0x384
        int32_t var_9d776ba6; // 0x388
        int32_t var_b9951041; // 0x38c
        int32_t var_8e0b0827; // 0x390
        float var_a2d7b97c;   // 0x394
        float var_5be370e9;   // 0x398
        int32_t var_42a3cafc; // 0x39c
        uint32_t unk3a0;
        int32_t gadget_pulse_max_range; // 0x3a4
        uint64_t unk3a8;
        uint64_t unk3b0;
        int32_t var_44c79c09; // 0x3b8
        uint32_t unk3bc;
        gadgetType_e gadget_type; // 0x3c0
        uint32_t unk3c4;
        uint64_t unk3c8;
        uint64_t unk3d0;
        uint32_t unk3d8;
        uint16_t unk3dc;
        byte var_2cb95b88; // 0x3de
        byte unk3df;
        uint64_t unk3e0;
        byte gadget_turnoff_onempjammed; // 0x3e8
        byte unk3e9;
        uint16_t unk3ea;
        uint32_t unk3ec;
        byte gadget_heroversion_2_0; // 0x3f0
        byte var_256488f1;           // 0x3f1
        uint16_t unk3f2;
        byte unk3f4;
        byte gadget_powergainscoreignoreself;       // 0x3f5
        byte var_f23e9d19;                          // 0x3f6
        byte gadget_powergainscoreignorewhenactive; // 0x3f7
        uint16_t unk3f8;
        byte unk3fa;
        byte var_ddaa57f2;                       // 0x3fb
        byte gadget_power_reset_on_class_change; // 0x3fc
        byte unk3fd;
        byte gadget_power_reset_on_spawn;        // 0x3fe
        byte gadget_power_reset_on_team_change;  // 0x3ff
        byte gadget_power_reset_on_round_switch; // 0x400
        byte gadget_power_consume_on_ammo_use;   // 0x401
        byte var_5801b768;                       // 0x402
        byte unk403;
        byte var_bec5136b; // 0x404
        byte unk405;
        byte unk406;
        byte var_28bb357c; // 0x407
        byte unk408;
        byte var_f10d73e1; // 0x409
        byte var_7b5016a7; // 0x40a
        byte unk40b;
        byte unk40c;
        byte var_ce34bb7e; // 0x40d
        uint16_t unk40e;
        guidedMissileType_t guidedMissileType; // 0x410
        uint32_t unk414;
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
        uint32_t unk4b0;
        OffhandClass offhandClass;
        OffhandSlot offhandSlot;
        uint32_t unk4bc;
        uint64_t unk4c0;
        uint64_t unk4c8;
        uint64_t unk4d0;
        uint64_t unk4d8;
        uint64_t unk4e0;
        uint64_t unk4e8;
        uint64_t unk4f0;
        uint64_t unk4f8;
        uint64_t unk500;
        uint64_t unk508;
        uint64_t unk510;
        uint64_t unk518;
        uint64_t unk520;
        uint64_t unk528;
        uint64_t unk530;
        uint64_t unk538;
        uint64_t unk540;
        uint64_t unk548;
        uint64_t unk550;
        uint64_t unk558;
        uint64_t unk560;
        uint32_t unk568;
        uint32_t unk56c;
        float var_27c94b15; // 0x570
        float var_16e90b80; // 0x574
        float var_6cb9946f; // 0x578
        float var_40ffe7d2; // 0x57c
        weapClass_t weapClass; // 0x580
        uint32_t unk584;
        uint32_t unk588;
        weapInventoryType_t inventoryType;
        uint32_t unk590;
        lockOnType_t lockontype;
        uint64_t unk598;
        uint32_t unk5a0;
        uint32_t unk5a4;
        uint32_t unk5a8;
        projExplosionType_t projExplosionType;
        uint64_t unk5b0;
        uint64_t unk5b8;
        weapType_t weapType; // 0x5c0
        uint32_t unk5c4;
        uint64_t unk5c8;
        uint64_t unk5d0;
        uint64_t unk5d8;
        uint64_t unk5e0;
        uint64_t unk5e8;
        uint64_t unk5f0;
        uint64_t unk5f8;
        uint64_t unk600;
        uint64_t unk608;
        uint64_t unk610;
        float aimeleerange; // 0x618
        uint32_t unk61c;
        uint64_t unk620;
        uint64_t unk628;
        uint64_t unk630;
        uint64_t unk638;
        uint64_t unk640;
        uint32_t unk648;
        float var_6844746b; // 0x64c
        float var_e2b40cd5; // 0x650
        uint32_t unk654;
        uint64_t unk658;
        uint64_t unk660;
        uint64_t unk668;
        uint64_t unk670;
        uint64_t unk678;
        uint64_t unk680;
        uint64_t unk688;
        uint64_t unk690;
        float damagetoownerscalar; // 0x698
        uint32_t unk69c;
        uint64_t unk6a0;
        uint64_t unk6a8;
        float var_c94f007c; // 0x6b0
        float var_3a00e7eb; // 0x6b4
        float var_2cf65b96; // 0x6b8
        uint32_t unk6bc;
        uint64_t unk6c0;
        uint64_t unk6c8;
        uint64_t unk6d0;
        uint64_t unk6d8;
        uint64_t unk6e0;
        uint64_t unk6e8;
        uint64_t unk6f0;
        uint64_t unk6f8;
        uint64_t unk700;
        uint64_t unk708;
        uint64_t unk710;
        uint64_t unk718;
        uint64_t unk720;
        uint64_t unk728;
        uint32_t unk730;
        float adsspread; // 0x734
        uint64_t unk738;
        uint64_t unk740;
        uint64_t unk748;
        uint64_t unk750;
        uint64_t unk758;
        uint64_t unk760;
        uint64_t unk768;
        uint64_t unk770;
        uint64_t unk778;
        uint64_t unk780;
        uint64_t unk788;
        uint64_t unk790;
        uint64_t unk798;
        uint64_t unk7a0;
        uint64_t unk7a8;
        uint64_t unk7b0;
        uint64_t unk7b8;
        uint64_t unk7c0;
        uint64_t unk7c8;
        uint64_t unk7d0;
        uint64_t unk7d8;
        uint64_t unk7e0;
        uint64_t unk7e8;
        uint64_t unk7f0;
        uint64_t unk7f8;
        uint64_t unk800;
        uint64_t unk808;
        uint64_t unk810;
        uint64_t unk818;
        uint64_t unk820;
        uint64_t unk828;
        uint64_t unk830;
        uint32_t unk838;
        float fightdist; // 0x83c
        uint64_t unk840;
        uint64_t unk848;
        uint32_t unk850;
        float var_1f13c7f1; // 0x854
        uint64_t unk858;
        uint64_t unk860;
        int32_t var_5af80bb6; // 0x868
        uint32_t unk86c;
        uint64_t unk870;
        uint64_t unk878;
        uint64_t unk880;
        uint64_t unk888;
        uint64_t unk890;
        uint64_t unk898;
        uint64_t unk8a0;
        uint64_t unk8a8;
        uint64_t unk8b0;
        uint64_t unk8b8;
        float var_36c76157; // 0x8c0
        uint32_t unk8c4;
        uint64_t unk8c8;
        uint64_t unk8d0;
        uint64_t unk8d8;
        uint64_t unk8e0;
        uint64_t unk8e8;
        uint64_t unk8f0;
        uint32_t unk8f8;
        float var_19f920eb; // 0x8fc
        uint64_t unk900;
        uint64_t unk908;
        uint64_t unk910;
        uint64_t unk918;
        uint64_t unk920;
        uint64_t unk928;
        uint64_t unk930;
        uint64_t unk938;
        uint64_t unk940;
        uint32_t unk948;
        float maxdist;        // 0x94c
        float maxgibdistance; // 0x950
        uint32_t unk954;
        uint64_t unk958;
        uint64_t unk960;
        uint64_t unk968;
        uint32_t unk970;
        float meleelungerange; // 0x974
        uint64_t unk978;
        float var_9aa1ef19; // 0x980
        uint32_t unk984;
        float var_cd539cb2; // 0x988
        float var_cb3d0f65; // 0x98c
        float var_7872b3a;  // 0x990
        float var_367c47fc; // 0x994
        float var_ccebc40;  // 0x998
        uint32_t unk99c;
        uint64_t unk9a0;
        uint64_t unk9a8;
        uint32_t unk9b0;
        float var_deb0b2fe; // 0x9b4
        uint64_t unk9b8;
        uint64_t unk9c0;
        uint64_t unk9c8;
        uint64_t unk9d0;
        uint64_t unk9d8;
        uint64_t unk9e0;
        uint64_t unk9e8;
        uint64_t unk9f0;
        float lifetime; // 0x9f8
        uint32_t unk9fc;
        uint64_t unka00;
        uint64_t unka08;
        uint64_t unka10;
        uint64_t unka18;
        uint64_t unka20;
        uint64_t unka28;
        uint64_t unka30;
        uint64_t unka38;
        uint64_t unka40;
        uint64_t unka48;
        uint64_t unka50;
        uint64_t unka58;
        uint64_t unka60;
        uint64_t unka68;
        uint64_t unka70;
        uint64_t unka78;
        uint32_t unka80;
        float sprintboostduration; // 0xa84
        float sprintboostradius;   // 0xa88
        uint32_t unka8c;
        uint64_t unka90;
        uint64_t unka98;
        uint64_t unkaa0;
        uint64_t unkaa8;
        uint64_t unkab0;
        uint64_t unkab8;
        uint64_t unkac0;
        uint32_t unkac8;
        float lockonlossanglehorizontal; // 0xacc
        float lockonlossanglevertical;   // 0xad0
        float lockonanglehorizontal;     // 0xad4
        float lockonanglevertical;       // 0xad8
        uint32_t unkadc;
        uint64_t unkae0;
        float vehicleprojectiledamagescalar;       // 0xae8
        float vehicleprojectilesplashdamagescalar; // 0xaec
        uint64_t unkaf0;
        uint64_t unkaf8;
        uint64_t unkb00;
        uint64_t unkb08;
        uint32_t unkb10;
        float var_416021d8; // 0xb14
        uint64_t unkb18;
        uint64_t unkb20;
        uint64_t unkb28;
        uint64_t unkb30;
        uint64_t unkb38;
        uint64_t unkb40;
        uint64_t unkb48;
        uint64_t unkb50;
        uint64_t unkb58;
        uint32_t unkb60;
        int32_t aiFuseTime; // 0xb64
        uint64_t unkb68;
        uint64_t unkb70;
        uint64_t unkb78;
        uint32_t unkb80;
        int32_t burndamage;         // 0xb84
        int32_t burndamageinterval; // 0xb88
        int32_t burnduration;       // 0xb8c
        int32_t burstcount;         // 0xb90
        uint32_t unkb94;
        uint64_t unkb98;
        uint64_t unkba0;
        uint64_t unkba8;
        uint64_t unkbb0;
        uint64_t unkbb8;
        int32_t var_98333ae;  // 0xbc0
        int32_t var_1c0e3cb7; // 0xbc4
        int32_t var_4941de5;  // 0xbc8
        uint32_t unkbcc;
        uint64_t unkbd0;
        uint64_t unkbd8;
        uint64_t unkbe0;
        uint64_t unkbe8;
        uint64_t unkbf0;
        uint64_t unkbf8;
        uint32_t unkc00;
        int32_t var_f7e67f28; // 0xc04
        int32_t var_227c90e1; // 0xc08
        int32_t var_ab300840; // 0xc0c
        int32_t var_849af6b4; // 0xc10
        int32_t var_c4aae0fa; // 0xc14
        int32_t var_5ac2e7a4; // 0xc18
        int32_t var_76127e14; // 0xc1c
        uint64_t unkc20;
        uint64_t unkc28;
        uint64_t unkc30;
        uint64_t unkc38;
        uint64_t unkc40;
        uint64_t unkc48;
        uint64_t unkc50;
        uint64_t unkc58;
        uint64_t unkc60;
        uint64_t unkc68;
        uint32_t unkc70;
        int32_t var_df381b5d; // 0xc74
        int32_t var_a8bd8bb2; // 0xc78
        int32_t var_8134b209; // 0xc7c
        int32_t heal;         // 0xc80
        int32_t var_4465ef1e; // 0xc84
        uint64_t unkc88;
        uint64_t unkc90;
        uint64_t unkc98;
        uint64_t unkca0;
        uint32_t unkca8;
        int32_t ammoindex; // 0xcac
        uint64_t unkcb0;
        uint64_t unkcb8;
        uint32_t iClipSize;
        int32_t var_ec2cbce2; // 0xcc4
        int32_t var_df0f9ce9; // 0xcc8
        uint32_t unkccc;
        uint64_t unkcd0;
        uint64_t unkcd8;
        uint64_t unkce0;
        uint64_t unkce8;
        uint64_t unkcf0;
        uint64_t unkcf8;
        uint64_t unkd00;
        uint64_t unkd08;
        uint64_t unkd10;
        uint64_t unkd18;
        uint32_t unkd20;
        int32_t var_1123a989; // 0xd24
        uint64_t unkd28;
        uint64_t unkd30;
        uint64_t unkd38;
        uint64_t unkd40;
        uint32_t unkd48;
        uint32_t iMaxAmmo;
        uint32_t meleedamage;
        uint32_t unkd54;
        uint64_t unkd58;
        uint32_t unkd60;
        uint32_t unkd64;
        uint32_t unkd68;
        uint32_t meleetime;
        uint64_t unkd70;
        uint64_t unkd78;
        uint64_t unkd80;
        uint64_t unkd88;
        uint64_t unkd90;
        uint64_t unkd98;
        uint64_t unkda0;
        uint64_t unkda8;
        uint64_t unkdb0;
        uint64_t unkdb8;
        uint64_t unkdc0;
        uint64_t unkdc8;
        uint64_t unkdd0;
        uint64_t unkdd8;
        uint64_t unkde0;
        uint64_t unkde8;
        uint64_t unkdf0;
        uint64_t unkdf8;
        uint64_t unke00;
        uint64_t unke08;
        uint64_t unke10;
        uint64_t unke18;
        uint64_t unke20;
        uint64_t unke28;
        uint64_t unke30;
        uint64_t unke38;
        uint32_t iStartAmmo; // 0xe40
        int32_t fuellife; // 0xe44
        uint64_t unke48;
        int32_t lockonlossradius; // 0xe50
        uint32_t unke54;
        uint64_t unke58;
        int32_t lockonminrange; // 0xe60
        int32_t lockonradius;   // 0xe64
        uint32_t unke68;
        int32_t lockonscreenradius; // 0xe6c
        uint64_t unke70;
        uint64_t unke78;
        uint64_t unke80;
        uint64_t unke88;
        int32_t maxheal;             // 0xe90
        int32_t maxinstancesallowed; // 0xe94
        uint64_t unke98;
        uint64_t unkea0;
        uint64_t unkea8;
        uint64_t unkeb0;
        uint64_t unkeb8;
        uint64_t unkec0;
        uint64_t unkec8;
        uint32_t unked0;
        uint32_t meleechargetime; // ed4
        uint64_t unked8;
        uint64_t unkee0;
        uint64_t unkee8;
        uint64_t unkef0;
        uint64_t unkef8;
        uint64_t unkf00;
        uint64_t unkf08;
        uint64_t unkf10;
        uint32_t meleepowertime; // 0xf18
        uint32_t unkf1c;
        uint64_t unkf20;
        uint32_t meleepowertimeleft; // 0xf28
        uint32_t unkf2c;
        uint64_t unkf30;
        uint64_t unkf38;
        uint32_t unkf40;
        int32_t var_cddb5cd0; // 0xf44
        uint32_t unkf48;
        int32_t var_e5db3b95;    // 0xf4c
        int32_t var_61d29b2f;    // 0xf50
        int32_t multidetonation; // 0xf54
        int32_t var_c264efc6;    // 0xf58
        int32_t var_95d8fabf;    // 0xf5c
        uint64_t unkf60;
        uint64_t unkf68;
        uint64_t unkf70;
        uint64_t unkf78;
        uint64_t unkf80;
        uint64_t unkf88;
        int32_t playerdamage; // 0xf90
        uint32_t unkf94;
        uint64_t unkf98;
        uint64_t unkfa0;
        uint64_t unkfa8;
        uint64_t unkfb0;
        uint64_t unkfb8;
        int32_t var_ac36c1db; // 0xfc0
        uint32_t unkfc4;
        uint64_t unkfc8;
        uint64_t unkfd0;
        uint64_t unkfd8;
        uint64_t unkfe0;
        uint64_t unkfe8;
        uint64_t unkff0;
        uint64_t unkff8;
        uint64_t unk1000;
        int32_t var_60563796; // 0x1008
        int32_t var_7a93ed37; // 0x100c
        uint64_t unk1010;
        uint64_t unk1018;
        uint64_t unk1020;
        uint64_t unk1028;
        uint64_t unk1030;
        uint64_t unk1038;
        uint64_t unk1040;
        uint64_t unk1048;
        uint64_t unk1050;
        uint64_t unk1058;
        uint64_t unk1060;
        uint64_t unk1068;
        uint64_t unk1070;
        uint64_t unk1078;
        uint64_t unk1080;
        uint64_t unk1088;
        uint64_t unk1090;
        uint64_t unk1098;
        uint64_t unk10a0;
        uint64_t unk10a8;
        uint64_t unk10b0;
        byte unk10b8;
        byte unk10b9;
        byte unk10ba;
        byte unk10bb;
        byte unk10bc;
        byte unk10bd;
        byte unk10be;
        byte ammoCountClipRelative; // 0x10bf
        byte unk10c0;
        byte var_8032088a; // 0x10c1
        byte ammoregen;    // 0x10c2
        byte unk10c3;
        uint32_t unk10c4;
        uint32_t unk10c8;
        uint16_t unk10cc;
        byte var_f6dea63a; // 0x10ce
        byte unk10cf;
        byte unk10d0;
        byte altoffhand; // 0x10d1
        uint16_t unk10d2;
        byte unk10d4;
        bool bAltWeaponDisableSwitching;
        byte unk10d6;
        byte unk10d7;
        byte unk10d8;
        byte anyplayercanretrieve; // 0x10d9
        byte unk10da;
        byte isboltaction; // 0x10db
        uint16_t unk10dc;
        byte unk10de;
        byte bulletimpactexplode; // 0x10df
        uint16_t unk10e0;
        byte canuseunderwater; // 0x10e2
        byte iscliponly;       // 0x10e3
        uint16_t unk10e4;
        byte var_337fc1cf;              // 0x10e6
        byte craftitem;                 // 0x10e7
        byte var_9fffdcee;              // 0x10e8
        byte decoy;                     // 0x10e9
        byte destroyablebytrophysystem; // 0x10ea
        bool bDieOnRespawn;
        byte unk10ec;
        byte disabledeploy; // 0x10ed
        uint16_t unk10ee;
        uint16_t unk10f0;
        byte unk10f2;
        byte disallowatmatchstart; // 0x10f3
        uint16_t unk10f4;
        byte dodamagefeedback; // 0x10f6
        byte doempdestroyfx;   // 0x10f7
        byte doesfiredamage;   // 0x10f8
        byte dohackedstats; // 0x10f9
        byte dostun;        // 0x10fa
        byte unk10fb;
        byte unk10fc;
        bool isdualwield;
        byte unk10fe;
        byte unk10ff;
        uint16_t unk1100;
        byte unk1102;
        byte var_251796e3; // 0x1103
        uint16_t unk1104;
        byte unk1106;
        byte ignoresflakjacket; // 0x1107
        byte ignoreslightarmor; // 0x1108
        byte ignorespowerarmor; // 0x1109
        byte ignoreteamkills;   // 0x110a
        byte unk110b;
        byte unk110c;
        byte isaikillstreakdamage; // 0x110d
        byte isballisticknife;     // 0x110e
        byte iscarriedkillstreak;  // 0x110f
        byte isemp;                // 0x1110
        byte isflash;              // 0x1111
        byte isflourishweapon;     // 0x1112
        byte isgameplayweapon;     // 0x1113
        byte ishacktoolweapon;     // 0x1114
        byte bIsHybridWeapon;      // 0x1115
        byte isperkbottle;         // 0x1116
        byte isscavengable;        // 0x1117
        byte issniperweapon;     // 0x1118
        byte isstun;             // 0x1119
        byte issupplydropweapon; // 0x111a
        byte unk111b;
        byte unk111c;
        byte var_130391b9; // 0x111d
        byte var_6f12adba; // 0x111e
        byte blocksprone;  // 0x111f
        uint16_t unk1120;
        byte meleeignoreslightarmor; // 0x1122
        byte unk1123;
        uint32_t unk1124;
        uint32_t unk1128;
        byte unk112c;
        byte nohitmarker; // 0x112d
        byte unk112e;
        byte nonstowedweapon; // 0x112f
        uint32_t unk1130;
        byte notkillstreak; // 0x1134
        byte unk1135;
        uint16_t unk1136;
        uint64_t unk1138;
        uint32_t unk1140;
        byte setusedstat; // 0x1144
        byte unk1145;
        byte shownenemyequip;  // 0x1146
        byte shownenemyexplo;  // 0x1147
        byte shownretrievable; // 0x1148
        byte unk1149;
        byte skipbattlechatterkill;   // 0x114a
        byte skipbattlechatterreload; // 0x114b
        byte skiplowammovox;          // 0x114c
        byte specialpain;             // 0x114d
        byte isthrowback;             // 0x114e
        byte unk114f;
        uint64_t unk1150;
        uint64_t unk1158;
        uint32_t unk1160;
        byte unk1164;
        byte var_9111ccc0; // 0x1165
        uint16_t unk1166;
        uint32_t unk1168;
        uint16_t unk116c;
        byte unk116e;
        byte damagealwayskillsplayer; // 0x116f
        byte deployable;              // 0x1170
        byte var_e0d42861;            // 0x1171
        byte var_3344c07e;            // 0x1172
        byte var_70b09d5b;            // 0x1173
        byte destroysequipment;       // 0x1174
        byte var_8025ffca;            // 0x1175
        uint16_t unk1176;
        byte var_f076a292; // 0x1178
        byte unk1179;
        uint16_t unk117a;
        uint16_t unk117c;
        byte unk117e;
        byte doannihilate; // 0x117f
        byte var_6262fd11; // 0x1180
        byte unk1181;
        byte doblowback;       // 0x1182
        byte dogibbing;        // 0x1183
        byte dogibbingonmelee; // 0x1184
        byte unk1185;
        byte donotdamageowner; // 0x1186
        byte unk1187;
        uint32_t unk1188;
        byte unk118c;
        byte forcedamagehitlocation; // 0x118d
        byte unk118e;
        byte var_33d50507; // 0x118f
        uint64_t unk1190;
        byte grappleweapon; // 0x1198
        byte unk1199;
        uint16_t unk119a;
        uint16_t unk119c;
        byte unk119e;
        byte var_fab9617b; // 0x119f
        byte var_ca947940; // 0x11a0
        byte unk11a1;
        uint16_t unk11a2;
        uint16_t unk11a4;
        byte unk11a6;
        byte isnotdroppable; // 0x11a7
        uint16_t unk11a8;
        byte issignatureweapon;   // 0x11aa
        byte istacticalinsertion; // 0x11ab
        byte var_76ce72e8;        // 0x11ac
        byte var_ff0b00ba;        // 0x11ad
        byte isvaluable;          // 0x11ae
        byte var_29d24e37;        // 0x11af
        uint64_t unk11b0;
        uint64_t unk11b8;
        uint16_t unk11c0;
        byte mountable;        // 0x11c2
        byte var_ba335ef;      // 0x11c3
        byte noadslockoncheck; // 0x11c4
        byte unk11c5;
        uint16_t unk11c6;
        uint64_t unk11c8;
        byte unk11d0;
        byte var_d69ee9ed; // 0x11d1
        uint16_t unk11d2;
        uint32_t unk11d4;
        uint64_t unk11d8;
        uint16_t unk11e0;
        byte unk11e2;
        byte requirelockontofire; // 0x11e3
        byte var_8072cf0b;        // 0x11e4
        byte unk11e5;
        uint16_t unk11e6;
        uint64_t unk11e8;
        uint32_t unk11f0;
        byte unk11f4;
        bool istimeddetonation;
        bool unlimitedAmmo; // 0x11f6
        byte unk11f7;
        uint64_t unk11f8;
        uint64_t unk1200;
        byte unk1208;
        byte var_18608bfe; // 0x1209
        byte var_775d2aad; // 0x120a
        byte var_b8a85edd; // 0x120b
        byte var_dbbd4cec; // 0x120c
    };


}