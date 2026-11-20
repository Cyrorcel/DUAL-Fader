#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class DualFaderAudioProcessor : public juce::AudioProcessor
{
public:
    DualFaderAudioProcessor();
    ~DualFaderAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DualFader"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Identifiants des paramètres (partagés avec l'éditeur)
    static constexpr const char* idFader1 = "fader1";
    static constexpr const char* idFader2 = "fader2";
    static constexpr const char* idMaster = "master";

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Les deux pistes stéréo issues de la duplication du signal d'entrée
    juce::AudioBuffer<float> track1, track2;

    std::atomic<float>* fader1Param = nullptr;
    std::atomic<float>* fader2Param = nullptr;
    std::atomic<float>* masterParam = nullptr;

    // Gains linéaires lissés (évite les clics quand on bouge un fader)
    juce::SmoothedValue<float> gain1, gain2, gainMaster;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualFaderAudioProcessor)
};
