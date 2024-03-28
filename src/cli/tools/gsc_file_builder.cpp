#include "gsc_file_builder.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"

tool::gscfile::GscFile::GscFile(uint64_t name, VM vm) : name(name), vm(vm) {
}

void tool::gscfile::GscFile::Compile(std::vector<byte>& data) {
	using namespace tool::gsc;

	T8GSCOBJ header{};



	*reinterpret_cast<uint64_t*>(&header.magic[0]) = 0x0A0D43534780;
	header.magic[7] = (byte)vm;
	header.crc = 0;
	header.name = name;
	header.script_size = sizeof(header);
	
	data.clear();
	// allocate a basic header
	data.insert(data.begin(), reinterpret_cast<byte*>(&header), reinterpret_cast<byte*>(&(&header)[1]));


	if (includes.size()) {
		header.include_offset = (int32_t)data.size();
		header.include_count = (uint16_t)includes.size();
		for (const auto inc : includes) {
			utils::WriteValue(data, inc);
		}
	}

	header.script_size = sizeof(header);
}
