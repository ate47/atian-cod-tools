add_executable(abp_game_config_packer 
    "${CMAKE_SOURCE_DIR}/tools/game_config_packer/main.cpp"
    "${CMAKE_SOURCE_DIR}/tools/game_config_packer/template.hpp.inja"
    "${CMAKE_SOURCE_DIR}/tools/game_config_packer/template.cpp.inja"
)
target_compile_options(abp_game_config_packer PRIVATE "/source-charset:windows-1252" /Zi)
set_target_properties(abp_game_config_packer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
set_target_properties(abp_game_config_packer PROPERTIES
    OUTPUT_NAME "abp_game_config_packer"
    FOLDER "Tools"
)

target_include_directories(abp_game_config_packer PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/nlohmann-json/single_include/"
    "${CMAKE_SOURCE_DIR}/deps/inja/include/"
)

# add a target to generated a packed sig reader
# target: target to create
# gameid: game to load in the config/data/games dir
# out:    output files to generated, will create .hpp.inl and .cpp.inl
# example: add_game_pack_target(AtianCodToolsBO4ShieldPluginGame bo4 "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin/data/bo4_generated")
macro(add_game_pack_target target gameid out)
    add_custom_command(
        OUTPUT "${out}.hpp.inl" "${out}.cpp.inl"
        COMMAND abp_game_config_packer
                "${CMAKE_SOURCE_DIR}/tools/game_config_packer/"
                "${gameid}"
                "${CMAKE_SOURCE_DIR}/config/data/games/${gameid}.json"
                "${out}"
        DEPENDS abp_game_config_packer 
                "${CMAKE_SOURCE_DIR}/config/data/games/${gameid}.json"
                "${CMAKE_SOURCE_DIR}/tools/game_config_packer/template.hpp.inja"
                "${CMAKE_SOURCE_DIR}/tools/game_config_packer/template.cpp.inja"
        COMMENT "Generating ${gameid} pack data into ${out}.cpp.inl"
    )
    add_custom_target(${target} DEPENDS "${out}.hpp.inl" "${out}.cpp.inl")
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${target}"
        FOLDER "Tools/Generated"
    )
    add_format_excluded("${out}.hpp.inl")
    add_format_excluded("${out}.cpp.inl")
endmacro(add_game_pack_target)
