# vcpkg dependencies
if (NOT NO_QT_BUILD)
    find_package(Qt6 REQUIRED COMPONENTS Core)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    find_package(Qt6 REQUIRED COMPONENTS Concurrent)
endif()

if (NOT NO_OPEN_CL_BUILD)
    find_package(OpenCL REQUIRED)
endif()

find_package(antlr4-runtime CONFIG REQUIRED)
if(NOT TARGET antlr4-runtime)
	add_library(antlr4-runtime ALIAS antlr4_shared)
endif()

find_package(asmjit CONFIG REQUIRED)
if(NOT TARGET asmjit)
	add_library(asmjit ALIAS asmjit::asmjit)
endif()

find_path(DETOURS_INCLUDE_DIR detours/detours.h REQUIRED)
set(DETOURS_TRIPLET_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
if(NOT EXISTS "${DETOURS_TRIPLET_ROOT}")
	set(DETOURS_TRIPLET_ROOT "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
endif()
find_library(DETOURS_LIBRARY_RELEASE
	NAMES detours
	PATHS "${DETOURS_TRIPLET_ROOT}/lib"
	NO_DEFAULT_PATH
	REQUIRED
)
find_library(DETOURS_LIBRARY_DEBUG
	NAMES detours
	PATHS "${DETOURS_TRIPLET_ROOT}/debug/lib"
	NO_DEFAULT_PATH
	REQUIRED
)
if(NOT TARGET detours)
	add_library(detours STATIC IMPORTED GLOBAL)
	set_target_properties(detours PROPERTIES
		IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
		IMPORTED_LOCATION_DEBUG "${DETOURS_LIBRARY_DEBUG}"
		IMPORTED_LOCATION_RELEASE "${DETOURS_LIBRARY_RELEASE}"
		INTERFACE_INCLUDE_DIRECTORIES "${DETOURS_INCLUDE_DIR}"
	)
endif()

find_package(glfw3 CONFIG REQUIRED)

find_package(imgui CONFIG REQUIRED)
if(NOT TARGET imgui)
	add_library(imgui ALIAS imgui::imgui)
endif()

find_package(lz4 CONFIG REQUIRED)
if(NOT TARGET lz4)
	add_library(lz4 ALIAS lz4::lz4)
endif()

find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)

find_package(xxHash CONFIG REQUIRED)
if(NOT TARGET xxhash)
	add_library(xxhash ALIAS xxHash::xxhash)
endif()

find_package(ZLIB REQUIRED)
if(NOT TARGET zlib)
	add_library(zlib ALIAS ZLIB::ZLIB)
endif()

find_package(zstd CONFIG REQUIRED)
if(NOT TARGET zstd)
	add_library(zstd ALIAS zstd::libzstd)
endif()

find_package(libuv CONFIG REQUIRED)
find_package(llhttp CONFIG REQUIRED)
