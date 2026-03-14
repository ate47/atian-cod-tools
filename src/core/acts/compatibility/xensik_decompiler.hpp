#pragma once
#include <tools/gsc/gsc_opcodes_load.hpp>

namespace compatibility::xensik::decompiler {
#ifdef GPL_BUILD
	constexpr bool available = true;
#else
	constexpr bool available = false;
#endif

	struct ScriptFileInformation {
		int32_t compressedLen;
		int32_t len;
		int32_t bytecodeLen;
		void* buffer;
		void* bytecode;
		tool::gsc::opcode::VMId vm;
		tool::gsc::opcode::Platform plt{ tool::gsc::opcode::Platform::PLATFORM_PC };

		template<typename SF>
		void FillScriptFile(SF* sf) {
			compressedLen = sf->compressedLen;
			len = sf->len;
			bytecodeLen = sf->bytecodeLen;
			buffer = sf->buffer;
			bytecode = sf->bytecode;
		}
	};

	bool DecompileScript(ScriptFileInformation* info, std::filesystem::path fsPath);

}