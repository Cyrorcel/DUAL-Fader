#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

class DualFaderAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DualFaderAudioProcessorEditor (DualFaderAudioProcessor&);
    ~DualFaderAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    DualFaderAudioProcessor& processor;

    juce::Slider fader1, fader2, master;
    juce::Label  label1, label2, labelMaster;

    // Déclarés après les sliders : ils doivent être détruits en premier
    std::unique_ptr<Attachment> attachment1, attachment2, attachmentMaster;

    void setupFader (juce::Slider&, juce::Label&, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualFaderAudioProcessorEditor)
};
