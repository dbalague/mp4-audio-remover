cmake_minimum_required(VERSION 3.10)

# Set the project name and C++ standard version
project(RemoveAudio VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the FFmpeg package provided by vcpkg
find_package(FFMPEG REQUIRED)

# Add executable target
add_executable(remove_audio_all remove_audio.cpp)

# Add include dirs and libs to main target
target_include_directories(remove_audio_all PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(remove_audio_all PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(remove_audio_all PRIVATE ${FFMPEG_LIBRARIES})
