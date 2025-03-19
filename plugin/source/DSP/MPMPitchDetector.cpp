#include "VocalAnalysis/DSP/MPMPitchDetector.h"

MPMPitchDetector::MPMPitchDetector(double currentSampleRate) {
    jassert(currentSampleRate > 0);
    sampleRate = currentSampleRate;
}


// Main pitch detection method
float MPMPitchDetector::detectPitch(const juce::AudioBuffer<float>& buffer, int channel)
{
    if (channel < 0 || channel >= buffer.getNumChannels()) 
    {
        return 0.0f;
    }

    auto* data = buffer.getReadPointer(channel);
    int numSamples = buffer.getNumSamples();

    // Resize buffers for the current block size
    if (differenceFunction.size() < static_cast<size_t>(numSamples))
    {
        differenceFunction.resize(static_cast<size_t>(numSamples), 0.0f);
        cumulativeDifferenceFunction.resize(static_cast<size_t>(numSamples), 0.0f);
    }

    calculateDifferenceFunction(data, numSamples);
    calculateCumulativeDifferenceFunction();
    int pitchPeriod = findPitchPeriod();
    if (pitchPeriod > 0)
    {
        return sampleRate / static_cast<float>(pitchPeriod);
    }

    return 0.0f;
}

void MPMPitchDetector::calculateDifferenceFunction(const float* data, int numSamples)
{
    for (int tau = 0; tau < numSamples; ++tau)
    {
        float sum = 0.0f;
        for (int j = 0; j < numSamples - tau; ++j)
        {
            float diff = data[j] - data[j + tau];
            sum += diff * diff;
        }
        differenceFunction[tau] = sum;
    }
}

void MPMPitchDetector::calculateCumulativeDifferenceFunction()
{
    cumulativeDifferenceFunction[0] = 1.0f;
    for (int tau = 1; tau < static_cast<int>(differenceFunction.size()); ++tau)
    {
        float cumulativeSum = 0.0f;
        for (int j = 1; j <= tau; ++j)
        {
            cumulativeSum += differenceFunction[j];
        }
        cumulativeDifferenceFunction[tau] = (cumulativeSum == 0.0f) ? 1.0f : (differenceFunction[tau] / (cumulativeSum / tau));
    }
}

int MPMPitchDetector::findPitchPeriod() const
{
    const float threshold = 0.1f;
    for (int tau = 2; tau < static_cast<int>(cumulativeDifferenceFunction.size()); ++tau)
    {
        if (cumulativeDifferenceFunction[tau] < threshold &&
            cumulativeDifferenceFunction[tau] < cumulativeDifferenceFunction[tau - 1])
        {
            return tau;
        }
    }
    return -1;
}