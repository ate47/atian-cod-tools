#pragma once

namespace fastfile {
	int ComputeChecksum32(const char* buffer, unsigned int len, int start = 0);
}