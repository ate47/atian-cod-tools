#pragma once
#include <includes.hpp>

namespace tool::gscfile {
	using namespace tool::gsc;
	using namespace tool::gsc::opcode;

	class GscFunctionImport {
		UINT32 nsp;
		UINT32 name;
		BYTE flag;
		UINT32 rloc;
	};
	class GscFunctionString {
		std::vector<BYTE> string{};
		BYTE flag;
	};

	struct GscFunctionGlobal {
		UINT32 name;
		UINT32 rloc;
	};

	class GscFunction {
		UINT32 nsp;
		UINT32 callback_event;
		UINT32 name;
		BYTE flags;
		BYTE params;

		std::vector<BYTE> data{};
		std::vector<GscFunctionImport> imports{};
		std::vector<GscFunctionString> strings{};
		std::vector<GscFunctionGlobal> global{};
	};

	class GscFile {
		UINT64 name;
		VM vm;
		std::set<UINT64> includes{};
		std::vector<GscFunction> functions{};
		
		GscFile(UINT64 name, VM vm);

		/*
		 * Compile a file into a data vector
		 * @param data
		 */
		void Compile(std::vector<BYTE>& data);
	};


}