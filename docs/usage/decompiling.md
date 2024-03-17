[Back...](../index.md)

# GSC Decompiling

**Table of Contents**

- [GSC Decompiling](#gsc-decompiling)
  - [Find scripts from different games](#find-scripts-from-different-games)
    - [Black Ops 4](#black-ops-4)
    - [Black Ops Cold War](#black-ops-cold-war)
    - [Other cod games](#other-cod-games)
  - [Decompiling](#decompiling)
    - [Decompiler options](#decompiler-options)


ACTS start purpose is to decompile GSC scripts.

## Find scripts from different games

### Black Ops 4

The tool `wps` can dump loaded Black Ops 4 gsc scripts into the directory `scriptparsetree`.

### Black Ops Cold War

The tool `wpscw` can dump loaded Black Ops Cold War gsc scripts into the directory `scriptparsetree_cw`.

Unlike with Black Ops 4, the string decryption function isn't implemented in acts. To decrypt the strings, you need first to create a directory with the name `scriptparsetree_cw` inside your cw install with all the dumped scripts.

Then start the game and run this acts mod:

```pwsh
acts mod t9dll DLL_DecryptGSCScripts
```

It'll use the game to decrypt the strings.

Once this is done use the `scriptparsetree_cw_decrypt` as your dump.

### Other cod games

To dump from other cod games, you can't use ACTS, but if you know the C/C++ and know how to find the XAsset pool for the gsc script (or scriptparsetree). The struct is more or less:

```c
struct GscEntry
{
	uint64_t name;
	uint32_t len;
	uint32_t __pad0c;
	uint8_t* buffer;
};
```

len is the buffer size to dump.

## Decompiling

The base command to decompile the scripts is:

```pwsh
acts gscinfo -g [file]+ (-o [output])
```

If you are using a iw based game, you need to specify the game using `--vm`

### Decompiler options

- `--dumpstrings [file]` : Dump all the extracted strings into file at the end of the decompilation
- `-a` : Produce assembly code with the decompilation
- `-H` : Dump the script header
- `-m [file]` : Dump the extracted hashes into file at the end of the decompilation
- `-l` : Write the decompiled location inside functions
