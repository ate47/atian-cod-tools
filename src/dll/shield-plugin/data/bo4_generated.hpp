#pragma once
#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <hook/generated_scan_header.hpp>
namespace bo4 {
    class ScopedCriticalSection;
#include "bo4_generated.hpp.inl"

#define Cmd_AddCommand(name, func)                                                                                     \
    do {                                                                                                               \
        static bo4::cmd_function_t func##_VAR;                                                                         \
        XHash cmdName{ hash::Hash64(name) };                                                                           \
        bo4::Cmd_AddCommandInternal(&cmdName, func, &func##_VAR);                                                      \
    } while (0)

    inline void R_AddCmdDrawText(
        const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation,
        const vec_t* color, int style
    ) {
        AddBaseDrawTextCmd(text, maxChars, font, x, y, xScale, yScale, rotation, color, style, -1, 0, 0);
    }

    inline void R_AddCmdDrawTextWithCursor(
        const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation,
        const vec_t* color, int style, int cursorPos, char cursor
    ) {
        AddBaseDrawTextCmd(text, maxChars, font, x, y, xScale, yScale, rotation, color, style, cursorPos, cursor, 0);
    }

    class ScopedCriticalSection {
        int32_t _s;
        bool _hasOwnership;
        bool _isScopedRelease;
        ScopedCriticalSection* _next;

      public:
        ScopedCriticalSection(int32_t s, ScopedCriticalSectionType type) {
            ScopedCriticalSectionConstructor(this, s, type);
        }
        ~ScopedCriticalSection() { ScopedCriticalSectionDestructor(this); }
    };
} // namespace bo4