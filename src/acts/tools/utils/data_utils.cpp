#include <includes.hpp>
#include "data_utils.hpp"

namespace utils::data {

	void WriteHex(std::ostream& out, uintptr_t base, void* _buff, size_t size) {
		byte* buff{ (byte*)_buff };
		for (size_t j = 0; j < size; j++) {
			if (j % 8 == 0) {
				if (base) {
					out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
				}
				out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
				if (j + 7 < size) {
					out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(&buff[j]);
				}
			}
			if (j - j % 8 + 7 >= size) {
				out << std::hex << std::setw(2) << std::setfill('0') << (int)buff[j];
			}
			if ((j + 1) % 8 == 0) {
				out << "|";

				for (size_t i = j - 7; i <= j; i++) {
					if (buff[i] >= ' ' && buff[i] <= '~') {
						out << (char)buff[i];
					}
					else {
						out << ".";
					}
				}

				// check x64 values
				if (j >= 7) {
					auto val = *reinterpret_cast<uint64_t*>(&buff[j - 7]);
					if (val) {
						// not null, hash?
						auto* h = hashutils::ExtractPtr(val);
						if (h) {
							out << " #" << h;
						}
					}
				}
				out << "\n";
			}
		}
		out << "\n";
	}
	std::string AsHex(void* buff, size_t size) {
		std::stringstream ss{};

		byte* ptr{ (byte*)buff };
		for (size_t i = 0; i < size; i++) {
			if ((i & 7) == 0 && i) ss << " ";
			ss << std::setw(2) << std::hex << std::setfill('0') << (int)ptr[i];
		}

		return ss.str();
	}

}