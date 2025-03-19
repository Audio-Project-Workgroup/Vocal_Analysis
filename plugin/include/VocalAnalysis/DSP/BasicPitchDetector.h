#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class BasicPitchDetector
{
public:
    BasicPitchDetector(double sampleRate);
    double detectPitch(const juce::AudioBuffer<float>& buffer, int channel);
private:
    double sampleRate;
    std::vector<double> computeAutocorrelation(const float* audioData, std::size_t numSamples);
    int findBestLag(const std::vector<double>& autocorrelation);
};