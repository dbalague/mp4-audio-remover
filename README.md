# mp4-audio-remover
Remove audio from MP4 videos. Written with help from ChatGPT

# Installation

## Linux/MacOS

- Install CMake:
    - On Linux: most Linux distros have it in their respective package manager).
    - On MacOS: just use `brew install cmake` (assuming you have [homebrew](https://brew.sh))

- Install and configure [VCPKG](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash) (Steps 1 and 2 only)

- Clone or download and then install this repo:

```bash
git clone https://github.com/dbalague/mp4-audio-remover.git
cd mp4-audio-remover
cmake --preset=default
cmake --build build
```

The corresponding executable is inside the `build` directory

## Windows

- Install [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/). Make sure to check *Desktop development with C++* and that the *vcpkg package manager* and *C++ CMake tools for Windows* options are checked.
- Download or clone this repo.
- Open the folder containing the files of this repo as a project (the first time VS will download a lot of packages).
- Build the project (it will create the executable `remove_audio_all`)
- The corresponding executable and dlls are in the `build` directory

# Usage

Copy the executable (and dlls on Windows, if applicable) in a directory where you have the `mp4` videos and, within a terminal, run `./remove_audio_all` or equivalent on Wiindows. On Windows, you can also double click the executable.
