#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

class WavImporter
{
public:
    static bool loadWavFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer, double& sampleRate);
};