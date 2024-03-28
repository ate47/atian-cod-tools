#include <includes_shared.hpp>
#include "scriptinstance.hpp"

const char* scriptinstance::Name(ScriptInstance inst) {
	switch (inst) {
	case SI_SERVER:
		return "server";
	case SI_CLIENT:
		return "client";
	default:
		return "<error>";
	}
}
