# T8 Tools

Tools for Black ops 4

## Tools

- dump linked events `gsc dfunc [output=func.csv]` (need the game started)
- dump api functions `gsc devents [output=events.csv]` (need the game started)
- dump pooled scripts `gsc dps [output=pool.csv]` (need the game started)
- dump linked scripts `gsc dls [output=linked.csv]` (need the game started)
- dump linked/pooled scripts on disk `gsc wps [output=scriptparsetree]` (need the game started)
- gsc disassembler, made in 3 days with a lot of alcohol so don't use it. `gsc gscinfo -a -o "output" [input=scriptparsetree]`

## Lookup

To have a lookup over the extracted hashes, you can use a file named `strings.txt` when using the process, it will be loaded automatically, one string per line.

- Serious's t8-compiler for some opcodes, it fasted up the process of understanding the internal game functions.
