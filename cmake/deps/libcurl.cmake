# libcurl
file(GLOB_RECURSE LIBCURL_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/curl/lib/*.h"
    "${CMAKE_SOURCE_DIR}/deps/curl/lib/*.c"
)
add_library(libcurl STATIC)
target_sources(libcurl PRIVATE ${LIBCURL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/curl"
    FILES ${LIBCURL_SOURCES}
)
target_compile_options(libcurl PRIVATE /W0)
set_target_properties(libcurl PROPERTIES
    OUTPUT_NAME "libcurl"
    FOLDER "Dependencies"
)

target_include_directories(libcurl PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/curl/lib"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
)
append_common_defs(libcurl C)
target_compile_definitions(libcurl PRIVATE 
    CURL_STRICTER
    CURL_STATICLIB
    CURL_DISABLE_DICT
    CURL_DISABLE_FILE
    CURL_DISABLE_LDAP
    CURL_DISABLE_LDAPS
    CURL_DISABLE_FTP
    CURL_DISABLE_GOPHER
    CURL_DISABLE_IMAP
    CURL_DISABLE_MQTT
    CURL_DISABLE_POP3
    CURL_DISABLE_RTSP
    CURL_DISABLE_SMTP
    CURL_DISABLE_SMB
    CURL_DISABLE_TELNET
    CURL_DISABLE_TFTP
    
    BUILDING_LIBCURL

    USE_SCHANNEL
    USE_WINDOWS_SSPI
    USE_THREADS_WIN32
)

target_link_libraries(libcurl PRIVATE
    Crypt32.lib
)

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/deps/curl/include
    DESTINATION include
)
install_common_defs(libcurl "")