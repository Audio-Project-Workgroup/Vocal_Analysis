#ifndef AUDIOIO_H
#define AUDIOIO_H


#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>



class WavImporter
// Handles importing .wav files to JUCE 
{
public:
    WavImporter();
    ~WavImporter() = default;
    
    bool loadWavFile(const juce::File& file, juce::AudioBuffer<float>& buffer, double& sampleRate);
    
private:
    juce::AudioFormatManager formatManager; // For reading other audio file formats

};

#endif 

