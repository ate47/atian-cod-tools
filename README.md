# Atian Call of Duty Tools

My set of tools for Call of Duty.

**Supported games**
- Call of Duty: Black Ops 4 (T8)

## Tools

- dump linked events `gsc dfunc [output=func.csv]` (need the game started)
- dump api functions `gsc devents [output=events.csv]` (need the game started)
- dump pooled scripts `gsc dps [output=pool.csv]` (need the game started)
- dump linked scripts `gsc dls [output=linked.csv]` (need the game started)
- dump linked/pooled scripts on disk `gsc wps [output=scriptparsetree]` (need the game started)
- gsc disassembler, made in 3 days with a lot of alcohol so don't use it. `gsc gscinfo -a -o "output" [input=scriptparsetree]`
- gsc decompiler, same as the disassembler, not any better, some structures are missing. `gsc gscinfo -g -o "output" [input=scriptparsetree]`

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

## Credits

- Serious's t8-compiler for some opcodes, it fasted up the process of understanding the internal game functions.
