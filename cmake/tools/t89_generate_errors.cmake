add_executable(abp_t89_error_generator tools/t89_error_generator.cpp)

set_target_properties(abp_t89_error_generator PROPERTIES
    OUTPUT_NAME "abp_t89_error_generator"
    FOLDER "Tools"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp
    COMMAND abp_t89_error_generator
            ${CMAKE_SOURCE_DIR}/config/t89_errors.tsv
            ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp
    DEPENDS abp_t89_error_generator ${CMAKE_SOURCE_DIR}/config/t89_errors.tsv
    COMMENT "Generating error header from TSV"
)

add_custom_target(t89_generate_errors DEPENDS ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp)

set_target_properties(t89_generate_errors PROPERTIES
    OUTPUT_NAME "t89_generate_errors"
    FOLDER "Tools"
)
