#pragma once

#include <includes_shared.hpp>
#include <hook/refs.hpp>
#include <utils/hash.hpp>
#include <core/system.hpp>
#include <hook/scan_container.hpp>

#define EXPORT extern "C" __declspec(dllexport)

namespace acts {
	hook::scan_container::ScanContainer& Scan();
}