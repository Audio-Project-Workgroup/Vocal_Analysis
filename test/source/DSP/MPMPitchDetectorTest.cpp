#include "MPMPitchDetectorTest.h"

namespace pitch_detection_test {
    TEST_F(MPMPitchDetectorTest, DetectsPitch440HzCorrectly) {

        const float frequency = 440.0f;
        generateSineWave(frequency);

        float detectedPitch = pitchDetector->detectPitch(buffer, 0);

        EXPECT_NEAR(detectedPitch, frequency, 1.0f);  // 1 Hz tolerance
    }

    TEST_F(MPMPitchDetectorTest, DetectsPitch220HzCorrectly) {

        const float frequency = 220.0f;
        generateSineWave(frequency);

        float detectedPitch = pitchDetector->detectPitch(buffer, 0);

        EXPECT_NEAR(detectedPitch, frequency, 1.0f);  // 1 Hz tolerance
    }

    TEST_F(MPMPitchDetectorTest, DetectsPitchD4FileCorrectly) {
        const float expectedFrequency = 293.67f; // D4
        
        loadAndProcessAudio();

        float detectedPitch = pitchDetector->detectPitch(buffer, 0);
        EXPECT_NEAR(detectedPitch, expectedFrequency, 1.0f);
    }
} 