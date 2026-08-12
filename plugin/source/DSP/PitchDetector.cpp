#include <VocalAnalysis/DSP/PitchDetector.h>
#include <cmath>
#include <vector>
#include <stdlib.h>


void PitchDetector::allocateBuffers(size_t maxBlockSize, double sampleRate)
{
    mbufferSize = maxBlockSize;
    msampleRate = sampleRate;
    diffBuffer.assign(maxBlockSize, 0.0);
    cmndBuffer.assign(maxBlockSize, 0.0);
}

void PitchDetector::diff_function(const double *input, size_t size)
{
    size_t halfSize = size / 2;
    if (halfSize == 0) return;

    for (size_t tau = 0; tau < halfSize; ++tau)
    {
        double sum = 0.0;
        for (size_t i = 0; i < halfSize; ++i)
        {
            double delta = input[i] - input[i + tau];
            sum += delta * delta;
        }

        diffBuffer[tau] = sum;
    }
}

void PitchDetector::cmndf(size_t size)
{
    // Calculate the cumulative mean normalised difference function
    cmndBuffer[0] = 1.0;
    double running_sum = 0.0;

    for (size_t tau = 1; tau < size; ++tau)
    {
        running_sum += diffBuffer[tau];
        if (running_sum > 0.0)
        {
            cmndBuffer[tau] = diffBuffer[tau] * static_cast<double>(tau) / running_sum;
        }
        else
        {
            cmndBuffer[tau] = 1.0;
        }
    }
}


int PitchDetector::absolute_threshold(size_t tauMin, size_t tauMax)
{
    size_t firstTau = 0;
    bool found = false;

    // Search for first minimum below threshold
    for (size_t tau = tauMin; tau <= tauMax; tau++)
    {
        if (cmndBuffer[tau] < YIN_THRESHOLD)
        {
            while (tau + 1 < tauMax && cmndBuffer[tau + 1] < cmndBuffer[tau])
            {
                tau++;
            }
            firstTau = tau; // Both are size_t, no casting needed
            found = true;
            break;
        }
    }

    if (found)
    {
        // Check if double lag (2 * tau) has a significantly lower dip (true fundamental)
        size_t doubleTau = firstTau * 2;
        if (doubleTau <= tauMax)
        {
            // If the double lag is substantially deeper, prefer the fundamental
            if (cmndBuffer[doubleTau] < cmndBuffer[firstTau] * 0.5)
            {
                return static_cast<int>(doubleTau);
            }
        }
        return static_cast<int>(firstTau);
    }

    // Fallback: search for absolute global minimum if no threshold hit
    auto beginIt = cmndBuffer.begin() + static_cast<std::ptrdiff_t>(tauMin);
    auto endIt = cmndBuffer.begin() + static_cast<std::ptrdiff_t>(tauMax + 1);
    auto minElementIt = std::min_element(beginIt, endIt);

    if (*minElementIt > 0.40) // Confidence gate
    {
        return -1; 
    }

    return static_cast<int>(std::distance(cmndBuffer.begin(), minElementIt));
}



 float PitchDetector::parabolic_interpolation(size_t tau_estimate, size_t tauMin, size_t tauMax)
 {
   
    if (tau_estimate <= tauMin || tau_estimate >= tauMax) return static_cast<float>(tau_estimate); 
  
    size_t x0 = tau_estimate - 1;
    size_t x2 = tau_estimate + 1;
    
    
    double s0 = cmndBuffer[x0];
    double s1 = cmndBuffer[tau_estimate];
    double s2 = cmndBuffer[x2];
    
    double denominator = 2.0 * (s0 + s2 - 2.0 * s1);
    if (std::abs(denominator) < 1e-6) return static_cast<float>(tau_estimate);
    
    double shift = (s0 - s2) / denominator;

    return static_cast<float>(tau_estimate) + static_cast<float>(shift);
    
}

float PitchDetector::get_pitch(const double *input, size_t size)
{
    // Get the pitch of the signal using the YIN algorithm
    size = std::min(size, mbufferSize);
    if (size == 0 || input == nullptr) return 0.0f; 


    size_t halfSize = size / 2;
    double sumOfSquares = 0.0;

    for (size_t i = 0; i < size; ++i)
    {
        sumOfSquares += input[i] * input[i];
    }

    const double SILENCE_THRESHOLD_RMS = 0.00316; //Checks RMS for silence
 
    double rms = std::sqrt(sumOfSquares / static_cast<double>(size));
    if (rms < SILENCE_THRESHOLD_RMS) return 0.0f; 

    diff_function(input, size);
    cmndf(halfSize);

    

    double fMin = 120.0;
    double fMax = 1046.5;

    // Convert frequenciess to period lags (tau)
    size_t tauMin = static_cast<size_t>(std::floor(msampleRate / fMax));
    size_t tauMax = static_cast<size_t>(std::ceil(msampleRate / fMin));

    // Clamp to valid arrays
    tauMin = std::max(tauMin, static_cast<size_t>(2));         // Lag must be at least 2 for differences/interpolation
    tauMax = std::min(tauMax, halfSize - 1);

    int tau = absolute_threshold(tauMin, tauMax);

    if (tau != -1)
    {
        float refined_tau = parabolic_interpolation(static_cast<size_t>(tau), tauMin, tauMax);

        if (refined_tau > 0.0f) return static_cast<float>(msampleRate) / refined_tau; // Convert lag to frequency
        
    } 
    
    return 0.0f; // No pitch found
    
}


float PitchDetector::hzToMIDI(float freq)
{
// Convert hz to MIDI
    float midi_note = 69 + (12 * log2(freq/440.0f));
    return std::round(midi_note);

}

juce::String PitchDetector::getMidiLetter(int midiNoteNumber)
{

    const juce::StringArray noteNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    if (midiNoteNumber > 127 || midiNoteNumber < 0)
    {
        return "Pitch out of range"; 
    }

    int noteIndex = midiNoteNumber % 12;
    int octave = (midiNoteNumber / 12) - 1;

    return noteNames[noteIndex] + juce::String(octave);

}


