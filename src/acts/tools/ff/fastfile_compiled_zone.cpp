#include <includes.hpp>
#include <tools/ff/fastfile_compiled_zone.hpp>

namespace fastfile::compiled_zone {

	void CompiledZone::DumpDebug(const std::filesystem::path& out) {
		std::filesystem::create_directories(out.parent_path());
		utils::OutFileCE os{ out, true };

		os << "// compiled zone test\n\n";

		// what if we note all the links to realign them back?
		// to add new assets/strings, we need to add something in the header, which will
		// unalign and shift all the virtual data
		//
		// write the locations obtained using DB_ConvertOffsetToPointer / DB_ConvertOffsetToAlias / DB_GetOffsetData
		// but DB_GetOffsetData can create issues? We don't have the pointer location.
	}

}