#include "../actslib.hpp"
#include "disk_sort.hpp"

namespace actslib::disk_sort {
	class BinaryFileWriter {
		std::ofstream stream;
	public:
		BinaryFileWriter(const std::filesystem::path& path) : stream(path, std::ios::binary) {}
		~BinaryFileWriter() { stream.close(); }


		void WriteVByte(uint64_t value) {
			char tmp[0x10];
			char* t{ tmp };
			while (value > 127) {
				*(t++) = (char)(value & 127);
				value >>= 7;
			}
			*(t++) = (char)(value | 0x80);

			stream.write(tmp, t - tmp);
		}

	};

}