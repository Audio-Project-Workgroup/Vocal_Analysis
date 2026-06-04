#include "VocalAnalysis/DSP/PitchDetector.h"

#include <cmath>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <stdlib.h>


void YinAlgorithm::allocateBuffers(size_t maxBlockSize, double sampleRate)
{
    mbufferSize = maxBlockSize;
    msampleRate = sampleRate;
    diffBuffer.assign(maxBlockSize, 0.0);
    cmndBuffer.assign(maxBlockSize, 0.0);
}

void YinAlgorithm::diff_function(const double *input, size_t size)
{
    size_t halfSize = size / 2;
    // Calculate the difference function of signal x and lag tau
    for (size_t tau = 0; tau < halfSize; tau++){
        diffBuffer[tau] = 0;
        for (size_t j = 0; j < size - tau; j++){
            double delta = input[j] - input[j + tau];
            diffBuffer[tau] += delta * delta;
        }
    }
}

void YinAlgorithm::cmndf(size_t size)
{
    // Calculate the cumulative mean normalised difference function
    
    cmndBuffer[0] = 1.0;
    double running_sum = 0.0;

    for (size_t tau = 1; tau < size; tau++){
        running_sum += diffBuffer[tau];
        cmndBuffer[tau] = diffBuffer[tau] * tau / running_sum;
    }
    
}

 int YinAlgorithm::absolute_threshold(size_t size)
 {
    // Search through the cumulative mean normalised difference function and find the first minimum that is below the threshold
    
    for (size_t tau = 1; tau < size; tau++){
        if (cmndBuffer[tau] < YIN_THRESHOLD){
            while (tau + 1 < size && cmndBuffer[tau + 1] < cmndBuffer[tau]){
                tau++;
            }
            return static_cast<int>(tau);
        }
    }
    return -1;
 }

 float YinAlgorithm::parabolic_interpolation(size_t tau_estimate, size_t halfSize)
 {
    size_t x0 = (tau_estimate < 1) ? tau_estimate : tau_estimate - 1;
    size_t x2 = (tau_estimate + 1 < halfSize) ? tau_estimate + 1 : tau_estimate;
    
    if (x0 == tau_estimate || x2 == tau_estimate)
    {
        return static_cast<float>(tau_estimate); // Returns lag value w/o interpolation if at the boundaries
    }
    
    double s0 = cmndBuffer[x0];
    double s1 = cmndBuffer[tau_estimate];
    double s2 = cmndBuffer[x2];
    
    double denominator = 2.0 * (2.0 * s1 - s2 - s0);
    if (std::abs(denominator) < 1e-5) return static_cast<float>(tau_estimate);
    
    return static_cast<float>(tau_estimate) + static_cast<float>((s2 - s0) / denominator);
    
}

float YinAlgorithm::get_pitch(const double *input, size_t size)
{
    // Get the pitch of the signal using the YIN algorithm
    size_t halfSize = size / 2;
    diff_function(input, size);
    cmndf(halfSize);
    int tau = absolute_threshold(halfSize);

    if (tau != -1){
        float refined_tau = parabolic_interpolation(static_cast<size_t>(tau), halfSize);

        if (refined_tau > 0.0f){

            return static_cast<float>(msampleRate) / refined_tau; // Convert lag to frequency
        }
    } 
    
    return 0.0f; // No pitch found
    
}

float MidiConverter::hz_to_midi(float hz)
{
    // Convert frequency in Hz to MIDI note number
    if (hz <= 0.0f) return 0.0f;
    return 69 + 12 * log2(hz / 440.0);
}


// Placeholder for testing the Yin algorithm. Will be implemented using juce framework later in the plugin development process.
std::string MidiConverter::midi_to_note_name(int midi_note){
    static const std::vector<std::string> note_names = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (midi_note / 12) - 1;
    int note_index = midi_note % 12;
    return note_names[static_cast<std::size_t>(note_index)] + std::to_string(octave);
}
