#ifndef PITCHDETECTOR_H
#define PITCHDETECTOR_H

#include <cmath>
#include <algorithm>
#include <juce_dsp/juce_dsp.h>


#define YIN_THRESHOLD 0.1


class YinAlgorithm
{
    public:
        YinAlgorithm() = default;
        ~YinAlgorithm() = default;
        void allocateBuffers(size_t maxBlockSize, double sampleRate);
        int absolute_threshold(size_t size);
        float parabolic_interpolation(size_t tau, size_t halfSize);
        float get_pitch(const double *input, size_t size);
        

        // void set_buffer_size(int newSize) { mbufferSize = newSize; }
        // void set_sample_rate(int newRate) { msampleRate = newRate; }

        // int get_buffer_size() { return mbufferSize; }
        // int get_sample_rate() { return msampleRate; }

    private:
        size_t mbufferSize = 1024;
        int msampleRate = 44100;

        // Buffers for the difference function and cumulative mean normalised difference function
        std::vector<double> diffBuffer;
        std::vector<double> cmndBuffer;

        void diff_function(const double *input, size_t size);
        void cmndf(size_t size);

        

};

class MidiConverter
{
    public:
        MidiConverter() = default;
        ~MidiConverter() = default;
        float hz_to_midi(float hz);
        std::string midi_to_note_name(int midi_note); // placeholder for testing the yun algorithm, 
                                                    // will be implemented in the plugin using juce::MidiMessage::getMidiNoteName
};

#endif  // PITCHDETECTOR_H