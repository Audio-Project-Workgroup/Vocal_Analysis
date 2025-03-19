#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

/* McLeod Pitch Method */
class MPMPitchDetector
{
public:
    MPMPitchDetector(double sampleRate);
    ~MPMPitchDetector() = default;

    float detectPitch(const juce::AudioBuffer<float>& buffer, int channel);

private:
    double sampleRate;

    void calculateDifferenceFunction(const float* data, int numSamples);
    void calculateCumulativeDifferenceFunction();
    int findPitchPeriod() const;

    std::vector<float> differenceFunction;
    std::vector<float> cumulativeDifferenceFunction;
};
