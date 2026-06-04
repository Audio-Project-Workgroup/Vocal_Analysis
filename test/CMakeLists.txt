cmake_minimum_required(VERSION 3.26)

project(Vocal_AnalysisTest)

# Signals to CMake that we want to run tests from this directory.
enable_testing()


# Download and add AudioFile as a dependency for testing purposes
include(FetchContent)

FetchContent_Declare(
    audiofile_github
    GIT_REPOSITORY https://github.com/adamstark/AudioFile.git
    GIT_TAG        master 
)

# Download and make the content available
FetchContent_MakeAvailable(audiofile_github)

add_executable(${PROJECT_NAME}
    source/AudioProcessorTest.cpp
    source/BackendTest.cpp

)


# Sets the necessary include directories: ours, JUCE's, and googletest's.
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../plugin/include
        ${JUCE_SOURCE_DIR}/modules
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
        
       
    SYSTEM PRIVATE
        ${audiofile_github_SOURCE_DIR}
)


target_compile_definitions(${PROJECT_NAME}
    PRIVATE 
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_data"
)
   

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Vocal_AnalysisPlugin
        GTest::gtest_main)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Adds googletest-specific CMake commands at our disposal.
include(GoogleTest)
# Add all tests defined with googletest to the CMake metadata so that these tests are run upon a call to ctest in the test projects' binary directory.
if (CMAKE_GENERATOR STREQUAL Xcode)
# On macOS arm64, all binaries have to be signed before running. In local development, the linker adds an ad-hoc placeholder signature. In Xcode however, the ad-hoc signature is delayed until after the “Run Script” build phase, so the POST_BUILD command added by gtest_discover_tests cannot run. Thus, we need to delay test discovery until run time.
# Source: https://discourse.cmake.org/t/googletest-crash-when-using-cmake-xcode-arm64/5766/8
  gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
  gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
endif()