add_executable(abp_t89_error_generator tools/t89_error_generator.cpp)

set_target_properties(abp_t89_error_generator PROPERTIES
    OUTPUT_NAME "abp_t89_error_generator"
    FOLDER "Tools"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp
    COMMAND abp_t89_error_generator
            ${CMAKE_SOURCE_DIR}/config/errors_t89.tsv
            ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp
    DEPENDS abp_t89_error_generator ${CMAKE_SOURCE_DIR}/config/errors_t89.tsv
    COMMENT "Generating t89 error header from TSV"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo6/t10_errors_generated.hpp
    COMMAND abp_t89_error_generator
            ${CMAKE_SOURCE_DIR}/config/errors_t10.tsv
            ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo6/t10_errors_generated.hpp
    DEPENDS abp_t89_error_generator ${CMAKE_SOURCE_DIR}/config/errors_t10.tsv
    COMMENT "Generating t10 error header from TSV"
)

add_custom_target(t89_generate_errors DEPENDS ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp)
add_custom_target(t10_generate_errors DEPENDS ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo6/t10_errors_generated.hpp)

set_target_properties(t89_generate_errors PROPERTIES
    OUTPUT_NAME "t89_generate_errors"
    FOLDER "Tools"
)

set_target_properties(t10_generate_errors PROPERTIES
    OUTPUT_NAME "t10_generate_errors"
    FOLDER "Tools"
)
