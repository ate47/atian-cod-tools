#include <includes_shared.hpp>

LPCCH alogs::name(loglevel lvl) {
	switch (lvl)
	{
	case alogs::LVL_TRACE:
		return "TRACE";
	case alogs::LVL_ERROR:
		return "ERROR";
	case alogs::LVL_WARNING:
		return "WARN.";
	case alogs::LVL_INFO:
		return "INFO.";
	default:
		return "UKN..";
	}
}
static alogs::loglevel g_loglevel = alogs::LVL_INFO;
void alogs::setlevel(loglevel lvl) {
	g_loglevel = lvl;
}
alogs::loglevel alogs::getlevel() {
	return g_loglevel;
}

std::ostream& alogs::log(loglevel level) {
	if (g_loglevel > level) {
		static std::ofstream nullstream{};
		nullstream.setstate(std::ios_base::badbit);
		// basic impl
		return nullstream;
	}
	return (level > LVL_WARNING ? std::cout : std::cerr) << '[' << name(level) << "] ";
}