#pragma once
#include <hook/refs.hpp>

namespace cod2020 {
	using namespace hook::refs;

	S_ANY Ref<void(int con, const char* line)> CBuf_AddText{ 0x16F3A10_a };
	S_ANY Ref<void(int screen)> LobbyUI_ForceUIScreen{ 0x105D9C0_a };
	S_ANY Ref<void()> LiveStorage_ParseKeysTxtv{ 0x1011720_a };
	S_ANY Ref<void(const char* csv)> LiveStorage_ParseKeysTxtc{ 0x1012900_a };
}