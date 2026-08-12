#ifndef PITCHDETECTOR_H
#define PITCHDETECTOR_H

#include <cmath>
#include <algorithm>
#include <juce_core/juce_core.h>

#define YIN_THRESHOLD 0.10


class PitchDetector
{
    public:
        PitchDetector() = default;
        ~PitchDetector() = default;
        void allocateBuffers(size_t maxBlockSize, double sampleRate);
        float detectPitch(const double* input, size_t size) { return get_pitch(input, size); };
        static juce::String getMidiLetter(int midiNoteNumber);
        float hzToMIDI(float freq);
      

    private:
        size_t mbufferSize = 1024;
        int msampleRate = 44100;
      
    protected:
        // Buffers for the difference function and cumulative mean normalised difference function
        std::vector<double> diffBuffer;
        std::vector<double> cmndBuffer;

    
        void diff_function(const double *input, size_t size);
        void cmndf(size_t size);
        int absolute_threshold(size_t tauMin, size_t tauMax);
        float parabolic_interpolation(size_t tau_estimate, size_t tauMin, size_t tauMax);
        float get_pitch(const double *input, size_t size);
        

};


#endif  // PITCHDETECTOR_