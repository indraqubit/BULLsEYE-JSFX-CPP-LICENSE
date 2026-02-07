cmake_minimum_required(VERSION 3.15)

# ========================================================================
# PROJECT CONFIGURATION
# ========================================================================

project(BULLsEYE VERSION 1.0.0)

# Add JUCE (from submodule or symlink)
# Option 1: Git submodule
# add_subdirectory(modules/JUCE)

# Option 2: Symlink to local JUCE
# ln -s /path/to/JUCE modules/JUCE
add_subdirectory(modules/JUCE)

# ========================================================================
# PLUGIN CONFIGURATION
# ========================================================================

juce_add_plugin(BULLsEYE
    COMPANY_NAME "IQ"
    PLUGIN_MANUFACTURER_CODE IQDE
    PLUGIN_CODE BULL
    BUNDLE_ID "com.iq.bullseye"
    FORMATS VST3 AU
    PRODUCT_NAME "BULLsEYE"
    DESCRIPTION "LUFS-I Gated Loudness Meter with True Peak Detection"
    VERSION 2.0.0
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER "IQ"
)

# ========================================================================
# SOURCE FILES
# ========================================================================

target_sources(BULLsEYE PRIVATE
    # Main files
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    # SSOT files
    Source/SSOT/ModelSSOT.h
    Source/SSOT/UISSOT.h
    Source/SSOT/ProcessorSSOT.h
    Source/SSOT/DSPSSOT.h

    # DSP
    Source/DSP/BULLsEYEProcessor.h

    # Components
    Source/Components/StatusDisplayComponent.cpp
    Source/Components/StatusDisplayComponent.h
    Source/Components/ModeSelectorComponent.cpp
    Source/Components/ModeSelectorComponent.h
    Source/Components/CircularMeterComponent.cpp
    Source/Components/CircularMeterComponent.h
)

# ========================================================================
# INCLUDE DIRECTORIES
# ========================================================================

target_include_directories(BULLsEYE PRIVATE
    Source
)

# ========================================================================
# COMPILE OPTIONS
# ========================================================================

target_compile_features(BULLsEYE PRIVATE cxx_std_17)

# ========================================================================
# PREPROCESSOR DEFINITIONS
# ========================================================================

target_compile_definitions(BULLsEYE PRIVATE
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

# ========================================================================
# LINK LIBRARIES
# ========================================================================

target_link_libraries(BULLsEYE PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_dsp
)

# ========================================================================
# PLATFORM-SPECIFIC SETTINGS
# ========================================================================

# macOS-specific settings
if(APPLE)
    # Universal binary support (Intel x86_64 + Apple Silicon ARM64)
    if(NOT DEFINED CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Target architectures")
    endif()
    
    # Set minimum macOS version (Monterey 12.0 through Tahoe 15.0)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")
    
    set_target_properties(BULLsEYE PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/build/macOS/Info.plist.in"
        MACOSX_BUNDLE_INFO_STRING "LUFS-I Gated Loudness Meter with True Peak Detection"
    )
    
    message(STATUS "Building for macOS ${CMAKE_OSX_DEPLOYMENT_TARGET}+ (${CMAKE_OSX_ARCHITECTURES})")
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(BULLsEYE PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# ========================================================================
# CUSTOM COMMANDS
# ========================================================================

# Copy built plugins to standard locations (optional)
# macOS VST3
add_custom_command(TARGET BULLsEYE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:BULLsEYE>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3"
    COMMENT "Installing VST3 to ~/Library/Audio/Plug-Ins/VST3/"
)

# macOS AU
add_custom_command(TARGET BULLsEYE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:BULLsEYE>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/Components/BULLsEYE.component"
    COMMENT "Installing AU to ~/Library/Audio/Plug-Ins/Components/"
)

# ========================================================================
# CODE SIGNING (OPTIONAL - For Distribution)
# ========================================================================
#
# The following CMake code demonstrates how to integrate code signing
# into your CMake build process. To enable, set the CMAKE_SIGNING_CERT
# variable or use -DCMAKE_SIGNING_CERT="Certificate Name" during build.
#
# For detailed instructions, see docs/CODE_SIGNING.md and NOTARIZATION.md
#

# Example: Add code signing as a post-build step (disabled by default)
# Uncomment and modify the following to enable CMake-based signing:
#
# if(APPLE AND DEFINED CMAKE_SIGNING_CERT)
#     add_custom_command(TARGET BULLsEYE POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E echo "Signing AU component..."
#         COMMAND codesign --force --sign "${CMAKE_SIGNING_CERT}" --deep
#             --timestamp --options=runtime
#             "$ENV{HOME}/Library/Audio/Plug-Ins/Components/BULLsEYE.component"
#         COMMAND ${CMAKE_COMMAND} -E echo "Signing VST3 component..."
#         COMMAND codesign --force --sign "${CMAKE_SIGNING_CERT}" --deep
#             --timestamp --options=runtime
#             "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/BULLsEYE.vst3"
#         COMMENT "Code signing plugin components with certificate: ${CMAKE_SIGNING_CERT}"
#     )
#
#     message(STATUS "Code signing enabled with certificate: ${CMAKE_SIGNING_CERT}")
# else()
#     message(STATUS "Code signing disabled. Set CMAKE_SIGNING_CERT to enable.")
# endif()
#
# Usage:
#   cmake -DCMAKE_SIGNING_CERT="Developer ID Application: Your Name" ..
#   cmake --build . --config Release
#

# ========================================================================
# TESTS CONFIGURATION
# ========================================================================

# Enable testing
enable_testing()

# Add tests directory
add_subdirectory(tests)
