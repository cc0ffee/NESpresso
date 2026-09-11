# NESpresso

<img src="https://i.imgur.com/4smgNBV.gif" align="right" width="38%"/>

Emulation of the Nintendo Entertainment System (NES) implemented with C++ and SDL2 library.

Built from scratch to learn how the NES and emulation works, it implements the CPU, PPU, memory bus, and cartridge hardware with ongoing work to improve accuracy and compatibility.

Currently there are 4 supported cartridge mappers:
- **Mapper 0 (NROM)**
- **Mapper 1 (MMC1/SxROM)**
- **Mapper 2 (UxROM)**
- **Mapper 3 (CNROM)**

These implemented mappers allow the emulator to run ~350 NES games!

> [!WARNING]
> Game compatibility varies, and the emulator is not yet cycle-accurate. Audio and additional mappers are planned.

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

### Screenshots

<table>
  <tr>
    <td align="center">
      <img src="https://i.imgur.com/uyqT1dK.png" width="97%"/>
      <br>
      <sub>AccuracyCoin Test Results</sub>
    </td>
    <td width="2"></td>
    <td align="center">
      <img src="https://i.imgur.com/3BviZk7.png" width="100%"/>
      <br>
      <sub>Tetris (using Mapper 1 - MMC1 - SxROM)</sub>
    </td>
  </tr>
</table>

### Acknowledgments
- [NESDev Wiki Reference](https://www.nesdev.org/wiki/NES_reference_guide)
- [100th_Coin's NES Post](https://www.patreon.com/100th_Coin/posts/making-your-nes-137873901)
- [emudev.de](https://emudev.de/nes-emulator/overview/)
