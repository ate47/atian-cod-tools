project "libcurl"
    language "C"
    kind "StaticLib"
    warnings "Off"
    characterset "MBCS"

    targetname "libcurl"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/curl/lib/**.c",
        ROOT .. "/deps/curl/lib/**.h",
    }

    defines {
        "CURL_STRICTER",
        "CURL_STATICLIB",
        "CURL_DISABLE_DICT",
        "CURL_DISABLE_FILE",
        "CURL_DISABLE_LDAP",
        "CURL_DISABLE_LDAPS",
        "CURL_DISABLE_FTP",
        "CURL_DISABLE_GOPHER",
        "CURL_DISABLE_IMAP",
        "CURL_DISABLE_MQTT",
        "CURL_DISABLE_POP3",
        "CURL_DISABLE_RTSP",
        "CURL_DISABLE_SMTP",
        "CURL_DISABLE_SMB",
        "CURL_DISABLE_TELNET",
        "CURL_DISABLE_TFTP",
        
        "BUILDING_LIBCURL",

        "USE_SCHANNEL",
        "USE_WINDOWS_SSPI",
        "USE_THREADS_WIN32",
    }

    links { "Crypt32.lib" }

    includedirs {
        ROOT .. "/deps/curl/lib",
        ROOT .. "/deps/curl/include"
    }

