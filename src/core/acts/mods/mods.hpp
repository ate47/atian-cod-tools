#pragma once
#include <includes.hpp>


namespace mods {

	typedef int(*modfunction)(int argc, const char* argv[]);

	class modfunctiondata {
	public:
		const char* m_name;
		const char* m_description;
		modfunction m_func;
		modfunctiondata(const char* name, const char* description, modfunction func);

		bool operator!() const;
		bool operatorbool() const;
	};

	/*
	 * @return map of the mods
	 */
	std::map<std::string, modfunctiondata*>& mods();

	/*
	 * find a mod by a name
	 * @param name name
	 * @return mod, can be invalid for a bad name
	 */
	const modfunctiondata& findmod(const char* name);
}

#define ADD_MOD(name, desc, function) static mods::modfunctiondata __modfunctiondata_##function(name, desc, function)
