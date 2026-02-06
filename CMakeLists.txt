cmake_minimum_required(VERSION 3.15)

# ========================================================================
# PROJECT CONFIGURATION
# ========================================================================

# TODO: Replace with your plugin name
project(YourPluginName VERSION 1)

# Add JUCE (SOT - from submodule or symlink)
# Option 1: Git submodule
# add_subdirectory(modules/JUCE)

# Option 2: Symlink to local JUCE
# ln -s /path/to/JUCE modules/JUCE
add_subdirectory(modules/JUCE)

# ========================================================================
# PLUGIN CONFIGURATION
# ========================================================================

# TODO: Customize these values for your plugin
juce_add_plugin(YourPluginName
    COMPANY_NAME "Your Company"
    PLUGIN_MANUFACTURER_CODE YCTR  # TODO: Get from https://www Steinberg.net/en/company/developer.html
    PLUGIN_CODE YPCD              # TODO: 4-character code (unique per plugin)
    BUNDLE_ID "com.yourcompany.yourplugin"  # macOS bundle ID
    FORMATS VST3 AU                # VST3, AU, AAX (comment out if not needed)
    PRODUCT_NAME "Your Plugin Name"
    DESCRIPTION "Your plugin description"
    VERSION 1.0.0
    IS_SYNTH FALSE                # TRUE for instruments, FALSE for effects
    NEEDS_MIDI_INPUT FALSE        # TRUE if plugin accepts MIDI
    NEEDS_MIDI_OUTPUT FALSE       # TRUE if plugin produces MIDI
    IS_MIDI_EFFECT FALSE          # TRUE for MIDI-only plugins
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE  # Copies plugin to standard location after build
    PLUGIN_MANUFACTURER "Your Company"
)

# ========================================================================
# SOURCE FILES
# ========================================================================

# TODO: Add your source files here
target_sources(YourPluginName PRIVATE
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
    
    # Components
    Source/Components/StatusDisplayComponent.cpp
    Source/Components/StatusDisplayComponent.h
    Source/Components/ModeSelectorComponent.cpp
    Source/Components/ModeSelectorComponent.h
    Source/Components/MuteButtonComponent.cpp
    Source/Components/MuteButtonComponent.h
    Source/Components/InvertToggleComponent.cpp
    Source/Components/InvertToggleComponent.h
    Source/Components/ModeDescriptionComponent.cpp
    Source/Components/ModeDescriptionComponent.h
    
    # DSP
    Source/DSP/YourProcessor.h
)

# ========================================================================
# INCLUDE DIRECTORIES
# ========================================================================

target_include_directories(YourPluginName PRIVATE
    Source
)

# ========================================================================
# COMPILE OPTIONS
# ========================================================================

target_compile_features(YourPluginName PRIVATE cxx_std_17)

# ========================================================================
# PREPROCESSOR DEFINITIONS
# ========================================================================

# TODO: Add any necessary preprocessor definitions
target_compile_definitions(YourPluginName PRIVATE
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1  # For VST3 compatibility
    # Add more as needed
)

# ========================================================================
# LINK LIBRARIES
# ========================================================================

target_link_libraries(YourPluginName PRIVATE
    juce::juce_audio_utils      # For UI utilities
    juce::juce_audio_plugin_client  # For plugin hosting
    juce::juce_dsp               # For DSP utilities
)

# ========================================================================
# PLATFORM-SPECIFIC SETTINGS
# ========================================================================

# macOS-specific settings
if(APPLE)
    set_target_properties(YourPluginName PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/build/macOS/Info.plist.in"
    )
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(YourPluginName PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# ========================================================================
# CUSTOM COMMANDS
# ========================================================================

# Copy built plugins to standard locations (optional)
# macOS VST3
add_custom_command(TARGET YourPluginName POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:YourPluginName>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/YourPlugin.vst3"
    COMMENT "Installing VST3 to ~/Library/Audio/Plug-Ins/VST3/"
)

# macOS AU
add_custom_command(TARGET YourPluginName POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:YourPluginName>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/Components/YourPlugin.component"
    COMMENT "Installing AU to ~/Library/Audio/Plug-Ins/Components/"
)
