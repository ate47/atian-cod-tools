#include <includes.hpp>
#include <deps/idc_builder.hpp>

namespace {

	int idc_test(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		deps::idc_builder::IdcBuilder builder{};

		builder.AddHeader("#define MY_VAL 42");

		builder.AddStruct("MyStruct");
		builder.AddStruct("MyOptStruct", true);

		deps::idc_builder::IdcEnumId myEnum1{ builder.AddEnum("MyEnum1") };
		builder.AddEnumMember(myEnum1, "MYE1_MY_VAL1");
		builder.AddEnumMember(myEnum1, "MYE1_MY_VAL2", 32);
		builder.AddEnumMember(myEnum1, "MYE1_MY_VAL3", 4);
		builder.AddEnumMember(myEnum1, "MYE1_MY_VAL4");


		deps::idc_builder::IdcEnumId myEnum2{ builder.AddEnum("MyEnum2", true) };
		builder.AddEnumMember(myEnum2, "MYE2_MY_VAL1", 0x20);
		builder.AddEnumMember(myEnum2, "MYE2_MY_VAL2", 0x40);
		builder.AddEnumMember(myEnum2, "MYE2_MY_VAL3", 0x80);
		builder.AddEnumMember(myEnum2, "MYE2_MY_VAL4", -42);
		
		builder.AddAddress(idc_test, "idc_test", "int idc_test(int argc, const char* argv[])");
		builder.AddAddress(tool::NotEnoughParam, "tool::NotEnoughParam");


		builder.WriteIdcFile(argv[2]);
		LOG_INFO("created {}", argv[2]);

		return tool::OK;
	}

	ADD_TOOL(idc_test, "dev", " [out]", "", idc_test);
}