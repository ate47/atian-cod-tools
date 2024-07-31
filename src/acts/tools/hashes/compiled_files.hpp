#pragma once
namespace tool::hashes::compiled_files {
	constexpr uint64_t MAGIC = 0x123456ACCF10;

	struct CompiledFileHeader {
		uint64_t magic{ 0x123456ACCF10 };
		uint64_t name;
		uint8_t isString{};
		uint8_t isSpecial{};
		char type[0x10]{ 0 };
		char preferedExtension[0x10]{ 0 };
	};
}