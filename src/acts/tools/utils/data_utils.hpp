#pragma once

namespace utils::data {

	void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size);

	std::string AsHex(const void* buff, size_t size);
}