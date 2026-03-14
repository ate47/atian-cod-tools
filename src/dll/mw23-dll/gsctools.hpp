/*
 * MIT License
 * 
 * Copyright (c) 2023 Antoine Willerval
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Standalone file to handle operations on mwiii GSC scripts. Assuming buff is a pointer
 * to a gsc object, the code here remove a dev block and null a function.
 * 
 * // create a tool
 * mwiii::gsc::GscTool tool{ buff };
 * 
 * // check that the buff is correct and point to a valid script object
 * if (tool) {
 *     // remove a dev block call, will fix the import table, but missing strings or
 *     // functions won't be replaced
 *     tool.ClearDevBlock(tool.FindFunction("ob", 0xfcc474b48416cd3c), 0x1c8);
 * 
 *     // replace a function code to return;
 *     tool.NullFunction(tool.FindFunction("ob", "init_vehicles"));
 * }
 */
#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <mutex>
#include <filesystem>
#include <cassert>
#include <functional>
#include <cstdint>

namespace mwiii::gsc {
    // GSC object magic
    constexpr uint64_t GSC_MAGIC = 0xa0d4353478a;

    // small opcode list
    enum IW23OpCode {
        IWOC_DevBlockBegin = 0,
        IWOC_SafeCreateLocalVariables = 0x98,
        IWOC_CheckClearParams = 0x43,
        IWOC_Return = 0x2a,
    };

    // gsc import object
    struct IW23GSCImport {
        uint64_t name;
        uint64_t name_space;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    // gsc export object
    struct IW23GSCExport {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint64_t checksum;
        uint32_t address;
        uint8_t param_count;
        uint8_t flags;
    };

    // gsc object
    struct GscObj23 {
        uint8_t magic[8];
        uint64_t name;
        uint16_t unk10;
        uint16_t animtree_use_count;
        uint16_t animtree_count;
        uint16_t unk16;
        uint16_t export_count;
        uint16_t fixup_count;
        uint16_t unk1C;
        uint16_t imports_count;
        uint16_t includes_count;
        uint16_t unk22;
        uint16_t string_count;
        uint16_t unk26;
        uint16_t unk28;
        uint16_t unk2A;
        uint32_t animtree_use_offset;
        uint32_t animtree_offset;
        uint32_t cseg_offset;
        uint32_t cseg_size;
        uint32_t unk3C;
        uint32_t export_offset;
        uint32_t fixup_offset;
        uint32_t size1;
        uint32_t import_table;
        uint32_t include_table;
        uint32_t size2;
        uint32_t string_table;
        uint32_t unk5C;

        uint64_t GetMagic() {
            return *reinterpret_cast<uint64_t*>(magic);
        }
    };
    constexpr uint64_t Hash64A(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
        uint64_t hash = start;

        for (const char* data = str; *data; data++) {
            if (*data >= 'A' && *data <= 'Z') {
                // to lower
                hash = hash ^ (uint8_t)('a' + (*data - (uint8_t)'A'));
            }
            else if (*data == '\\') {
                // replace path char
                hash = hash ^ '/';
            }
            else {
                hash = hash ^ *data;
            }

            hash *= iv;
        }

        return hash;
    }
    constexpr uint64_t Hash64AField(const char* str) { return Hash64A(str, 0x79D6530B0BB9B5D1, 0x10000000233); }

    // toolkit
    class GscTool {
        GscObj23* gscobj;

    public:
        GscTool(uint8_t* gscobj) : gscobj(reinterpret_cast<GscObj23*>(gscobj)) {
        }
        GscTool(char* gscobj) : gscobj(reinterpret_cast<GscObj23*>(gscobj)) {
        }
        GscTool(uintptr_t gscobj) : gscobj(reinterpret_cast<GscObj23*>(gscobj)) {
        }

        // is the gsc object valid
        operator bool() {
            return gscobj && gscobj->GetMagic() == GSC_MAGIC;
        }

        inline const GscObj23* operator->() {
            return gscobj;
        }

        inline const GscObj23& operator*() {
            return *gscobj;
        }

        // pointer inside the gsc script
        template<typename T = uint8_t>
        inline T* Ptr(size_t offset = 0) {
            return reinterpret_cast<T*>(gscobj->magic + offset);
        }

        // ref inside the gsc script
        template<typename T>
        inline T& Ref(size_t offset = 0) {
            return *Ptr<T>(offset);
        }

        // find a function by namespace and name, if namespace = 0, only by name
        IW23GSCExport* FindFunction(uint64_t name_space, uint64_t name) {
            auto* exports = Ptr<IW23GSCExport>(gscobj->export_offset);
            for (size_t i = 0; i < gscobj->export_count; i++) {
                auto& exp = exports[i];

                if ((exp.name & 0x7FFFFFFFFFFFFFFF) == (name & 0x7FFFFFFFFFFFFFFF) 
                    && (!name_space || (exp.name_space & 0x7FFFFFFFFFFFFFFF) == (name_space & 0x7FFFFFFFFFFFFFFF))) {
                    return &exp;
                }
            }

            return nullptr;
        }

        IW23GSCExport* FindFunction(uint64_t name_space, const char* name) {
            return FindFunction(name_space, name ? Hash64AField(name) : 0);
        }

        IW23GSCExport* FindFunction(const char* name_space, uint64_t name) {
            return FindFunction(name_space ? Hash64AField(name_space) : 0, name);
        }

        IW23GSCExport* FindFunction(const char* name_space, const char* name) {
            return FindFunction(name_space ? Hash64AField(name_space) : 0, name ? Hash64AField(name) : 0);
        }

        // clear a dev block and realloc the import
        void ClearDevBlock(IW23GSCExport* start, uint32_t offset) {
            if (!start) {
                throw std::runtime_error("empty start for dev block");
            }
            // find the dev block
            uint32_t slocStart = start->address + offset;
            uint8_t* bytecode = Ptr(slocStart);

            if (*bytecode != IWOC_DevBlockBegin) {
                throw std::runtime_error("not a dev block");
            }

            int16_t& delta = Ref<int16_t>(slocStart + 1);

            uint32_t slocEnd = slocStart + delta;

            // patch the import so everything is linked

            IW23GSCImport* imports = Ptr<IW23GSCImport>(gscobj->imports_count);
            for (size_t i = 0; i < gscobj->imports_count; i++) {
                IW23GSCImport* imp = imports + i;

                uint32_t* rlocs = reinterpret_cast<uint32_t*>(imp + 1);

                bool patched{};

                for (size_t j = 0; j < imp->num_address; j++) {
                    if (rlocs[j] >= slocStart && rlocs[j] < slocEnd) {
                        patched = true;
                        break;
                    }
                }

                if (patched) {
                    // remove the dev link flag
                    imp->flags &= ~0x10;
                }

                imports = reinterpret_cast<IW23GSCImport*>(rlocs + imp->num_address);
            }

            // remove the block
            delta = 0;
        }
        
        // null a function
        void NullFunction(IW23GSCExport* start) {
            if (!start) {
                throw std::runtime_error("empty start for null function");
            }
            uint8_t* bytecode = Ptr(start->address);

            if (*bytecode == IWOC_CheckClearParams) {
                bytecode++; // pass only one opcode
            }
            else if (*bytecode == IWOC_SafeCreateLocalVariables) {
                bytecode += 1 + sizeof(uint64_t) * *bytecode; // pass the params
            }
            else {
                throw std::runtime_error("invalid function start for null function");
            }

            *bytecode = IWOC_Return;
        }
    };
}