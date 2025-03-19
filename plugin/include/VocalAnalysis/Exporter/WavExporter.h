#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

class WavExporter {
public:
    static void exportBufferToWav(const juce::AudioBuffer<float>& buffer,
                                  const juce::File& file,
                                  double sampleRate);
};