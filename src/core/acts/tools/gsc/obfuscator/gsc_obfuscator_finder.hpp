#pragma once

namespace tool::gsc::obfuscator::finder {
    struct GscObject {
        char* name;
        size_t fileSize;
        byte* obj;
    };

    std::vector<GscObject> FindGscInBuffer(byte* buffer, size_t bufferSize);
} // namespace tool::gsc::obfuscator::finder