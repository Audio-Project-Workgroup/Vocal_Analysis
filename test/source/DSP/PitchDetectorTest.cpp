#include "VocalAnalysis/DSP/PitchDetector.h"
#include "VocalAnalysis/AudioIO.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <cmath>
#include <fstream>

namespace fs = std::filesystem;

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

struct ExposePitch : public PitchDetector {
    public:
        using PitchDetector::diff_function;
        using PitchDetector::cmndf;
        using PitchDetector::absolute_threshold;
        using PitchDetector::parabolic_interpolation;
        using PitchDetector::diffBuffer;
        using PitchDetector::cmndBuffer;
    };

// This sets up a shared environment (Fixture) for your test cases
class PitchDetectorTest : public ::testing::Test 
{
protected:
    // Expose protected get_pitch for testing
    ExposePitch pitch;
    
    size_t buffer_length = 2048;  // accomodates large size for vocal tracking
    double sample_rate = 44100.0;
 
    // This runs automatically before every individual test case below
    void SetUp() override {
        pitch.allocateBuffers(buffer_length, sample_rate);
    }
    
};


std::vector<float> loadGroundTruthBinary(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return {};

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<float> frequencies(static_cast<size_t>(size) / sizeof(float));
    file.read(reinterpret_cast<char*>(frequencies.data()), size);
    
    return frequencies;
}

// Test 1: Verify the Hz to MIDI converter math works 
TEST_F(PitchDetectorTest, TestMidiConversionMath) 
{
    // True C4 is exactly 261.63 Hz
    int c4_midi = pitch.hzToMIDI(261.63f);
    EXPECT_EQ(c4_midi, 60); 
    EXPECT_EQ(pitch.getMidiLetter(60), "C4");
    
    // Test A4 (440 Hz)
    int a4_midi = pitch.hzToMIDI(440.00f);
    EXPECT_EQ(a4_midi, 69);
    EXPECT_EQ(pitch.getMidiLetter(69), "A4");
}

// Test 2: Run a strict threshold check on a known 440Hz reference file
// Note: only for one single pitch value
TEST_F(PitchDetectorTest, TestSineWave440HzTracking) 
{
    WavImporter importer;

    fs::path test_file = fs::path(TEST_DATA_DIR)/"sine_wave440Hz.wav";
    
    juce::File file(test_file.string());
    juce::AudioBuffer<float> buffer;

    double sampleRate = 0.0;

    ASSERT_TRUE(importer.loadWavFile(file, buffer, sampleRate))
        << "Could not load test file: " << test_file;

    auto samples = static_cast<size_t>(buffer.getNumSamples());
    
    std::vector<double> channelData(samples);
    const float* src = buffer.getReadPointer(0);
    for (size_t i = 0; i < samples; ++i)
    {
        channelData[i] = static_cast<double>(src[i]);
    }
    float pitchHz = pitch.detectPitch(channelData.data(), static_cast<size_t>(samples));
    
    EXPECT_NEAR(pitchHz, 440.0f, 2.0f);
    
}


// Unit Testing
// Test difference function: Zero lag tau=0 must equal 0.0 for any signal
TEST_F(PitchDetectorTest, DiffFunctionZeroLagIsZero) {
    ExposePitch testPitch;
    testPitch.allocateBuffers(512, 44100.0);
    
    std::vector<double> input(512, 1.0); // Constant DC signal
    testPitch.diff_function(input.data(), input.size());
    
    EXPECT_DOUBLE_EQ(testPitch.diffBuffer[0], 0.0);
}

// CMDF Normalisation Test tau=0 always be 1.0

TEST_F(PitchDetectorTest, CMNDFInitializesToOne) {
    ExposePitch testPitch;
    testPitch.allocateBuffers(512, 44100.0);

    for (size_t tau = 0; tau < 256; ++tau)
    { 
        testPitch.diffBuffer[tau] = static_cast<double>(tau * tau);  
    }

    testPitch.cmndf(256);
    EXPECT_DOUBLE_EQ(testPitch.cmndBuffer[0], 1.0);

}

TEST_F(PitchDetectorTest, ParabolicInterpolationSymmetricValley)
{
    ExposePitch testPitch;
    testPitch.allocateBuffers(100, 44100.0);
    
    // Create a dip at index 10: s0=0.5, s1=0.1, s2=0.5
    testPitch.cmndBuffer[9]  = 0.5;
    testPitch.cmndBuffer[10] = 0.1;
    testPitch.cmndBuffer[11] = 0.5;
    
    float refinedTau = testPitch.parabolic_interpolation(10, 2, 50);
    EXPECT_FLOAT_EQ(refinedTau, 10.0f);
}

TEST_F(PitchDetectorTest, ParabolicInterpolationAsymmetricShift) {
    ExposePitch testPitch;
    testPitch.allocateBuffers(100, 44100.0);
    
    // Valley shifted slightly toward index 9 (s0 < s2)
    testPitch.cmndBuffer[9]  = 0.3;
    testPitch.cmndBuffer[10] = 0.1;
    testPitch.cmndBuffer[11] = 0.5;
    
    float refinedTau = testPitch.parabolic_interpolation(10, 2, 50);
    EXPECT_LT(refinedTau, 10.0f); // Interpolated minimum must be < 10.0
    EXPECT_GT(refinedTau, 9.5f);
}


TEST_F(PitchDetectorTest, GroundTruthSampleVocal)
{
    WavImporter importer;
    fs::path test_file = fs::path(TEST_DATA_DIR) / "female_vocal.wav";
    
    juce::File file(test_file.string());
    juce::AudioBuffer<float> buffer;
    double sampleRate = 0.0;

    ASSERT_TRUE(importer.loadWavFile(file, buffer, sampleRate))
        << "Could not load test file: " << test_file;

    const float* samples = buffer.getReadPointer(0);
    const size_t totalSamples = static_cast<size_t>(buffer.getNumSamples());
    
    const size_t hopSize = 512;
    const size_t frameSize = 2048;
    
    float prevPitch = 0.0f;
    size_t voicedFrames = 0;

    for (size_t pos = 0; pos + frameSize <= totalSamples; pos += hopSize) 
    {
        std::vector<double> frame(frameSize);
        for (size_t i = 0; i < frameSize; ++i) {
            frame[i] = static_cast<double>(samples[pos + i]);
        }

        float pitchHz = pitch.detectPitch(frame.data(), frameSize);

        if (pitchHz > 0.0f) 
        {
            voicedFrames++;

            // Property 1: Pitch within realistic female vocal bounds
            EXPECT_GE(pitchHz, 120.0f) << "Pitch lower than human female range at sample " << pos;
            EXPECT_LE(pitchHz, 1100.0f) << "Pitch higher than human female range at sample " << pos;

            // Property 2: Pitch continuity (check for octave jumping relative to previous frame)
            if (prevPitch > 0.0f) 
            {
                float ratio = pitchHz / prevPitch;
                // Detect octave doubling (~2.0) or halving (~0.5)
                EXPECT_TRUE(ratio > 0.7f && ratio < 1.4f) 
                    << "Discontinuous pitch jump detected from " << prevPitch 
                    << " Hz to " << pitchHz << " Hz near sample " << pos;
            }
            prevPitch = pitchHz;
        }

        else
        {
            prevPitch = 0.0f;

        }
     
        
    }

    // Ensure detector actually picked up pitch over the file
    EXPECT_GT(voicedFrames, 0u) << "No voiced segments detected in vocal file.";

}