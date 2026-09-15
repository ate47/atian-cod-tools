# GSC Language Support

VS Code syntax and editor support for the GSC family of scripts used by Acts.

## Supported files

- `.gsc` GSC scripts
- `.csc` CSC scripts
- `.gsh` GSC header/include scripts

## Features

- Syntax highlighting for directives such as `#using`, `#namespace`, `#define`, and `#constexpr`.
- Highlighting for functions, detours, hashes, paths, strings, numbers, control flow, and operators from `grammar/gsc.g4`.
- GSC developer blocks using `/# ... #/`.
- Bracket, quote, and comment behavior.
- Connects to the native Acts GSC Language Server over stdio.

The native server is expected to implement the Language Server Protocol and provide completion, hover, symbols, diagnostics, and compiler integration. The extension does not duplicate parsing or invoke `gscc` directly.

## Language server configuration

By default, the extension starts:

```text
acts language-server
```

Configure another executable or extra arguments in VS Code settings:

```json
{
  "acts.languageServerPath": "N:\\path\\to\\acts.exe",
  "acts.languageServerArgs": ["language-server", "--stdio"],
  "acts.languageServerCwd": "N:\\path\\to\\project"
}
```

## Development

```bash
npm install
npm run compile
npm test
```

Press `F5` in VS Code to launch an Extension Development Host.
