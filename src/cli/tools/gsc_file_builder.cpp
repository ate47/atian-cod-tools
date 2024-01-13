#include "gsc_file_builder.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"

tool::gscfile::GscFile::GscFile(UINT64 name, VM vm) : name(name), vm(vm) {
}

void tool::gscfile::GscFile::Compile(std::vector<BYTE>& data) {
	using namespace tool::gsc;

	T8GSCOBJ header{};



	*reinterpret_cast<UINT64*>(&header.magic[0]) = 0x0A0D43534780;
	header.magic[7] = (BYTE)vm;
	header.crc = 0;
	header.name = name;
	header.script_size = sizeof(header);
	
	data.clear();
	// allocate a basic header
	data.insert(data.begin(), reinterpret_cast<BYTE*>(&header), reinterpret_cast<BYTE*>(&(&header)[1]));


	if (includes.size()) {
		header.include_offset = (INT32)data.size();
		header.include_count = (UINT16)includes.size();
		for (const auto inc : includes) {
			utils::WriteValue(data, inc);
		}
	}

	header.script_size = sizeof(header);
}
