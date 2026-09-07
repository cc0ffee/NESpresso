# NESpresso

Emulation of the Nintendo Entertainment System (NES)

## Requirements

- A C++20-compatible compiler
- [CMake](https://cmake.org/) 3.16 or newer
- [SDL2](https://www.libsdl.org/) development libraries

### Installing dependencies

Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev
```

macOS with Homebrew:

```bash
brew install cmake sdl2
```

Windows with an MSYS2 UCRT64 shell:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-SDL2
```

### Build

Clone the repository and build it with CMake:

```bash
git clone https://github.com/cc0ffee/NESpresso.git
cd NESpresso
cmake -S . -B build
cmake --build build
```

### Usage

Pass the path to a NES ROM as the first argument:

```bash
./build/nes path/to/rom.nes
```

### Acknowledgments
- [NESDev Wiki Reference](https://www.nesdev.org/wiki/NES_reference_guide)
- [100th_Coin's NES Post](https://www.patreon.com/100th_Coin/posts/making-your-nes-137873901)