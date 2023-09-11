# Atian Call of Duty Tools

My set of tools for Call of Duty.

**Supported games**
- Call of Duty: Black Ops 4 (T8)

## Tools

### Mods

- `acts mod_t8_cee` - enable EEs in Custom mutations, offline or casual (Black Ops 4).

### Dump tools

Tools to dump from the game memory, most of these files can be seen in the [t8-atian-menu repo](https://github.com/ate47/t8-atian-menu/tree/master/docs/notes) in my notes.

- dump api functions `acts dfunc [output=func.csv]` (need the game started)
- dump cli functions `acts dcfunc [output=cfunc.csv]` (need the game started)
- dump linked events `acts devents [output=events.csv]` (need the game started)
- dump pooled scripts `acts dps [output=pool.csv]` (need the game started)
- dump linked scripts `acts dls [output=linked.csv]` (need the game started)
- dump linked/pooled scripts on disk `acts wps [output=scriptparsetree]` (need the game started)

### Decompiler/Disassembler

Tools to decompile or disassemble the GSC scripts, a bo4 script decompilation is available in the [bo4-source repo](https://github.com/ate47/bo4-source).

- gsc disassembler, made in 3 days with a lot of alcohol so don't use it. `acts gscinfo -a -o "output" [input=scriptparsetree]`
- gsc decompiler, same as the disassembler, but 10 days after, not any better. `acts gscinfo -g -o "output" [input=scriptparsetree]`

### GSC Development

Tools to help with the GSC development.

- gsc vm debugger, dump the function stack when the vm has a crash, `acts dbg`
	- can dump the var stack `-s` local var `-v`
	- can look inside structures with the depth for array `-A [depth]` and structs `-S [depth]`  (need the game started)

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

## Credits

- Serious's t8-compiler for some opcodes, it fasted up the process of understanding the internal game functions. Also for the childthread operator syntax.
