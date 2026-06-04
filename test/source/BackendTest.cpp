#include <gtest/gtest.h>
#include "DSP/PitchDetector.h"
#include "AudioFile.h"
#include <filesystem>
#include <cmath>

namespace fs = std::filesystem;

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

// This sets up a shared environment (Fixture) for your test cases
class PitchDetectorTest : public ::testing::Test {
protected:
    YinAlgorithm yin;
    MidiConverter converter;
    size_t buffer_length = 1024;
    double sample_rate = 44100.0;

    // This runs automatically before every individual test case below
    void SetUp() override {
        yin.allocateBuffers(buffer_length, sample_rate);
    }
};

// Test 1: Verify the Hz to MIDI converter math works 
TEST_F(PitchDetectorTest, TestMidiConversionMath) {
    // True C4 is exactly 261.63 Hz
    float midi_note = converter.hz_to_midi(261.63f);
    int nearest_midi = static_cast<int>(std::round(midi_note));
    
    EXPECT_EQ(nearest_midi, 60); 
    EXPECT_EQ(converter.midi_to_note_name(60), "C4");
    
    // Test A4 (440 Hz)
    EXPECT_EQ(static_cast<int>(std::round(converter.hz_to_midi(440.0f))), 69);
    EXPECT_EQ(converter.midi_to_note_name(69), "A4");
}

// Test 2: Run a strict threshold check on a known 440Hz reference file
TEST_F(PitchDetectorTest, TestSineWave440HzTracking) {
    fs::path test_file = fs::path(TEST_DATA_DIR) / "sine_wave440Hz.wav";
    
    AudioFile<double> audioFile;
    bool loaded = audioFile.load(test_file.string());
    
    // Force the test to instantly abort if test asset directory is missing
    ASSERT_TRUE(loaded) << "Could not find or load target test file: " << test_file;

    std::vector<double> audioData = audioFile.samples[0];
    int hop_size = static_cast<int>(buffer_length / 2);
    int voiced_frames = 0;
    
    // Scan a fixed snippet of the reference file
    for (int i = 0; i + static_cast<int>(buffer_length) < static_cast<int>(audioData.size()); i += hop_size) {
        const double* blockStart = &audioData[static_cast<size_t>(i)];
        float current_pitch = yin.get_pitch(blockStart, buffer_length);
        
        if (current_pitch > 0) {
            voiced_frames++;
            // Asserts that the estimated pitch is within 2.0 Hz of 440 Hz.
            EXPECT_NEAR(current_pitch, 440.0, 2.0) 
                << "Pitch tracking drifted too far at sample window: " << i;
        }
    }
    
    // Assert that the tracker didn't output silence/zeros through the whole file
    EXPECT_GT(voiced_frames, 0) << "Algorithm failed to detect any voiced pitch in the sine wave";
}