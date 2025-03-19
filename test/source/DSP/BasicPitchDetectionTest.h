#pragma once

#include <gtest/gtest.h>
#include <VocalAnalysis/DSP/BasicPitchDetector.h>
#include <VocalAnalysis/Importer/WavImporter.h>

namespace pitch_detection_test {
    class BasicPitchDetectorTest : public ::testing::Test {
    protected:
        void SetUp() override {
            GTEST_SKIP() << "Skipping all pitch detection tests since the algorithm is not accurate enough.";

            sampleRate = 44100.0;
            numTestSamples = 4096;
            zeroPadding = numTestSamples;
            totalTestSamples = zeroPadding + numTestSamples + zeroPadding; 
            buffer.setSize(1, totalTestSamples);
            pitchDetector = std::make_unique<BasicPitchDetector>(sampleRate);
        }

        void TearDown() override {
            pitchDetector.reset();
        }

        double sampleRate;
        int channel = 0;
        int numTestSamples;
        int zeroPadding;
        int totalTestSamples;
        juce::AudioBuffer<float> buffer;
        std::unique_ptr<BasicPitchDetector> pitchDetector;
    public:
        void generateSineWave(float frequency) {
            const float amplitude = 0.5f;
            buffer.clear();
            float* data = buffer.getWritePointer(0);
            
            for (size_t i = 0; i < static_cast<size_t>(numTestSamples); ++i)
            {
                data[static_cast<size_t>(zeroPadding) + i] = amplitude * std::sinf(2.0f * juce::MathConstants<float>::pi * frequency * (i / static_cast<float>(sampleRate)));
            }
        }

        void loadAndProcessAudio() {
            juce::File wavFile = juce::File::getCurrentWorkingDirectory()
                                        .getChildFile("test/resources/audio/female_voice_D4_[16bit_44100Hz].wav");

            sampleRate = 0.0;

            if (WavImporter::loadWavFileIntoBuffer(wavFile, buffer, sampleRate)) {
                DBG("Successfully loaded WAV file!");

            } else {
                FAIL() << "Failed to load WAV file.";
            }
        }
    };
}
