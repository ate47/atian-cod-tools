#pragma once

namespace tool::ui {
	typedef int(*toolfunctionui)(HWND window);

	class tooluifunctiondata {
	public:
		const char* m_id;
		const char* m_description;
		toolfunctionui m_func;
		tooluifunctiondata(const char* id, const char* description, toolfunctionui registerFunc);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::ui::tooluifunctiondata*>& tools();


}
#define ADD_TOOL_UI(id, desc, function) static tool::tooluifunctiondata __tooluifunctiondata_##function(id, desc, function)