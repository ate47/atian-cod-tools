#include <dll_includes.hpp>

// map used to translate the error codes
std::unordered_map<UINT64, LPCCH> error_handler::errors = {
	{ 1252503459, "caller is not an entity" },
	{ 3122940489, "caller is not an entity" },
	{ 2116335949, "function called with too many parameters" },
	{ 219686544, "object is not an array index" },
	{ 744499668, "too many vehicles" },
	{ 317100267, "unmatching types" },
	{ 4104994143, "can't allocate more script variables" },
	{ 1609894461, "bad entity" },
	{ 1670707254, "linking error" },
	{ 3222417139, "Usage of .size outside of a string or array" },
	{ 2681972741, "bad clientfield for name" },
	{ 2681972741, "bad clientfield for name" },
	{ 3654063291, "ScrEvent map is full, unable to register new event" },
	{ 829015102, "var isn't a field object" },
	{ 2751867714, "self isn't a field object" },
	{ 1427704235, "add to struct with a non pointer/struct value" },
	{ 512306404, "not a function pointer" },
	{ 2538360978, "not a function pointer" },
	{ 2448966512, "string too long" },
	{ 245612264, "foreach should be used with an array" },
	{ 647662103, "var can't be converted to string" },
	{ 4213634562, "precacheLeaderboards must be called before any wait statements in the gametype or level script" },
	{ 3143575744, "parameter does not exist" }, // GetVector
	{ 2517242050, "parameter does not exist" }, // GetString
	{ 4196473479, "parameter does not exist" }, // GetBool
	{ 3699844858, "parameter does not exist" }, // GetType
	{ 280703902, "parameter does not exist" }, // GetPointerType
	{ 312545010, "not a vector" },
	{ 647662103, "parameter can't be cast to a string" },
	{ 952690413, "parameter can't be cast to a boolean" },
	{ 3761634992, "not a pointer" },
	{ 754846421, "invalid vehicle spawn origin" },
};

LPCCH error_handler::FindDesc(UINT64 code) {
	auto nfo = errors.find(code);

	if (nfo == errors.end()) {
		return NULL;
	}
	return nfo->second;
}
