#pragma once
#include <imgui.h>

namespace tool::nui {

	typedef bool(*toolfunctionui)();

	class tooluifunctiondata {
	public:
		const char* m_id;
		const char* m_description;
		toolfunctionui m_func;

		tooluifunctiondata(const char* id, const char* description, toolfunctionui registerFunc);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::nui::tooluifunctiondata*>& tools();

	void OpenNuiWindow();
}

#define ADD_TOOL_NUI(id, desc, function) static tool::nui::tooluifunctiondata __tooluifunctiondata_##function(id, desc, function)