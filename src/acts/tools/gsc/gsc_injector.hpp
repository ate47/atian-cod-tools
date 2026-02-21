#pragma once
#include <tools/gsc/gsc_opcodes_load.hpp>

namespace tool::gsc {
	namespace compiler {
		class CompileObject;
	}
	namespace injector {
		struct InjectorConfig {
			// used for ps4 injections
			const char* ps4{};
			const char* replacedScript{};
			const char* hookedScript{};

			tool::gsc::compiler::CompileObject* obj{};
			std::vector<byte>* gdb{};
		};

		class GscInjector {
		public:
			tool::gsc::opcode::VMId vm;
			tool::gsc::opcode::Platform platform;
			GscInjector(tool::gsc::opcode::VMId vm, tool::gsc::opcode::Platform platform) : vm(vm), platform(platform) {}

			virtual void InjectGsc(std::vector<byte>& gsc, InjectorConfig& config) = 0;
		};

		GscInjector* FindInjector(tool::gsc::opcode::VMId vm, tool::gsc::opcode::Platform platform);
	}
}