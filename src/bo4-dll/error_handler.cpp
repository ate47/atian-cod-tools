#include <dll_includes.hpp>

std::vector<error_handler::error_info> error_handler::errors{
	{
		.code = 1252503459,
		.description = "caller is not an entity"
	},
	{
		.code = 3122940489,
		.description = "caller is not an entity"
	},
	{
		.code = 2116335949,
		.description = "function called with too many parameters"
	},
	{
		.code = 219686544,
		.description = "object is not an array index"
	},
	{
		.code = 744499668,
		.description = "too many vehicles"
	},
	{
		.code = 317100267,
		.description = "unmatching types"
	},
	{
		.code = 4104994143,
		.description = "can't allocate more script variables"
	},
	{
		.code = 1609894461,
		.description = "bad entity"
	},
	{
		.code = 1670707254,
		.description = "linking error"
	},
	{
		.code = 3222417139,
		.description = "Usage of .size outside of a string or array"
	},
	{
		.code = 2681972741,
		.description = "bad clientfield for name"
	},
	{
		.code = 2681972741,
		.description = "bad clientfield for name"
	},
	{
		.code = 3654063291,
		.description = "ScrEvent map is full, unable to register new event"
	},
	{
		.code = 829015102,
		.description = "var isn't a field object"
	},
	{
		.code = 2751867714,
		.description = "self isn't a field object"
	},
	{
		.code = 1427704235,
		.description = "add to struct with a non pointer/struct value"
	},
	{
		.code = 512306404,
		.description = "not a function pointer"
	},
	{
		.code = 2538360978,
		.description = "not a function pointer"
	},
	{
		.code = 2448966512,
		.description = "string too long"
	},
	{
		.code = 245612264,
		.description = "foreach should be used with an array"
	},
	{
		.code = 647662103,
		.description = "var can't be converted to string"
	}
};

LPCCH error_handler::FindDesc(UINT32 code) {
	auto nfo = std::find_if(errors.begin(), errors.end(), [code](const error_handler::error_info& v) { return v.code == code; });

	return nfo == errors.end() ? NULL : nfo->description;
}
