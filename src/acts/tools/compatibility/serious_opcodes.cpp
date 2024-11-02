#include <includes.hpp>
#include <utils.hpp>
#include <core/strings.hpp>
#include "tools/gsc.hpp"
#include "compiler/preprocessor.hpp"

namespace {

	int serious_opcodes_read(int argc, const char* argv[]) {
		if (argc < 4) return tool::BAD_USAGE;

		std::string file{};
		if (!utils::ReadFile(argv[2], file)) {
			LOG_ERROR("Can't read {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		acts::compiler::preprocessor::PreProcessorOption opt{};

		opt.ApplyPreProcessor(file, [&argv](alogs::loglevel lvl, size_t line, const std::string& message) { alogs::log(lvl, std::format("{}@{}: {}", argv[2], line, message)); });


		LOG_INFO("Parsing file...");

		std::map<std::string, std::unordered_set<std::string>> dataset{};

		size_t curr{};
		while (curr < file.length()) {
			size_t end{ file.find(';', curr) };

			if (end == std::string::npos) end = file.length();
			size_t idx{ curr };
			curr = end + 1;

			core::strings::TrimIdx(file, idx, end);

			if (idx >= end) continue; // empty line

			std::string_view line{ &file[idx], end - idx };

			size_t split{ line.find('=') };

			if (split == std::string::npos) {
				LOG_WARNING("Invalid instruction {}", line);
				continue;
			}

			std::string k{ core::strings::Trim(line, 0, split) };
			std::string v{ core::strings::Trim(line, split + 1, line.length()) };

			dataset[v].insert(k);
		}

		std::ofstream of{ argv[3] };
		if (!of) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}
		utils::CloseEnd ce{ of };
		for (const auto& [k, vs] : dataset) {
			of << k;
			for (const auto& v : vs) {
				of << ", " << v;
			}
			of << "\n";
		}

		return tool::OK;
	}
	int serious_opcodes_write(int argc, const char* argv[]) {
		if (argc < 5) return tool::BAD_USAGE;
		const char* output{ argv[2] };

		tool::gsc::opcode::VmInfo* nfo{};
		if (!tool::gsc::opcode::IsValidVm(tool::gsc::opcode::VMOf(argv[3]), nfo)) {
			LOG_ERROR("Invalid vm: {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		tool::gsc::opcode::Platform plt{ tool::gsc::opcode::PlatformOf(argv[4]) };

		if (!plt || !nfo->HasPlatform(plt)) {
			LOG_ERROR("Invalid platform for this vm: {}", argv[4]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Dumping {}/{} into {}...", nfo->name, tool::gsc::opcode::PlatformName(plt), output);

		std::ofstream of{ output };
		if (!of) {
			LOG_ERROR("Can't open {}", output);
			return tool::BASIC_ERROR;
		}
		utils::CloseEnd ce{ of };

		auto mapIt{ nfo->opcodemappltlookup.find(plt) };
		if (mapIt != nfo->opcodemappltlookup.end()) {
			std::map<uint16_t, tool::gsc::opcode::OPCode> map{};
			for (const auto& [k, vs] : mapIt->second) {
				for (const auto v : vs) map[v] = k;
			}
			for (const auto& [val, opcode] : map) {
				of << "0x" << std::hex << val << " = " << tool::gsc::opcode::OpCodeName(opcode) << ";\n";
			}
		}

		return tool::OK;
	}
	int serious_opcodes_translate(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		std::string file{};
		if (!utils::ReadFile(argv[2], file)) {
			LOG_ERROR("Can't read {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		acts::compiler::preprocessor::PreProcessorOption opt{};

		opt.ApplyPreProcessor(file, [&argv](alogs::loglevel lvl, size_t line, const std::string& message) { alogs::log(lvl, std::format("{}@{}: {}", argv[2], line, message)); });

		const char* output{ argv[5] };

		tool::gsc::opcode::VmInfo* nfo{};
		if (!tool::gsc::opcode::IsValidVm(tool::gsc::opcode::VMOf(argv[3]), nfo)) {
			LOG_ERROR("Invalid vm: {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		tool::gsc::opcode::Platform plt{ tool::gsc::opcode::PlatformOf(argv[4]) };

		if (!plt || !nfo->HasPlatform(plt)) {
			LOG_ERROR("Invalid platform for this vm: {}", argv[4]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Parsing file using {}/{} into {}...", nfo->name, tool::gsc::opcode::PlatformName(plt), output);

		std::set<std::string> unknown{};
		std::map<std::string, std::set<std::string>> dataset{};

		size_t curr{};
		while (curr < file.length()) {
			size_t end{ file.find(';', curr) };

			if (end == std::string::npos) end = file.length();
			size_t idx{ curr };
			curr = end + 1;

			core::strings::TrimIdx(file, idx, end);

			if (idx >= end) continue; // empty line

			std::string_view line{ &file[idx], end - idx };

			size_t split{ line.find('=') };

			if (split == std::string::npos) {
				LOG_WARNING("Invalid instruction {}", line);
				continue;
			}

			std::string k{ core::strings::Trim(line, 0, split) };
			std::string v{ core::strings::Trim(line, split + 1, line.length()) };

			int64_t opcodeId{ utils::ParseFormatInt(k.c_str()) };

			const tool::gsc::opcode::OPCodeInfo* opnfo{ tool::gsc::opcode::LookupOpCode(nfo->vmMagic, plt, (uint16_t)opcodeId) };

			dataset[tool::gsc::opcode::OpCodeName(opnfo->m_id)].insert(v);

		}

		std::ofstream of{ argv[5] };
		if (!of) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}
		utils::CloseEnd ce{ of };
		of << "acts,serious";
		for (const auto& [k, vs] : dataset) {
			of << "\n" << k << ",";
			auto it = vs.begin();
			if (it != vs.end()) {
				of << *(it++);
				while (it != vs.end()) {
					of << ";" << *(it++);
				}
			}
		}

		return tool::OK;
	}

	ADD_TOOL(serious_opcodes_read, "compatibility", " [file] [out]", "Dump opcodes file", serious_opcodes_read);
	ADD_TOOL(serious_opcodes_write, "compatibility", " [out] [vm] [plt]", "Create opcodes file", serious_opcodes_write);
	ADD_TOOL(serious_opcodes_translate, "compatibility", " [file] [vm] [plt] [output]", "Translate opcode names to acts names", serious_opcodes_translate);
}