#include <VocalAnalysis/AudioIO.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>
#include <limits>


WavImporter::WavImporter()
{
    formatManager.registerBasicFormats();
}

bool WavImporter::loadWavFile(const juce::File& file, juce::AudioBuffer<float>& buffer, double& sampleRate)
{
    if (!file.existsAsFile()) 
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(file));

    if (!reader)
        return false;

    sampleRate = reader->sampleRate;

    auto numSamples64 = reader->lengthInSamples;
    jassert(numSamples64 <= std::numeric_limits<int>::max());

    int safeSamples = static_cast<int>(numSamples64);
    int safeChannels = static_cast<int>(reader->numChannels);

    buffer.setSize(safeChannels, safeSamples);
    if (!reader->read(&buffer, 0, safeSamples, 0, true, true))
        return false;

    return true;
}
