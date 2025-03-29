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
	- [Dumper](#dumper)
	- [Fast File Handler](#fast-file-handler)
	- [ACTS Lib](#acts-lib)
	- [Dependencies](#dependencies)
	- [Downloads](#downloads)
	- [Related repositories](#related-repositories)
	- [Lookup](#lookup)
	- [Credits](#credits)


## GSC Compiler/Decompiler

**Supported game**

| Name                     | Rev | Decompiler | Compiler | Type(s)      | Source\*                                                      |
| ------------------------ | --- | ---------- | -------- | ------------ | ------------------------------------------------------------- |
| Black Ops 3 (T7)         | 1B  | Partial    | ✅        | pc           | [ate47/bo3-source](https://github.com/ate47/bo3-source)       |
| Black Ops 3 (T7)         | 1C  | ✅          | ✅        | pc,ps4       | [ate47/bo3-source](https://github.com/ate47/bo3-source)       |
| Black Ops 4 (T8)         | 31  | ✅          | ✅        | ps4          | [ate47/oldcod-source](https://github.com/ate47/oldcod-source) |
| Black Ops 4 (T8)         | 34  | ✅          | ✅        | ps4          | [ate47/oldcod-source](https://github.com/ate47/oldcod-source) |
| Black Ops 4 (T8)         | 35  | Partial    | ❌        | pc           | None                                                          |
| Black Ops 4 (T8)         | 36  | ✅          | ✅        | pc,ps4       | [ate47/bo4-source](https://github.com/ate47/bo4-source)       |
| Black Ops Cold War (T9)  | 37  | Dc         | ✅        | pc,ps4,alpha | [ate47/bocw-source](https://github.com/ate47/bocw-source)     |
| Black Ops Cold War (T9)  | 38  | Dc         | ✅        | pc,ps4       | [ate47/bocw-source](https://github.com/ate47/bocw-source)     |
| Modern Warfare III (JUP) | 8A  | ✅          | ✅        | pc           | [ate47/mwiii-source](https://github.com/ate47/mwiii-source)   |
| Modern Warfare III (JUP) | 8B  | Dc         | ✅        | pc           | [ate47/mwiii-source](https://github.com/ate47/mwiii-source)   |
| Black Ops 6 (T10)        | 06  | Dc & Ex    | ✅        | alpha        | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 07  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0B  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0C  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0D  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 0E  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 10  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |
| Black Ops 6 (T10)        | 11  | Dc & Ex    | Ex       | -            | [ate47/bo6-source](https://github.com/ate47/bo6-source)       |

- \* : Some source repositories might not be public yet.
- **Dc**: With pre-decode, see the [Using encrypted scripts in the wiki](https://github.com/ate47/atian-cod-tools/wiki/GSC-Decompiler#using-encrypted-scripts).
- **Ex**: With extensions, ***The extensions aren't provided publicly, at least not by me.***

## Dumper

**Supported pools**

- Black Ops 3: `scriptbundle`, `stringtable`, `structuredtable`, `rawfile`, `scriptparsetree`.
- Black Ops 4: `weapon`, `customizationtable`, `rawfile`, `stringtable`, `structuredtable`, `ddl`, `scriptparsetree`, `scriptparsetreeforced`, `scriptbundle`, `scriptbundlelist`, `ttf`, `bgcache`, `maptable`, `maptablelist`, `maptableloadingimages`, `maptablepreviewimages`, `playerrolecategory`, `playerrolecategorytable`, `gametypetable`, `unlockableitem`, `unlockableitemtable`, `playlists`, `hierarchicaltasknetwork`, `storagefile`, `storagefilelist`, `storeproduct`, `storecategory`, `storecategorylist`, `rank`, `ranktable`, `prestige`, `prestigetable`, `labelstore`, `labelstorelist`, `rawstring`.
- Black Ops Cold War (DEC): `rawfile`, `rawfilepreproc`, `rawtextfile`, `stringtable`, `scriptparsetree`, `scriptbundle`.
- Modern Warfare 2019 (COR): `scriptfile`.
- Modern Warfare III (COR): `gscobj`, `scriptbundle`, `stringtable`, `localize`, `luafile`, `ddl`.
- Black Ops 6 (COR): `gscobj`, `scriptbundle`, `stringtable`, `localize`, `ddl`, `operator`, `operatorlist`, `operatorskin`, `luafile`, `rawfile`, `storagefile`, `project`, `projecttable`, `character`.

- **DEC**: Requires pre-decode
- **COR**: Using [Cordycep](https://github.com/Scobalula/Cordycep).

## Fast File Handler

**Supported pools**

- `bo3`: `rawfile`, `scriptparsetree`
- `bo4`: `rawfile`, `scriptparsetree`, `bgcache`, `keyvaluepairs`, `luafile`, `rawstring`, `scriptbundle`, `scriptparsetreeforced`, `streamkey`, `stringtable`, `weapon`, `weapon_tunables`
- `GscXHash`: `scriptparsetree`
- `GscXString`: `scriptparsetree`, `scriptparsetreedbg`

## ACTS Lib

Prototype libary for random stuff, probably not linked to Call of Duty.

## Dependencies

This tool is using and providing different dependencies, see [LICENSE.md](LICENSE.md) for a full list.

## Downloads

You can download the latest release here:

- [Latest Release](https://github.com/ate47/atian-cod-tools/releases/latest)
- [Latest Dev Build](https://github.com/ate47/atian-cod-tools/releases/tag/latest_build) (Might be unstable)

## Related repositories

- [ate47/t8-atian-menu](https://github.com/ate47/t8-atian-menu/tree/master/docs/notes) : All the notes and dump generated by this tool. (All games)
- [ate47/BOHashTool](https://github.com/ate47/BOHashTool) : Tool to test hashes with error (en/de)coder for Black Ops games

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

You can also use .wni compiled files, create a directory with the name `package_index` and put the .wni files in it.

The tool `download_hash_index` allows to download the latest hash index from [ate47/HashIndex](https://github.com/ate47/HashIndex).

## Credits

- [Serious's t8-compiler](https://github.com/shiversoftdev/t7-compiler) for some opcodes for Black Ops 4, it fasted up the process of understanding the internal game functions. Also for the childthread operator syntax.
- [JariKCoding's CoDLuaDecompiler](https://github.com/JariKCoding/CoDLuaDecompiler) to understand the Lua HK scripting format.
