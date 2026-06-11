#pragma once
#include <data/bo4.hpp>

namespace systems::network {
	enum ActsNetworkCommand : char {
		SVCACMD_SET_DVAR = -96,
	};

	void SendActsNetworkCommand(bo4::client_t* client, ActsNetworkCommand cmd, const char* fmt, ...);
}