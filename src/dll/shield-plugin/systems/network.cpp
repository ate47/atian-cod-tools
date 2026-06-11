#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <magic_enum/magic_enum.hpp>
#include <systems/network.hpp>

namespace systems::network {
	void SendActsNetworkCommand(bo4::client_t* client, ActsNetworkCommand cmd, const char* fmt, ...) {
		va_list va;

		va_start(va, fmt);
		char* msg{ utils::vap(fmt, va) };
		va_end(va);
		if (*msg) {
			msg = utils::va("%c %s", cmd, msg);
		}
		else {
			msg = utils::va("%c", cmd);
		}

		bo4::SV_SendServerCommand(client, bo4::SV_CMD_RELIABLE, "%c %s", bo4::SVCCMD_ACTS_CUSTOM_CMD, msg);
	}

	namespace {
		hook::library::Detour CG_DeployServerCommand_Detour;

		void CG_DeployServerCommand_Stub(bo4::LocalClientNum_t localClientNum) {
			const char* param{ bo4::Sys_GetParamSafe(0) };
			bo4::SVSCMD_e cmd{ (bo4::SVSCMD_e)*param };
			if (cmd == bo4::SVCCMD_ACTS_CUSTOM_CMD) {
				const char* cmdType{ bo4::Sys_GetParamSafe(1) };
				// TODO: handle msg?

				return;
			}
			CG_DeployServerCommand_Detour.Call(localClientNum);
		}

		void HashPostInit(uint64_t uid) {
			CG_DeployServerCommand_Detour.Create(0xCC5920_a, CG_DeployServerCommand_Stub);
		}

		REGISTER_SYSTEM(network, nullptr, HashPostInit);
	}
}
