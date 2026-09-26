if(DEFINED CLANG_FORMAT_PATH)
    set(CLANG_FORMAT "${CLANG_FORMAT_PATH}")
else()
    find_program(CLANG_FORMAT
        NAMES clang-format clang-format.exe
    )
endif()

message(STATUS "CLANG_FORMAT=${CLANG_FORMAT}")

file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
    "src/*.cpp"
    "src/*.hpp"
    "examples/*.c"
    "examples/*.h"
    "include/*.h"
    "tools/*.cpp"
    "tools/*.hpp"
)

list(REMOVE_ITEM ALL_CXX_SOURCE_FILES ${EXCLUDED_FORMAT_FILE})


file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/cppfiles.txt"
    CONTENT "$<JOIN:${ALL_CXX_SOURCE_FILES},\n>")


add_custom_target(format
    COMMAND "${CLANG_FORMAT}" -i @${CMAKE_BINARY_DIR}/cppfiles.txt
)

set_target_properties(format PROPERTIES
    FOLDER "Utilities"
)

add_custom_target(check-format
    COMMAND "${CLANG_FORMAT}" --dry-run --Werror  @${CMAKE_BINARY_DIR}/cppfiles.txt
)
set_target_properties(check-format PROPERTIES
    FOLDER "Utilities"
)