#include "VocalAnalysis/Exporter/WavExporter.h"

void WavExporter::exportBufferToWav(const juce::AudioBuffer<float>& buffer, 
                                    const juce::File& file, 
                                    double sampleRate)
{
    if (file.existsAsFile()) {
        //file.deleteFile(); // Ensure it's a new file
        return;
    }

    juce::FileOutputStream fileStream(file);

    if (!fileStream.openedOk()) {
        DBG("Failed to open file for writing");
        return;
    }

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(&fileStream, sampleRate, buffer.getNumChannels(),
                                  16, {}, 0)
    );

    if (!writer) {
        DBG("Failed to create WAV writer");
        return;
    }

    writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    DBG("WAV file successfully written: " << file.getFullPathName());
}
