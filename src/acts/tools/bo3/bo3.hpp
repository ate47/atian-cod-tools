#pragma once

namespace bo3 {
    constexpr uint64_t GSC_MAGIC = 0x1C000a0d43534780;
    constexpr uint64_t GSC_MAGIC_1B = 0x1B000a0d43534780;

    int InjectScriptBO3(Process& proc, const char* script, const char* replace, std::string& notify);
}