# Atian Tools

[![Build](https://github.com/ate47/atian-cod-tools/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/ate47/atian-cod-tools/actions/workflows/build.yml)

My set of tools. The code is more important than the features, so feel free to reuse it. 🙂

![meme](docs/banner.png)

**Table of contents**

- [Atian Tools](#atian-tools)
	- [GSC Compiler/Decompiler](#gsc-compilerdecompiler)
	- [Dumper](#dumper)
	- [ACTS Lib](#acts-lib)
	- [Dependencies](#dependencies)
	- [Downloads](#downloads)
	- [Related repositories](#related-repositories)
	- [Lookup](#lookup)
	- [Credits](#credits)


## GSC Compiler/Decompiler

**Supported features**

| Name                     | Revision | Decompiler | Compiler | PS4 support |
| ------------------------ | -------- | ---------- | -------- | ----------- |
| Black Ops 3 (T7)         | 1B       | Partial    | ✅        | ❌           |
| Black Ops 3 (T7)         | 1C       | ✅          | ✅        | ❌           |
| Black Ops 4 (T8)         | 36       | ✅          | ✅        | EXT         |
| Black Ops Cold War (T9)  | 37       | DEC        | ✅        | EXT         |
| Black Ops Cold War (T9)  | 38       | DEC        | ✅        | EXT         |
| Modern Warfare III (JUP) | 8A       | EXT        | EXT      | ❌           |
| Modern Warfare III (JUP) | 8B       | DEC & EXT  | EXT      | ❌           |
| Black Ops 6 (CER)        | 06       | DEC & EXT  | ✅       | ❌           |
| Black Ops 6 (CER)        | 07       | DEC & EXT  | EXT      | ❌           |

- **DEC**: With pre-decode
- **EXT**: With extensions, ***The extensions aren't provided publicly, at least not by me.***

**Commands**
```pwsh
# Compile gsc file

acts gscc <input.gsc> -g <game>

# Example
acts gscc my_script.gsc -g cw # Compile my_script.gsc into a cold war script
```

```
# Decompile gsc file

acts gscd file.gscc -g

# Example
acts gscd compiled.gscc -g # Decompile the script compiled.gscc
```

## Dumper

**Supported pools**

- Black Ops 3: `scriptbundle`, `stringtable`, `structuredtable`, `rawfile`, `scriptparsetree`.
- Black Ops 4: `weapon`, `customizationtable`, `rawfile`, `stringtable`, `structuredtable`, `ddl`, `scriptparsetree`, `scriptparsetreeforced`, `scriptbundle`, `scriptbundlelist`, `ttf`, `bgcache`, `maptable`, `maptablelist`, `maptableloadingimages`, `maptablepreviewimages`, `playerrolecategory`, `playerrolecategorytable`, `gametypetable`, `unlockableitem`, `unlockableitemtable`, `playlists`, `hierarchicaltasknetwork`, `storagefile`, `storagefilelist`, `storeproduct`, `storecategory`, `storecategorylist`, `rank`, `ranktable`, `prestige`, `prestigetable`, `labelstore`, `labelstorelist`, `rawstring`.
- Black Ops Cold War (Dec): `rawfile`, `rawfilepreproc`, `rawtextfile`, `stringtable`, `scriptparsetree`, `scriptbundle`.
- Modern Warfare III (COR): `gscobj`, `scriptbundle`, `stringtable`, `localize`, `luafile`, `ddl`.

- **DEC**: Requires pre-decode
- **COR**: Using [Cordycep](https://github.com/Scobalula/Cordycep).

**Commands**
```pwsh
# Command
acts dp <pool>

# Example
acts dp stringtable
```

Some dumpers are writing to Compiled Files (.cf), the command cfd (cf dump) can extract them.

```pwsh
# Command
acts cfd <files>

# Example
acts cfd output_dump
```

## ACTS Lib

Prototype libary for random stuff, probably not linked to Call of Duty.

## Dependencies

This tool is using and providing

- [ANTLR-4](https://github.com/antlr/antlr4) - [BSD-3-Clause license](https://github.com/antlr/antlr4/blob/master/LICENSE.txt).
- [Detours](https://github.com/microsoft/Detours) - [MIT license](https://github.com/microsoft/Detours/blob/main/LICENSE.md).
- [Madler/zlib](https://github.com/madler/zlib) - [Custom license](https://github.com/madler/zlib/blob/master/LICENSE).
- [asmjit](https://github.com/asmjit/asmjit) - [ZLib license](https://github.com/asmjit/asmjit/blob/master/LICENSE.md).
- [ladislav-zezula/CascLib](https://github.com/ladislav-zezula/CascLib) - [MIT license](https://github.com/ladislav-zezula/CascLib/blob/master/LICENSE).
- [lz4/lz4](https://github.com/lz4/lz4) - [Custom license](https://github.com/lz4/lz4/blob/dev/lib/LICENSE).
- [curl/curl](https://github.com/curl/curl) - [Custom license](https://github.com/curl/curl/blob/master/COPYING).
- [vimpunk/mio](https://github.com/vimpunk/mio) - [MIT license](https://github.com/vimpunk/mio/blob/master/LICENSE).
- [JustasMasiulis/xorstr](https://github.com/JustasMasiulis/xorstr) - [Apache License 2.0](https://github.com/JustasMasiulis/xorstr/blob/master/LICENSE)
- [ocornut/imgui](https://github.com/ocornut/imgui) - [MIT license](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
- [biocomp/hw_break](https://github.com/biocomp/hw_break) - [MIT license](https://github.com/biocomp/hw_break/blob/master/LICENSE)
- [Tencent/rapidjson](https://github.com/Tencent/rapidjson) - [MIT and JSON license](https://github.com/Tencent/rapidjson/blob/master/license.txt)
- [glfw/glfw](https://github.com/glfw/glfw) - [zlib/libpng license](https://github.com/glfw/glfw/blob/master/LICENSE.md)
- [bshoshany/thread-pool](https://github.com/bshoshany/thread-pool) - [MIT license](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt)
- [nothings/stb](https://github.com/nothings/stb) - [MIT license](https://github.com/nothings/stb/blob/master/LICENSE)

## Downloads

You can download the latest release here:

- [Latest Release](https://github.com/ate47/atian-cod-tools/releases/latest)
- [Latest Dev Build](https://github.com/ate47/atian-cod-tools/releases/tag/latest_build) (Might be unstable)

## Related repositories

- [ate47/t8-atian-menu](https://github.com/ate47/t8-atian-menu/tree/master/docs/notes) : All the notes and dump generated by this tool. (All games)
- [ate47/bo3-source](https://github.com/ate47/bo3-source) : Black Ops 3 Dump
- [ate47/bo4-source](https://github.com/ate47/bo4-source) : Black Ops 4 Dump
- [ate47/bocw-source](https://github.com/ate47/bocw-source) : Black Ops Cold War Dump
- [ate47/mwiii-source](https://github.com/ate47/mwiii-source) : Modern Warfare III Dump
- [ate47/bo6-source](https://github.com/ate47/bo6-source) : Black Ops 6 Dump
- [ate47/BOHashTool](https://github.com/ate47/BOHashTool) : Tool to test hashes with error (en/de)coder for Black Ops games

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

You can also use .wni compiled files, create a directory with the name `package_index` and put the .wni files in it.

## Credits

- Serious's t8-compiler for some opcodes for Black Ops 4, it fasted up the process of understanding the internal game functions. Also for the childthread operator syntax.
