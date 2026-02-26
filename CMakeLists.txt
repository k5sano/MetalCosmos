cmake_minimum_required(VERSION 3.22)
project(MetalCosmos VERSION 1.0.0)

# Universal Binary for macOS (arm64 + x86_64)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)

# JUCE はプロジェクトルートのサブディレクトリにソースが配置されている
add_subdirectory(JUCE)

juce_add_plugin(MetalCosmos
    PLUGIN_MANUFACTURER_CODE K5sn
    MANUFACTURER_NAME "K5SANO"
    PLUGIN_CODE Mt2p
    FORMATS VST3 Standalone
    PRODUCT_NAME "MetalCosmos"
    COMPANY_NAME "K5SANO"
    COMPANY_WEBSITE "https://github.com/k5sano"
    COMPANY_EMAIL "contact@k5sano.com"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    VST3_CATEGORIES "Fx" "Distortion"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(MetalCosmos)

target_sources(MetalCosmos PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_include_directories(MetalCosmos PRIVATE
    Source
    scaffold
)

target_compile_features(MetalCosmos PRIVATE cxx_std_17)

if(MSVC)
    target_compile_definitions(MetalCosmos PRIVATE _USE_MATH_DEFINES)
endif()

# VST3-only release: ignore VST2/VST3 parameter ID mismatch warning
target_compile_definitions(MetalCosmos PRIVATE
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

# ===== 禁止されている compile_definitions =====
# JUCE_VST3_CAN_REPLACE_VST2 禁止
# JUCE_FORCE_USE_LEGACY_PARAM_IDS 禁止

target_link_libraries(MetalCosmos
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
)
