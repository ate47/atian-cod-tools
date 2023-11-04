#include <includes_shared.hpp>

char* decrypt::DecryptStringIW(char* str) {
    if ((*str & 0xC0) != 0x80) {
        return str; // already decrypted
    }

    //TODO
    return nullptr;
}