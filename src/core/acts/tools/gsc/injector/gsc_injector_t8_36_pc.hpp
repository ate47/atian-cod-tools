#pragma once
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>
#include <tools/gsc/gsc_injector.hpp>

namespace tool::gsc::injector {
	class GscInjectorT836PC : public GscInjector {
	public:
		GscInjectorT836PC()
			: GscInjector(tool::gsc::opcode::VMI_T8_36, tool::gsc::opcode::PLATFORM_PC) {}


		void InjectGsc(std::vector<byte>& gsc, InjectorConfig& cfg) override {

		}
	};

}