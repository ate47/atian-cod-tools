#include <dll_includes.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <core/system.hpp>
#include <data/refs.hpp>

namespace {
	const char* aesKeys =
		"mp_common,LKBcjAtLFtrhGQqXZP3GQN2MXbGe4yBA4CJ8KK+Tmyw=\n"
		"cp_common,1Xms8bivDnvtle9GlNy3IHsDBYi5q6kSJTqMJUZbUBo=\n"
		"zm_common,uVkxOTxN2vKCJHt2piY5tGqy33LKZ0dKlKizutZifuI=\n"
		"mp_common_script,7e6KAmqxCK3nN3W71rjZHpdFaqASeBJrKJX3MLn6TgA=\n"
		"cp_common_script,qegcHT9IT+OjPeB1hD8KdQ1Ly+mjbkc165y2oyJrKjE=\n"
		"zm_common_script,GUPGtrNkdKDbhv9BseRFiBWs75PqywvqD7a3qORZMz4=\n"
		"wz_common,qYOw3RHpf/4LoNqha7D8w0l1uJs1a8f1GXvz9RSlcpc=\n"
		"wz_common_script,gxDQi5HReQ+jVeuM2VidGfYPG/tIhsCCH5zeFi+Ef40=\n"
		"4k_mp_common,8is3WNns3wUz+/iP+gLqOE7PvUrEjAlC654uSCVqRfc=\n"
		"4k_cp_common,Nc4VDeLbPX/uTVFt8dmtNzgCxrimQN8ZZDnuhIw4T4E=\n"
		"4k_zm_common,B23/jgGIk/ZNmQkSC98gITpeNorBkLdVF68+KPSpEcI=\n"
		"4k_wz_common,ad2/QtgA/M6tMUs0gTGlFKHAKBjqr/aD4C7TwQfVAD0=\n"
		"1080_mp_common,HBc65qAKDuV7IOqiHGK6MKhnqgQqEuGrXE40o/PEo5s=\n"
		"1080_cp_common,1vbqZhxvF62GBvY06n6RY1YKNC3rWws8EpH/yWs7yDU=\n"
		"1080_zm_common,TJCTFHZUzkcB5x0pBhWTIfxFmnTaPR1Clr0pSMSCEsw=\n"
		"1080_wz_common,iIV65SIKNvl24hjoe3u8geuPXaJ8rR18dy7GuxK916A="
		;

	int64_t sub_180001070()
	{
		int64_t v0{ *(int64_t*)0xCF753C_a };
		if (!v0
			|| (v0 - 0x10000) > 0x7FFFFFFEFFFF
			|| IsBadReadPtr(*(void**)(0xCF753C_a), 1ui64)) {
			return 0i64;
		}
		return v0;
	}

	void PostInit(uint64_t uid) {

	}

	REGISTER_SYSTEM(patch, nullptr, PostInit);
}

EXPORT void PATCH_ForceConnect() {
	hook::process::WriteMemSafe<uint32_t>(0xA8609C8_a, 1);

	cod2020::LobbyUI_ForceUIScreen(10);
	cod2020::CBuf_AddText(0, "disconnect");
	cod2020::LiveStorage_ParseKeysTxtv();
	cod2020::LiveStorage_ParseKeysTxtc(aesKeys);
}

EXPORT void PATCH_LoadZombies() {
	hook::process::WriteMemSafe<uint32_t>(0xB7D18A0_a, 0);
	hook::process::WriteMemSafe<uint32_t>(0x65259E4_a, 0x40C0);
	hook::process::WriteMemSafe<uint32_t>(0xB3333A9_a, 0x616C637A);

	cod2020::CBuf_AddText(0, "disconnect");
}