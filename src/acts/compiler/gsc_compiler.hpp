#pragma once
#include <tools/gsc.hpp>
#include "preprocessor.hpp"

namespace acts::compiler {
	enum DetourType {
		DETOUR_UNKNOWN = 0,
		DETOUR_GSIC,
		DETOUR_ACTS
	};

	struct CompilerConfig {
		bool clientScript{};
		const char* name{};
		const char* fileName{};
		tool::gsc::opcode::Platform platform{ tool::gsc::opcode::PLATFORM_PC };
		tool::gsc::opcode::VMId vm{ tool::gsc::opcode::VMI_UNKNOWN };
		DetourType detourType{ DETOUR_UNKNOWN };
		int32_t checksum{};
		bool computeDevOption{};
		bool obfuscate{};
		bool defineAsConstExpr{};
		bool noDevCallInline{};
		preprocessor::PreProcessorOption processorOpt{};
		std::string* preprocOutput{};
		std::unordered_set<std::string>* hashes{};

		tool::gsc::opcode::VmInfo* GetVm() {
			if (!nfo || nfo->vmMagic != vm) {
				if (!tool::gsc::opcode::IsValidVmMagic(vm, nfo)) throw std::runtime_error("Can't find VM");
			}
			return nfo;
		}
	private:
		tool::gsc::opcode::VmInfo* nfo{};
	};

	void CompileGsc(const std::vector<std::filesystem::path>& files, std::vector<byte>& data, CompilerConfig& cfg);
	inline void CompileGsc(const std::filesystem::path& file, std::vector<byte>& data, CompilerConfig& cfg) {
		std::vector<std::filesystem::path> files{ 1 };
		files.emplace_back(file);
		CompileGsc(files, data, cfg);
	}


}