# Atian Call of Duty Tools

My set of tools for Call of Duty. The code is more important than the features, so feel free to reuse it. 🙂

**Supported games**
- Call of Duty: Black Ops 4 (T8)
- Call of Duty: Black Ops Cold War (T9)

## Dependencies

This tool is using and providing

- [ANTLR-4](https://github.com/antlr/antlr4) - [BSD-3-Clause license](https://github.com/antlr/antlr4/blob/master/LICENSE.txt).
- [Detours](https://github.com/microsoft/Detours) - [MIT license](https://github.com/microsoft/Detours/blob/main/LICENSE.md).
- [Madler/zlib](https://github.com/madler/zlib) - [Custom license](https://github.com/madler/zlib/blob/master/LICENSE).
- [asmjit](https://github.com/asmjit/asmjit) - [ZLib license](https://github.com/asmjit/asmjit/blob/master/LICENSE.md).

## Downloads

You can download the latest release here:

- [Latest Release](https://github.com/ate47/atian-cod-tools/releases/latest)
- [Latest Dev Build](https://github.com/ate47/atian-cod-tools/releases/tag/latest_build) (Might be unstable)

## Tools

### Mods

Mods implemented in my tool, run `acts mod` for the list.

- `acts mod t8cee` - enable EEs in Custom mutations, offline or casual (Black Ops 4).
- `acts mod t9cee` - enable EEs in offline (Black Ops Cold War).

### Dump tools

Tools to dump from the game memory, most of these files can be seen in the [t8-atian-menu repo](https://github.com/ate47/t8-atian-menu/tree/master/docs/notes) in my notes.

- dump api functions `acts dfunc [output=func.csv]` (need the game started)
- dump cli functions `acts dcfunc [output=cfunc.csv]` (need the game started)
- dump linked events `acts devents [output=events.csv]` (need the game started)
- dump pooled scripts `acts dps [output=pool.csv]` (need the game started)
- dump linked scripts `acts dls [output=linked.csv]` (need the game started)
- dump linked/pooled scripts on disk `acts wps [output=scriptparsetree]` (need the game started)

### Decompiler/Disassembler

Tools to decompile or disassemble the GSC scripts, a bo4 script decompilation is available in the [bo4-source](https://github.com/ate47/bo4-source) and [bocw-source](https://github.com/ate47/bocw-source) repositories.

- gsc disassembler, made in 3 days with a lot of alcohol so don't use it. `acts gscinfo -a -o "output" [input=scriptparsetree]`
- gsc decompiler, same as the disassembler, but 10 days after, not any better. `acts gscinfo -g -o "output" [input=scriptparsetree]`

### GSC Development

Tools to help with the GSC development.

- gsc vm debugger, dump the function stack when the vm has a crash, `acts dbg`
	- can dump the var stack `-s` local var `-v`
	- can look inside structures with the depth for array `-A [depth]` and structs `-S [depth]`  (need the game started)

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

## Hash resolver tool

The hash resolver tool is to test hash64 values, to use it, you need to create a config file, one config per line.

configs:

- `hashes [file]` - open a file with hashes, one hash per line, example: 
- `add [hash]` - add a hash in the list, example: `add 138f012bb30beb55`
- `dict [list]` - set the dictionary, example: `dict abcdefghijklmnopqrstuvwxyz0123456789_/`
- `pattern [pattern]` - set the pattern, a `???` should be put to tell where to put the search, example: `pattern scripts/zm_common/zm_???.gsc`
- `output [file]` - set the output file, example: `output brute.txt`

## Credits

- Serious's t8-compiler for some opcodes, it fasted up the process of understanding the internal game functions. Also for the childthread operator syntax.
