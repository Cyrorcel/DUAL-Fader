#include "PluginEditor.h"

DualFaderAudioProcessorEditor::DualFaderAudioProcessorEditor (DualFaderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setupFader (fader1, label1,      "Fader 1");
    setupFader (fader2, label2,      "Fader 2");
    setupFader (master, labelMaster, "Master");

    attachment1      = std::make_unique<Attachment> (processor.apvts, DualFaderAudioProcessor::idFader1, fader1);
    attachment2      = std::make_unique<Attachment> (processor.apvts, DualFaderAudioProcessor::idFader2, fader2);
    attachmentMaster = std::make_unique<Attachment> (processor.apvts, DualFaderAudioProcessor::idMaster, master);

    // Double-clic sur un fader : retour à 0 dB
    for (auto* s : { &fader1, &fader2, &master })
        s->setDoubleClickReturnValue (true, 0.0);

    setSize (360, 340);
}

void DualFaderAudioProcessorEditor::setupFader (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 22);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void DualFaderAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff23262b));

    // Séparateur discret avant le master
    auto area = getLocalBounds().reduced (16);
    const int colWidth = area.getWidth() / 3;
    g.setColour (juce::Colours::white.withAlpha (0.15f));
    g.drawVerticalLine (area.getX() + colWidth * 2, (float) area.getY(), (float) area.getBottom());
}

void DualFaderAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    const int colWidth = area.getWidth() / 3;

    auto layoutColumn = [] (juce::Rectangle<int> col, juce::Label& label, juce::Slider& slider)
    {
        label.setBounds (col.removeFromTop (28));
        slider.setBounds (col.reduced (8, 4));
    };

    layoutColumn (area.removeFromLeft (colWidth), label1, fader1);
    layoutColumn (area.removeFromLeft (colWidth), label2, fader2);
    layoutColumn (area,                           labelMaster, master);
}
