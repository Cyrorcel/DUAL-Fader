cmake_minimum_required(VERSION 3.22)

project(DualFader VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Télécharge JUCE automatiquement (nécessite git et un accès réseau lors de la configuration).
# Tu peux changer le tag, ou remplacer ce bloc par add_subdirectory(chemin/vers/JUCE)
# si tu as déjà JUCE sur ton disque.
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DualFader
    COMPANY_NAME "MonStudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Mnst
    PLUGIN_CODE Dfdr
    FORMATS VST3
    PRODUCT_NAME "DualFader"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(DualFader PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(DualFader PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DualFader
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
