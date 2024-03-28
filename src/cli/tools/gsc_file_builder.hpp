#pragma once
#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"

namespace tool::gscfile {
	using namespace tool::gsc;
	using namespace tool::gsc::opcode;

	class GscFunctionImport {
		uint32_t nsp;
		uint32_t name;
		byte flag;
		uint32_t rloc;
	};
	class GscFunctionString {
		std::vector<byte> string{};
		byte flag;
	};

	struct GscFunctionGlobal {
		uint32_t name;
		uint32_t rloc;
	};

	class GscFunction {
		uint32_t nsp;
		uint32_t callback_event;
		uint32_t name;
		byte flags;
		byte params;

		std::vector<byte> data{};
		std::vector<GscFunctionImport> imports{};
		std::vector<GscFunctionString> strings{};
		std::vector<GscFunctionGlobal> global{};
	};

	class GscFile {
		uint64_t name;
		VM vm;
		std::set<uint64_t> includes{};
		std::vector<GscFunction> functions{};
		
		GscFile(uint64_t name, VM vm);

		/*
		 * Compile a file into a data vector
		 * @param data
		 */
		void Compile(std::vector<byte>& data);
	};


}