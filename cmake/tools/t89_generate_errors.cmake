add_executable(abp_t89_error_generator tools/t89_error_generator.cpp)

set_target_properties(abp_t89_error_generator PROPERTIES
    OUTPUT_NAME "abp_t89_error_generator"
    FOLDER "Tools"
)

# add a target to generated an error list
# target: target to create
# in:     input tsv
# out:    output files to generated
macro(add_error_gen_target target in out)
    message(STATUS "Create error target ${target} ${in} -> ${out}")
    add_custom_command(
        OUTPUT ${out}
        COMMAND abp_t89_error_generator ${in} ${out}
        DEPENDS abp_t89_error_generator ${in}
        COMMENT "Generating error from TSV ${in} to ${out}"
    )
    
    add_custom_target(${target} DEPENDS ${out})

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${target}"
        FOLDER "Tools/Generated"
    )
    
    add_format_excluded("${out}")
endmacro(add_error_gen_target)
