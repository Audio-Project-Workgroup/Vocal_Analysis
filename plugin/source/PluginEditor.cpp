#include "VocalAnalysis/PluginProcessor.h"
#include "VocalAnalysis/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    
    //record button
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    recordButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::grey);
    recordButton.setClickingTogglesState(true);
    recordButton.onClick = [this]() { };
    addAndMakeVisible(recordButton);
    
    
    //record button
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    stopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    stopButton.setClickingTogglesState(true);
    stopButton.onClick = [this]() { };
    addAndMakeVisible(stopButton);
    
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //auto bounds = getLocalBounds();
    const int componentSize {50};
    recordButton.setBounds(bounds.removeFromTop(400).removeFromLeft(100).withSizeKeepingCentre(componentSize, componentSize));
    stopButton.setBounds(100, 125, componentSize, componentSize);

}
