---
name: gsc-translate-opcodes
description: Translate the GSC VM opcode ids from an old revision to a new one (opcodes shuffled after a VM update) using the ACTS MCP tools. Use when the user wants to port/recover opcode values for a new VM revision, find missing/unknown opcodes in new compiled scripts, or run gsct and then finish the remaining opcodes by hand.
---

# Translate GSC opcodes between two VM revisions

A VM revision usually adds an opcode or changes small things, but shuffles the opcode ids
(`OPCODE_Add=0x46` becomes `0x89`). The scripts stay nearly identical, so the same logic compiled
in the old and new dumps differs mostly by opcode ids. The goal is a complete, verified opcode
table for the new revision.

All the work is done through the `acts` MCP tools (the server must be connected; if it is not,
tell the user). Do not read the C++ sources or run `acts.exe` to find out how a tool behaves:
the tool errors below explain the usual causes.

## Platform rule (read first)

Old and new opcodes are both stored under the **platform the user names** (for example `mw4`),
not necessarily `pc`. `vm` is the `internalName` from `list_gsc_vm`, `platform` an id from
`list_gsc_platforms`. Before anything else, **dump the old vm's table for that exact platform**
(`dump_gsc_vm_opcodes` with `compact=true`, or `get_gsc_vm_opcodes`) to confirm it is populated.
Reading the table of another platform (e.g. `bo7` instead of `mw4`) silently gives wrong base
values. The read tools now fail when the platform isn't defined for the vm; do not work around
this by switching platform, ask the user.

## Tools

| Tool | Use |
|---|---|
| `list_gsc_vm`, `get_gsc_vm_info`, `list_gsc_platforms` | vms, platforms, `maxOpCode`, flags (u16 opcodes?) |
| `get_gsc_vm_opcodes`, `dump_gsc_vm_opcodes` | current values; dump (`compact=true`) lists the **unused** values below the highest used one |
| `list_gsc_opcode_names` | valid opcode names (optional `filter`, case insensitive) |
| `translate_gsc_opcodes` | automatic pass (gsct). Returns `mappings`, `conflicts`, `unmapped`; `apply=<platform>` registers the clean mappings directly |
| `decompile_gsc_file` | `mode=status` + `summary=true` + `outputFile` to find errors; `mode=asm` + `useInternalNames=true` to read; `exports` restricts to functions |
| `compare_gsc_function` | the same function in the old and new dumps, only the differing hunks (replaces two asm calls) |
| `search_gsc_scripts`, `get_gsc_script_data` | find the same script/function in both dumps |
| `set_gsc_vm_opcodes`, `remove_gsc_vm_opcodes` | register / undo values (in memory in the MCP server) |

## Inputs to get from the user (ask only for what is missing)

- Old dump directory and new dump directory (compiled `.gscc/.cscc/.gscbin/.cscbin`; the vm is
  read from each file magic).
- The platform (see the platform rule) and the vm internal names if ambiguous.
- Where to save the final table (file for `dump_gsc_vm_opcodes` `output`).

## Workflow

### 1. Automatic pass

1. `list_gsc_vm` / `get_gsc_vm_info` for the old and new vm. Note `maxOpCode` and whether the
   opcodes are u16 (several values may map to one opcode) or byte (one value per opcode).
2. Dump the old vm table for the platform (platform rule).
3. `translate_gsc_opcodes` with `old`, `new`, `platform` and `apply=<platform>`. Nothing needs
   to be retyped: the mappings without conflict are registered. Then review:
   - `conflicts`: opcodes with several values in a byte vm, or two opcodes on the same value
     (reported with `sharedWith`). They are **not** applied: resolve them in step 3.
   - `unmapped`: opcodes of the old vm with no new value. These are the work list for step 3
     (some may not exist in the new vm anymore).

### 2. Find what is still missing

1. `decompile_gsc_file` on the new dump with `mode=status`, `summary=true`,
   `outputFile=<file>` (the full result goes to the file, the answer only has totals; the
   `groups` of the summary are also in the file). Work from the groups
   (`message`, `count`, sample `scripts`/`functions`), not from the per-file list.
2. `dump_gsc_vm_opcodes` with `compact=true` for the new vm: the Undefined line is the pool of
   values still unassigned.
3. Rank the unknown values: seen in the most scripts / in the smallest functions first.

### 3. Manual inference by control flow (the core of the skill)

For each unknown new value `Z2` (or each `unmapped` / `conflicts` opcode):

1. Pick a small function containing it and call `compare_gsc_function` (`old`, `new`,
   `function`, `namespace`, `script`, `platform`, `newPlatform`). It returns only the differing
   hunks with the matching lines `before`/`after`. Use `full=true` only if the hunks are not
   enough. When several functions need checking, send the calls in **one parallel message**.
2. Read each hunk. The known opcodes around it are the anchors, with identical operands
   (constants, strings, hashes, jump targets):

   ```
   old: OPCODE_GetUnsignedInteger(XXX) 123456      new: OPCODE_GetUnsignedInteger(XXX2) 123456
        OPCODE_GetByte(YYY)            12               OPCODE_GetByte(YYY2)            12
        OPCODE_Add(ZZZ)                                 OPCODE_Unknown(ZZZ2)
   => candidate: ZZZ2 = OPCODE_Add
   ```

   A hunk with `sameSize=true` and an unknown opcode on the new side is a candidate pair.
3. Only accept a candidate when all of this holds:
   - the surrounding anchors (before and after) are identical, and the operand layout/size
     of the unknown instruction fits the old opcode (same bytes consumed, same jump targets);
   - the value is currently unassigned (in the Undefined pool and `< maxOpCode`);
   - the candidate is **confirmed by at least two independent places** (different functions or
     scripts) giving the same result, unless the anchors leave no other possibility;
   - no other opcode already has this value (byte vm).
4. If a function is ambiguous (several unknown ops in a row, a new opcode that does not exist in
   the old one, a rewritten function), skip it and try another one. Never assign by
   elimination alone without saying so.
5. Opcodes that exist only in the new vm cannot be matched: report them as unresolved with the
   functions where they appear.

### 4. Apply and verify

1. `set_gsc_vm_opcodes` for the confirmed batch (names from `list_gsc_opcode_names`, case
   insensitive).
2. Re-run `decompile_gsc_file` `mode=status` `summary=true`: the unknown-opcode errors for the
   assigned values must disappear and **no new group** may appear.
3. `decompile_gsc_file` / `compare_gsc_function` on the evidence functions (and a few others
   using the opcode): same logic, same constants, same control flow. A different structure
   means a wrong assignment.
4. If a verification fails, undo with `remove_gsc_vm_opcodes` for those values, re-evaluate the
   anchors, and continue.
5. Repeat steps 2-4: each round unlocks new anchors, because the more opcodes are known, the
   more functions become readable.

### 5. Stop rules

- Stop and report (do not hunt further) when the remaining status errors are values that
  **also fail in the old dump**, or opcodes unknown in both the old and the new dumps: they
  cannot be resolved from the base.
- Stop on a contradiction with an already verified value: show both pieces of evidence.

### 6. Save and report

1. `dump_gsc_vm_opcodes` with `output` (actsop csv format) to persist the table, and with `dumpType=cpp` to get the
   `vminfo->RegisterOpCode(PLATFORM_X, OPCODE_Y, 0x..);` lines to paste into the opcode registry
   (`src/core/acts/tools/gsc/opcodes/`). The values set
   with `set_gsc_vm_opcodes` / `apply` only live in the MCP server memory and are lost on restart.
2. Report to the user:
   - opcodes translated automatically vs by hand (with the evidence function for each manual one);
   - unresolved opcodes / values still in the Undefined pool, and the scripts that still error;
   - any conflict or low-confidence assignment that needs their review.

## Tool errors

- `Unknown opcode '...'` with a name from `list_gsc_opcode_names`: names are matched case
  insensitively now; if it still fails, the name really doesn't exist, check the spelling.
- `Platform '...' isn't defined for vm '...'`: wrong platform name for this vm (the message
  lists the defined ones). Don't guess by trial and error: ask, or use `list_gsc_platforms` and
  the vm's `platforms` in `list_gsc_vm`.
- A generic `Decompilation error, check the platform and the vm opcodes` with an empty
  `errors[]`: the opcode table of the platform is empty or wrong for that vm. Re-check the
  platform rule before anything else.
- `Can't map vm with different op sizes` / `Multiple magic detected`: the two dump directories
  don't contain a single vm each, or the vms really differ (u16 vs byte).

## Working with big outputs

- Use `outputFile` and `summary` for status runs; the full status of a few hundred scripts is
  too big to read inline.
- If a file must be processed, use Python (`python -I`), `jq` is not installed on this machine.
- Prefer small functions and `compare_gsc_function` over reading two full asm listings.

## Rules

- Never mark an assignment as certain without the verification of step 4.
- Do not overwrite values already verified.
- Keep the user informed of the progress (assigned count, remaining unknown values).
