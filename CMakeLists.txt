# Example Audio Plugin CMakeLists.txt

# To get started on a new plugin, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 4.0.0)

set (PROJECT_NAME "gemini-ii-modeling")
set (COMPANY_NAME "KonstantinKovalev")
set (CURRENT_VERSION "0.0.1")
set (PRODUCT_NAME "gemini-ii-modeling")
set (FORMATS Standalone AU VST3)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(gemini-ii-modeling VERSION 0.0.1)

add_subdirectory (modules/JUCE)

# Project configuration

set (CMAKE_CXX_STANDARD 20)

# Append custom module path
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/cmake-include/sudara-cmake")

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
# add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

# If you are building a VST2, CMake needs to be told where to find the SDK on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

# Define the plugin)
juce_add_plugin("${PROJECT_NAME}"    # Set this if the plugin version is different to the project version
COMPANY_NAME "${COMPANY_NAME}"    # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
BUNDLE_ID "${BUNDLE_ID}"
COPY_PLUGIN_AFTER_BUILD TRUE PLUGIN_MANUFACTURER_CODE Vdsp    # Specify the name of the plugin's author
PLUGIN_CODE Agii    # Is this a synth or an effect?
FORMATS ${FORMATS}    # Does the plugin need midi input?
PRODUCT_NAME "$PRODUCT_NAME}"    # Does the plugin need midi output?
NEEDS_MIDI_INPUT FALSE    # Is this plugin a MIDI effect?
NEEDS_MIDI_OUTPUT FALSE    # Does the editor need keyboard focus?
IS MIDI EFFECT FALSE    # Should the plugin be installed to a default location after building?
IS_SYNTH FALSE    # A four-character manufacturer id with at least one upper-case character
    # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
PRODUCT_NAME "GeminiIIModeling"         # The name of the final executable, which can differ from the target name
)

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(AudioPluginExample)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(gemini-ii-modeling
    PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp)



# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(gemini-ii-modeling
   PUBLIC
#         JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
#        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
#        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

add_compile_definitions (JUCE PROJECT_NAME="${PROJECT_NAME}")
# Define SharedCode as an INTERFACE library
add_library(SharedCode INTERFACE)
# Set compile features for SharedCode
target_compile_features (SharedCode INTERFACE xx_std_20)
# Include directories and compile definitions for SharedCode
target_include_directories(SharedCode INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/modules/JUCE/modules"
        "${CMAKE_BINARY_DIR}/AudioPluginData"
)

target_compile_definitions (SharedCode INTERFACE
        juce GLOBAL_MODULE_SETTINGS_INCLUDED=1
        juce WEB_BROWSER=1
        juce SECURL
        juce_VST3_CAN_REPLACE_VST2=0
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        VERSION="${CURRENT_VERSION}"
        juce DISPLAY_SPLASH_SCREEN=0
        PRODUCT_NAME_WITHOUT_VERSION="CometChannelStrip"
        X. 46
)
        # Add sources to the main project
        file (GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS

        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
        "${CMAKE_CURRENT_SOURCEDIR}/source/*.h"
)

# Sources to main project
target_sources ("${PROJECT_NAME}" PRIVATE ${SourceFiles})

# Assets setup

file (GLOB_RECURSE Assets"${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*")

target_link_libraries("${PROJECT_NAME}"
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_product_unlocking
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

