#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr float minDb = -60.0f;   // en dessous : silence total (-inf)
    constexpr float maxDb = 12.0f;

    float dbToLinear (float db)
    {
        return db <= minDb ? 0.0f : juce::Decibels::decibelsToGain (db);
    }
}

DualFaderAudioProcessor::DualFaderAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createParameterLayout())
{
    fader1Param = apvts.getRawParameterValue (idFader1);
    fader2Param = apvts.getRawParameterValue (idFader2);
    masterParam = apvts.getRawParameterValue (idMaster);
}

juce::AudioProcessorValueTreeState::ParameterLayout DualFaderAudioProcessor::createParameterLayout()
{
    auto makeFader = [] (const char* id, const char* name)
    {
        auto attributes = juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) -> juce::String
            {
                return v <= minDb ? juce::String ("-inf dB")
                                  : juce::String (v, 1) + " dB";
            })
            .withValueFromStringFunction ([] (const juce::String& text) -> float
            {
                auto t = text.trim();
                if (t.startsWithIgnoreCase ("-inf"))
                    return minDb;
                return t.getFloatValue();
            });

        return std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 },
            name,
            juce::NormalisableRange<float> (minDb, maxDb, 0.1f),
            0.0f,
            attributes);
    };

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (makeFader (idFader1, "Fader 1"));
    layout.add (makeFader (idFader2, "Fader 2"));
    layout.add (makeFader (idMaster, "Master"));
    return layout;
}

void DualFaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    track1.setSize (2, samplesPerBlock);
    track2.setSize (2, samplesPerBlock);

    const double rampSeconds = 0.02;
    gain1.reset (sampleRate, rampSeconds);
    gain2.reset (sampleRate, rampSeconds);
    gainMaster.reset (sampleRate, rampSeconds);

    gain1.setCurrentAndTargetValue (dbToLinear (fader1Param->load()));
    gain2.setCurrentAndTargetValue (dbToLinear (fader2Param->load()));
    gainMaster.setCurrentAndTargetValue (dbToLinear (masterParam->load()));
}

bool DualFaderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void DualFaderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || numSamples == 0)
        return;

    // Sécurité : si l'hôte envoie un bloc plus grand que prévu
    if (track1.getNumSamples() < numSamples)
    {
        track1.setSize (2, numSamples, false, false, true);
        track2.setSize (2, numSamples, false, false, true);
    }

    // 1) Duplication : l'entrée stéréo est copiée sur les deux pistes
    for (int ch = 0; ch < 2; ++ch)
    {
        track1.copyFrom (ch, 0, buffer, ch, 0, numSamples);
        track2.copyFrom (ch, 0, buffer, ch, 0, numSamples);
    }

    // ------------------------------------------------------------------
    // 2) C'est ici que tu pourras insérer un traitement propre à chaque piste :
    //      - piste 1 : traitement sur track1 (ex. un delay, un filtre, ...)
    //      - piste 2 : traitement sur track2
    // ------------------------------------------------------------------

    // 3) Mise à jour des gains cibles depuis les paramètres
    gain1.setTargetValue      (dbToLinear (fader1Param->load()));
    gain2.setTargetValue      (dbToLinear (fader2Param->load()));
    gainMaster.setTargetValue (dbToLinear (masterParam->load()));

    // 4) Faders de piste, somme vers le bus stéréo, puis fader master
    const float* t1[2] = { track1.getReadPointer (0), track1.getReadPointer (1) };
    const float* t2[2] = { track2.getReadPointer (0), track2.getReadPointer (1) };
    float* out[2]      = { buffer.getWritePointer (0), buffer.getWritePointer (1) };

    for (int i = 0; i < numSamples; ++i)
    {
        const float g1 = gain1.getNextValue();
        const float g2 = gain2.getNextValue();
        const float gm = gainMaster.getNextValue();

        for (int ch = 0; ch < 2; ++ch)
            out[ch][i] = (t1[ch][i] * g1 + t2[ch][i] * g2) * gm;
    }
}

juce::AudioProcessorEditor* DualFaderAudioProcessor::createEditor()
{
    return new DualFaderAudioProcessorEditor (*this);
}

void DualFaderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void DualFaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DualFaderAudioProcessor();
}
