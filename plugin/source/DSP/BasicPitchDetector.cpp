#include "VocalAnalysis/DSP/BasicPitchDetector.h"
#include <vector>
#include <cmath>

BasicPitchDetector::BasicPitchDetector(double currentSampleRate) {
    jassert(currentSampleRate > 0);
    sampleRate = currentSampleRate;
}

double BasicPitchDetector::detectPitch(const juce::AudioBuffer<float>& buffer, int channel) {
    std::size_t numSamples = static_cast<std::size_t>(buffer.getNumSamples());
    if (numSamples == 0 || sampleRate <= 0) return 0.0;

    const float* audioData = buffer.getReadPointer(channel);
    std::vector<double> autocorrelation = computeAutocorrelation(audioData, numSamples);
    
    int bestLag = findBestLag(autocorrelation);
    return sampleRate / static_cast<double>(bestLag);
}

std::vector<double> BasicPitchDetector::computeAutocorrelation(const float* audioData, std::size_t numSamples) {
    std::vector<double> autocorrelation(numSamples, 0.0);
    
    for (std::size_t lag = 0; lag < numSamples; ++lag) {
        double sum = 0.0;
        for (std::size_t i = 0; i < numSamples - lag; ++i) {
            sum += audioData[i] * audioData[i + lag];
        }
        autocorrelation[lag] = sum;
    }
    
    return autocorrelation;
}

int BasicPitchDetector::findBestLag(const std::vector<double>& autocorrelation) {
    int minLag = static_cast<int>(sampleRate / 1000); // Ignore frequencies above 1000 Hz
    int maxLag = static_cast<int>(sampleRate / 50);   // Ignore frequencies below 50 Hz

    int bestLag = minLag;
    double maxCorrelation = 0.0;

    for (int lag = minLag; lag < maxLag; ++lag) {
        if (autocorrelation[static_cast<std::size_t>(lag)] > maxCorrelation) {
            maxCorrelation = autocorrelation[static_cast<std::size_t>(lag)];
            bestLag = lag;
        }
    }
    
    return bestLag;
}