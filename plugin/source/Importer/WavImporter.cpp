#include "VocalAnalysis/Importer/WavImporter.h"

bool WavImporter::loadWavFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer, double& sampleRate) {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader == nullptr) {
        return false;
    }

    sampleRate = reader->sampleRate;
    int numSamples = static_cast<int>(reader->lengthInSamples);
    int numChannels = static_cast<int>(reader->numChannels);

    buffer.setSize(numChannels, numSamples);
    reader->read(&buffer, 0, numSamples, 0, true, true);

    return true;
}