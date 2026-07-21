include_guard(GLOBAL)

include(FetchContent)

function(vosk_import)
    set(options)
    set(oneValueArgs VERSION)
    cmake_parse_arguments(VOSK "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT VOSK_VERSION)
        set(VOSK_VERSION "0.3.45")
    endif()

    #
    # Select release archive
    #
    if(WIN32)
        set(VOSK_ARCHIVE
            "vosk-win64-${VOSK_VERSION}.zip")

    elseif(APPLE)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
            # Official releases are universal.
            set(VOSK_ARCHIVE
                "vosk-osx-${VOSK_VERSION}.zip")
        else()
            set(VOSK_ARCHIVE
                "vosk-osx-${VOSK_VERSION}.zip")
        endif()

    elseif(UNIX)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            message(FATAL_ERROR
                "No official ARM64 Linux Vosk binary is currently handled by this module.")
        endif()

        set(VOSK_ARCHIVE
            "vosk-linux-x86_64-${VOSK_VERSION}.zip")

    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()

    set(VOSK_URL
        "https://github.com/alphacep/vosk-api/releases/download/v${VOSK_VERSION}/${VOSK_ARCHIVE}")

    FetchContent_Declare(
        vosk
        URL ${VOSK_URL}
    )

    FetchContent_MakeAvailable(vosk)

    #
    # Locate include directory
    #
    find_path(VOSK_INCLUDE_DIR
        NAMES vosk_api.h
        PATHS ${vosk_SOURCE_DIR}
        NO_DEFAULT_PATH
    )

    if(NOT VOSK_INCLUDE_DIR)
        message(FATAL_ERROR
            "Could not locate vosk_api.h after downloading Vosk.")
    endif()

    #
    # Locate library
    #
    if(WIN32)

        find_file(VOSK_DLL
            NAMES libvosk.dll
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        find_file(VOSK_IMPLIB
            NAMES libvosk.lib
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        if(NOT VOSK_DLL OR NOT VOSK_IMPLIB)
            message(FATAL_ERROR
                "Could not locate Vosk DLL/import library.")
        endif()

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_DLL}"
            IMPORTED_IMPLIB "${VOSK_IMPLIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    elseif(APPLE)

        find_file(VOSK_LIB
            NAMES libvosk.dylib
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        if(NOT VOSK_LIB)
            message(FATAL_ERROR
                "Could not locate libvosk.dylib")
        endif()

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    else()

        find_file(VOSK_LIB
            NAMES libvosk.so
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        if(NOT VOSK_LIB)
            message(FATAL_ERROR
                "Could not locate libvosk.so")
        endif()

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    endif()
endfunction()
