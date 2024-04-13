#pragma once
#include "data/cw.hpp"

namespace core {
	extern cw::XAssetPool* xassetpools;
	extern byte* (*DecryptString)(byte* str);
}