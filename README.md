# Atian Tools

[![Build](https://github.com/ate47/atian-cod-tools/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/ate47/atian-cod-tools/actions/workflows/build.yml)

My set of tools. The code is more important than the features, so feel free to reuse it. 🙂

![meme](docs/banner.png)

**Wiki**

See the wiki to know how to use the features

- [GSC Compiler](https://github.com/ate47/atian-cod-tools/wiki/GSC-Compiler)
- [GSC Decompiler](https://github.com/ate47/atian-cod-tools/wiki/GSC-Decompiler)
- [XAsset Dumper](https://github.com/ate47/atian-cod-tools/wiki/XAsset-Dumper)
- [Fast Files Dumper](https://github.com/ate47/atian-cod-tools/wiki/Fast-Files-Dumper)

**Table of contents**

- [Atian Tools](#atian-tools)
	- [GSC Compiler/Decompiler](#gsc-compilerdecompiler)
	- [Fast File Reader](#fast-file-reader)
	- [Fast File Linker](#fast-file-linker)
	- [Downloads](#downloads)
	- [Related repositories](#related-repositories)
	- [Lookup](#lookup)
	- [Dependencies](#dependencies)
	- [Credits](#credits)

## GSC Compiler/Decompiler

**Supported game**

| Name                     | Rev | Decompiler | Compiler | Type(s)      | Source\*                                                      |
| ------------------------ | --- | ---------- | -------- | ------------ | ------------------------------------------------------------- |
| Black Ops 3 (T7)         | 1A  | ✅          | ✅        | pc           | None       |
| Black Ops 3 (T7)         | 1B  | Partial    | ✅        | pc           | [ate47/oldcod-source](https://github.com/ate47/oldcod-source)       |
| Black Ops 3 (T7)         | 1C  | ✅          | ✅        | pc,ps4       | [ate47/bo3-source](https://github.com/ate47/bo3-source)       |
| Black Ops 4 (T8)         | 31  | ✅          | ✅        | ps4          | [ate47/oldcod-source](https://github.com/ate47/oldcod-source) |
| Black Ops 4 (T8)         | 34  | ✅          | ✅        | pc,ps4       | [ate47/oldcod-source](https://github.com/ate47/oldcod-source) |
| Black Ops 4 (T8)         | 35  | Partial    | ❌        | pc           | None                                                          |
| Black Ops 4 (T8)         | 36  | ✅          | ✅        | pc,ps4       | [ate47/bo4-source](https://github.com/ate47/bo4-source)       |
| Black Ops Cold War (T9)  | 37  | Dc         | ✅        | pc,ps4,alpha | [ate47/oldcod-source](https://github.com/ate47/oldcod-source)     |
| Black Ops Cold War (T9)  | 38  | Dc         | ✅        | pc,ps4       | [ate47/bocw-source](https://github.com/ate47/bocw-source)     |
| Modern Warfare III (JUP) | 8A  | ✅          | ✅        | all         | [ate47/mwiii-source](https://github.com/ate47/mwiii-source)   |
| Modern Warfare III (JUP) | 8B  | Dc         | ✅        | all         | [ate47/mwiii-source](https://github.com/ate47/mwiii-source)   |
| Black Ops 6 (T10)        | 06  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 07  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0B  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0C  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0D  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0E  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 10  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 11  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 12  | Dc         | ✅        | all         | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 14  | Dc         | ✅        | bo6+bo6_v1  | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 7 (SAT)        | 14  | Dc         | ✅        | bo7         | [ate47/bo7-source](https://github.com/ate47/bo7-source)       |

- \* : Some source repositories might not be public yet.
- **Dc**: With pre-decode, see the [Using encrypted scripts in the wiki](https://github.com/ate47/atian-cod-tools/wiki/GSC-Decompiler#using-encrypted-scripts).
- **Ex**: With extensions, ***The extensions aren't provided publicly, at least not by me.***

## Fast File Reader

| Game                  | Reader | Handler   | Pools      |
| --------------------- | ------ | --------- | ---------- |
| Black Ops 3           | ✅      | `bo3`     | `rawfile`, `scriptparsetree` |
| Black Ops 4           | ✅      | `bo4`     | `rawfile`, `rawfile`, `scriptparsetree`, `scriptparsetreedbg`, `bgcache`, `keyvaluepairs`, `luafile`, `rawstring`, `scriptbundle`, `scriptparsetreeforced`, `streamkey`, `stringtable`, `weapon`, `weapon_tunables`, `localize`, `structuredtable`, `localizeentry` |
| Modern Warfare 2019   | ✅      | `mw19`      | `rawfile`, `scriptfile` (gpl\*), `luafile`, `localize` |
| Black Ops Cold War    | ✅      | `cw`      | `rawfile`, `scriptparsetree`, `scriptbundle`, `bgcache`, `ddl`, `localizeentry`|
| Vanguard              | ✅      | `vg`      | `rawfile`, `scriptfile` (gpl\*), `luafile`, `localize` |
| Modern Warfare II     | ✅      | `mwii`    | `rawfile`, `scriptfile` (gpl\*), `luafile`, `stringtable`, `ddl`, `scriptbundle`, `localize` |
| Modern Warfare II SP  | ✅      | `mwiisp`  | `rawfile`, `scriptfile` (gpl\*), `luafile`, `stringtable`, `ddl`, `scriptbundle`, `localize` |
| Modern Warfare III    | ✅      | `mwiii`   | `rawfile`, `gscobj`, `luafile`, `stringtable`, `ddl`, `scriptbundle`, `localize` |
| Modern&nbsp;Warfare&nbsp;III&nbsp;SP | ✅      | `mwiiisp` | `rawfile`, `gscobj`, `luafile`, `stringtable`, `ddl`, `scriptbundle`, `localize` |
| Black Ops 6           | ✅      | `bo6`     | `rawfile`, `gscobj`, `gscgdb`, `luafile`, `aitype`, `scriptbundle`, `stringtable`, `keyvaluepairs`, `netconststrings`, `operator`, `operatorlist`, `reactiveoperator`, `operatorskin`, `spray`, `contraildata`, `parachutedata`, `ddl`, `aishootstyleslist`, `ai_token_types`, `ai_token_stealing_definitions`, `ai_token_definitions`, `localize`, `soundbank`, `banktransient` |
| Black Ops 6 SP        | ✅      | `bo6sp`   | `rawfile`, `gscobj`, `gscgdb`, `stringtable`, `ddl`, `scriptbundle`, `netconststrings`, `luafile`, `aitype`, `localize` |
| Black Ops 7           | ✅      | `bo7`     | `rawfile`, `gscobj`, `gscgdb`, `luafile`, `aitype`, `scriptbundle`, `stringtable`, `keyvaluepairs`, `netconststrings`, `operator`, `operatorlist`, `reactiveoperator`, `operatorskin`, `spray`, `contraildata`, `parachutedata`, `ddl`, `aishootstyleslist`, `ai_token_types`, `ai_token_stealing_definitions`, `ai_token_definitions`, `localize`, `soundbank`, `banktransient` |

- gpl\*: Only working within the GPL-3 distribution using [xensik/gsc-tool](https://github.com/xensik/gsc-tool).

The `gsc` handler can read  `scriptparsetree`, `gscobj` or `scriptfile` pools from decompressed fastfiles.

## Fast File Linker

| Game                  | Linker   | Pools      |
| --------------------- | --------- | ---------- |
| Black Ops 4           | `bo4`     | `rawfile`, `scriptparsetree`, `scriptparsetreedbg`, `scriptparsetreeforced`, `structuredtable`, `luafile`, `rawstring`, `scriptbundle`, `stringtable`, `image`, `localize`, `ttf`, `bgcache` |
| Black&nbsp;Ops&nbsp;Cold&nbsp;War    | `cw`     | `rawfile` |

## Downloads

You can download the latest release here:

- [Latest Release](https://github.com/ate47/atian-cod-tools/releases/latest)
- [Latest Dev Build](https://github.com/ate47/atian-cod-tools/releases/tag/latest_build) (Might be unstable)

The tool `checkupdate` allows to check for the latest update.

## Related repositories

- [ate47/t8-atian-menu](https://github.com/ate47/t8-atian-menu/tree/master/docs/notes) : All the notes and dump generated by this tool. (All games)
- [ate47/BOHashTool](https://github.com/ate47/BOHashTool) : Tool to test hashes with error (en/de)coder for Black Ops games.
- [ate47/HashIndex](https://github.com/ate47/HashIndex) : Hashes storage generated to hash files.
- [ate47/oldcod-source](https://github.com/ate47/oldcod-source) : Repository where the GSC decompiler is tested.
- [ate47/cod-source](https://github.com/ate47/cod-source) : Repository where the Fast File handlers are tested.

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

You can also use .wni compiled files, create a directory with the name `package_index` and put the .wni files in it.

The tool `download_hash_index` allows to download the latest hash index from [ate47/HashIndex](https://github.com/ate47/HashIndex).

## Dependencies

This tool is using and providing different dependencies, see [LICENSE.md](LICENSE.md) for a full list.

## Credits

- [Serious's t8-compiler](https://github.com/shiversoftdev/t7-compiler) for some opcodes for Black Ops 4, it fasted up the process of understanding the internal game functions. Also for the childthread operator syntax.
- [JariKCoding's CoDLuaDecompiler](https://github.com/JariKCoding/CoDLuaDecompiler) to understand the Lua HK scripting format.
- [Jake-NotTheMuss's hksc](https://github.com/Jake-NotTheMuss/hksc) I've modified to support BO4 Lua compilation for the fastfile linker.
