# Guidelines for this project

This directory is a multi-project repository containing C++ utilities for Windows, built with CMake. The project is called atian-cod-tools (ACTS).

The architecture is:

- Shared library (`src/core/shared/`): common static library functionality shared by the projects. Use it for common functionality.
  - Common ACTS library (`src/core/acts/`): library containing the UI and CLI utilities. It is
    normally built as a DLL; the `STATIC_ACTS` CMake option can build it statically.
    - ACTS CLI executable (`src/exe/cli/`): loads the common library and CLI tools.
  - Common UI DLL (`src/core/acts-ui/`): dynamic library containing the UI widgets and UI code.
    - ACTS UI executable (`src/exe/ui3/`): loads the common UI library and UI tools.
- BO3 DLL (`src/dll/bo3-dll/`): dynamic library loaded by Call of Duty: Black Ops III.
- BO4 DLL (`src/dll/shield-plugin/`): dynamic library loaded by Call of Duty: Black Ops 4.
- BOCW DLL (`src/dll/bocw-dll/`): dynamic library loaded by Call of Duty: Black Ops Cold War.

The `build/` directory contains generated CMake build files and artifacts. Do not edit files in
`build/` manually. CMake also generates version-related headers in the source tree; do not edit
those generated files manually either. Make source or CMake changes in the repository instead.

## Format

The code is formatted using clang-format. The CMake targets `format` and `check-format` can be used to format and validate the whole project.

Use `cmake --build build --target format` to format the project and `cmake --build build --target check-format` to check formatting without changing files.

## Setup and Build

Run `scripts/setup.ps1` from the repository root to initialize submodules, install packages from
`packages.txt` through vcpkg, copy required data, and generate the build files. The generated
Visual Studio solution and project files are placed under `build/`; do not edit them directly.

Use the generated build system or Visual Studio to build the required configuration. CMake options
and setup behavior are defined by the root `CMakeLists.txt` and `scripts/setup.ps1`.

Qt6 is required for UI development. It can be disabled with the `-noQT` setup option. OpenCL is
required for OpenCL functionality and can be disabled with the `-noOpenCL` setup option. The `-ci`
option defines `CI_BUILD` for project code that needs different behavior in CI builds.

The project is MIT-licensed by default and must not contain GPL code in the normal build. The
`-gpl` setup option defines `GPL_BUILD` for explicitly configuring a GPL build. Use this option
only when GPL code is intentionally being developed; be careful not to introduce GPL code into the
default MIT-licensed project.

The `include/` directory contains the C SDK headers for the common and common UI DLLs. Headers
whose names end with `_ui` belong to the common UI DLL; the other headers belong to the common DLL.
The common UI DLL may depend on the common DLL, but the common DLL must not depend on the common UI
DLL.

## ACTS CLI

The command-line executable uses this structure:

```text
acts.exe (global options) tool (tool parameters)
```

Global options must appear before the tool name. Common options include:

- `--log [l]`: set the log level. `l` can be `p` (trace with source path), `t` (trace), `d`
  (debug), `i` (info), `w` (warning), or `e` (error).
- `--debug-data`: enable debug data collection so that, when the project crashes, it dumps a
  stack trace and register values.

For the complete list of global options, run `acts.exe --help`.

## Shared Library

### Logging

For logging, different levels are available (Error, Warning, Info, Debug, and Trace), each with a
macro. The parameters use the same format syntax as `std::format(fmt, ...)`; the level
configuration is set by the project. `core/logs.hpp` is automatically added in
`includes_shared.hpp`, included by all projects.

```cpp
LOG_ERROR("Error message {}", 42);
LOG_WARNING("Warning {}:{}", "foo", "bar");
LOG_INFO("Info");
LOG_DEBUG("Debug");
LOG_TRACE("Trace");
```

### Byte Buffers

To read buffers, the `core::bytebuffer` module provides different functions.

- `core/bytebuffer.hpp`: read buffers (`void*`, `size_t`)
- `core/bytebuffer_file.hpp`: read file streams (`std::ifstream`)

### Compression

The module `utils/compress_utils.hpp` provides functions for working with compressed buffers.
It supports lz4, zlib, deflate, gzip, zstd and oodle.

```cpp
// Compress data, destSize is updated to the new size, return success
bool Compress(CompressionAlgorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize);
// Decompress data, return the decompressed size or error if < 0
int Decompress2(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize);
```

### Process and Memory

The shared library contains utilities for hooking and modifying memory in the current process. They are in the `hook` directory.

- `error.hpp`: error handling
- `library.hpp`: functions to manipulate HMODULEs
- `memory.hpp` and `process.hpp`: functions to manipulate processes and memory
- `module_mapper.hpp`: functions to load an executable or DLL in memory and use its functions
- `scan_container.hpp`: utilities to cache scans for a library

The `utils/memapi.hpp` module provides functions for controlling the memory of other processes.

### Other Utilities

- `cli/cli_options.hpp`: utility to create CLI options
- `core/config.hpp`: JSON configuration functions
- `core/memory_allocator.hpp`: basic memory allocator
- `utils/utils.hpp`: common functions to read and write files and handle strings
- `utils/data_utils.hpp`: functions to render data
- `utils/hash_mini.hpp`: hash functions used by Call of Duty

## Common DLL

The `tools/` directory contains the CLI utilities used by ACTS CLI. A tool is defined as:

```cpp
int my_tool(int argc, const char* argv[]) {
    // Require one tool-specific argument. argv[0] is the executable name,
    // argv[1] is the tool name, and tool-specific arguments start at argv[2].
    if (tool::NotEnoughParam(argc, 1)) {
        // BAD_USAGE requests the tool usage, for example: "acts my_tool [name]"
        return tool::BAD_USAGE;
    }
    LOG_INFO("hello {}", argv[2]);
    return tool::OK; // tool::OK, tool::BAD_USAGE, or tool::BASIC_ERROR
}

// tool name, category, params, description, handler
ADD_TOOL(my_tool, "dev", " [name]", "my custom tool", my_tool);
```

## Common UI DLL

The UI uses Qt 6 and an MDI area with widgets acting as UI tools. An option bar can be used to open
the widgets. The widgets are registered in the `widgets/` directory using this macro:

```cpp
// widget type, name, menu path (null for no menu option), file extensions, allowDupe,
// needsInitialization
ADD_UI_TOOL(MyToolWidget, "My Tool", "Dev/My Tool", nullptr, false, false);
```

To open a file, a widget needs to have a method with this signature:
```cpp
void LoadFile(const QString& path);
```

## Repository Rules

- Make source changes under `src/`, `include/`, `cmake/`, or `deps/` as appropriate.
- Do not manually edit generated files under `build/`.
- Do not commit build outputs, intermediate files, or generated Visual Studio project files.
- Prefer existing shared utilities over adding duplicate implementations.
- Keep changes focused on the requested feature or fix.

## Validation

After making C++ changes:

1. Build the affected configuration.
2. Run the `check-format` target.