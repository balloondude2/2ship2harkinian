include_guard(GLOBAL)

include(FetchContent)
include(CMakeParseArguments)

function(vosk_import)
    set(options)
    set(oneValueArgs VERSION)
    cmake_parse_arguments(VOSK "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT VOSK_VERSION)
        set(VOSK_VERSION "0.3.45")
    endif()

    #
    # Runtime library
    #

    if(WIN32)

        set(VOSK_ARCHIVE
            "vosk-win64-${VOSK_VERSION}.zip")

    elseif(APPLE)

        set(VOSK_ARCHIVE
            "vosk-osx-${VOSK_VERSION}.zip")

    elseif(UNIX)

        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            message(FATAL_ERROR
                "Linux ARM64 binaries are not currently supported.")
        endif()

        set(VOSK_ARCHIVE
            "vosk-linux-x86_64-${VOSK_VERSION}.zip")

    else()

        message(FATAL_ERROR "Unsupported platform")

    endif()

    FetchContent_Declare(
        vosk
        URL https://github.com/alphacep/vosk-api/releases/download/v${VOSK_VERSION}/${VOSK_ARCHIVE}
    )

    #
    # Speech models
    #

    FetchContent_Declare(
        vosk_model_en
        URL https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip
    )

    FetchContent_Declare(
        vosk_model_ja
        URL https://alphacephei.com/vosk/models/vosk-model-small-ja-0.22.zip
    )

    FetchContent_MakeAvailable(
        vosk
        vosk_model_en
        vosk_model_ja
    )

    #
    # Export model paths
    #

    set(VOSK_MODEL_EN
        "${vosk_model_en_SOURCE_DIR}"
        CACHE INTERNAL "")

    set(VOSK_MODEL_JA
        "${vosk_model_ja_SOURCE_DIR}"
        CACHE INTERNAL "")

    #
    # Includes
    #

    find_path(
        VOSK_INCLUDE_DIR
        NAMES vosk_api.h
        PATHS ${vosk_SOURCE_DIR}
        NO_DEFAULT_PATH
    )

    if(NOT VOSK_INCLUDE_DIR)
        message(FATAL_ERROR
            "Could not locate vosk_api.h")
    endif()

    #
    # Runtime library
    #

    if(WIN32)

        find_file(
            VOSK_DLL
            NAMES libvosk.dll
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        find_file(
            VOSK_IMPLIB
            NAMES libvosk.lib
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_DLL}"
            IMPORTED_IMPLIB "${VOSK_IMPLIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    elseif(APPLE)

        find_file(
            VOSK_LIB
            NAMES libvosk.dylib
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    else()

        find_file(
            VOSK_LIB
            NAMES libvosk.so
            PATHS ${vosk_SOURCE_DIR}
            NO_DEFAULT_PATH)

        add_library(vosk::vosk SHARED IMPORTED GLOBAL)

        set_target_properties(vosk::vosk PROPERTIES
            IMPORTED_LOCATION "${VOSK_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}"
        )

    endif()

    #
    # Install runtime
    #

    install(
        IMPORTED_RUNTIME_ARTIFACTS vosk::vosk
        DESTINATION .
        COMPONENT 2s2h
    )

    #
    # Install speech models
    #

    install(
        DIRECTORY "${VOSK_MODEL_EN}/"
        DESTINATION "vosk/vosk-model-small-en-us-0.15"
        COMPONENT 2s2h
    )

    install(
        DIRECTORY "${VOSK_MODEL_JA}/"
        DESTINATION "vosk/vosk-model-small-ja-0.22"
        COMPONENT 2s2h
    )

endfunction()