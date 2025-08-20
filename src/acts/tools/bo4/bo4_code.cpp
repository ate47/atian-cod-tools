#include <includes.hpp>
#include <hook/module_mapper.hpp>

namespace {

	int bo4_bdiff_inc(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		hook::module_mapper::Module mod{ true };
		if (!mod.Load(argv[2], false)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}
		struct Instruction {
			__int16 op;
			unsigned __int16 mode;
			unsigned int size;
		};

		struct DoubleInstruction {
			Instruction instruction[2];
		};

		const DoubleInstruction* inc{ mod->Get<DoubleInstruction>(0xF990E80) };

		utils::OutFileCE os{ argv[3], true };

		os <<
			"static DoubleInstruction instructions[256] =\n"
			"{" << std::hex;

		static const char* vcdInstructionCodeNames[4]{ "VCDI_NOOP", "VCDI_ADD", "VCDI_RUN", "VCDI_COPY" };

		for (size_t i = 0; i < 256; i++) {
			const Instruction* di{ inc[i].instruction };
			if (i) os << " ,";
			os << "\n\t{ {";
			for (size_t i = 0; i < 2; i++) {
				Instruction inst{ di[i] };
				if (i) os << ",";
				os 
					<< " { "
				    << vcdInstructionCodeNames[inst.op]
					<< ", 0x" << (int)inst.mode
					<< ", 0x" << (int)inst.size
					<< " }"
					;
			}
			os << " } }";
		}

		os << "\n};";

		return tool::OK;
	}


	ADD_TOOL(bo4_bdiff_inc, "bo4", "[exe] [dump]", "dump bdiff inc", bo4_bdiff_inc);
}