cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME "Distortion")

set(FORMATS AU VST3 AUv3 Standalone)

file(STRINGS VERSION CURRENT_VERSION)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

set(CMAKE_XCODE_GENERATE_SCHEME OFF)

# set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Support macOS down to High Sierra")

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

add_subdirectory(3rdparty/JUCE build)

# Check the readme at `docs/CMake API.md` in the JUCE repo for full config
# https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md
juce_add_plugin("${PROJECT_NAME}"
        VERSION "0.0.0"
        ICON_BIG "logo2.png"
        ICON_SMALL "logo2.png"
        COMPANY_NAME "distortion"
        COMPANY_WEBSITE "https://distortion.de"
        COMPANY_EMAIL "info@distortion.de"
        BUNDLE_ID de.distortion.distortion
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
        PLUGIN_MANUFACTURER_CODE KrPd               # This has to be one uppercase, rest lower for AU formats
        PLUGIN_CODE D00B                            # A unique four-character plugin id with at least one upper-case character
        FORMATS "${FORMATS}"
        PRODUCT_NAME "${PROJECT_NAME}")        # The name of the final executable, which can differ from the target name


target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

set(SourceFiles

        src/inc/CustomRotaryDial.h
        src/inc/LookAndFeel.h
        src/inc/VuMeter.h
        src/UiElements.h
        src/DistortionConstants.h
        src/DistortionEditor.h
        src/DistortionProcessor.h
        src/DistortionProcessor.cpp
)

target_include_directories("${PROJECT_NAME}" PUBLIC src/dsp)

target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

source_group(TREE ../src PREFIX "" FILES ${SourceFiles})

juce_add_binary_data(Assets SOURCES logo.png)

# Required for Linux happiness:
# See https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

foreach (target ${FORMATS} "All")
    if (TARGET ${PROJECT_NAME}_${target})
        set_target_properties(${PROJECT_NAME}_${target} PROPERTIES
                FOLDER "Targets"

                XCODE_SCHEME_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app"

                XCODE_GENERATE_SCHEME ON)
    endif ()
endforeach ()
set_target_properties(Assets PROPERTIES FOLDER "Targets")

set(JUCE_DEPENDENCIES
        juce::juce_audio_utils
)

target_compile_definitions("${PROJECT_NAME}"
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0
)

target_link_libraries("${PROJECT_NAME}"
        PRIVATE
        Assets
        ${JUCE_DEPENDENCIES}
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Color our warnings and errors
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif ()

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(src/dsp/unittests)
endif()


