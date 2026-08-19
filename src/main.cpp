#include "cpu.hpp"
#include "cartridge.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) { std::cerr << "Usage: nes ROM\n"; return 1; }
    std::ifstream rom_file(argv[1], std::ios::binary);
    if (!rom_file) { std::cerr << "Could not open ROM\n"; return 1; }
    Cartridge cartridge(rom_file);
    Bus bus(cartridge);
    CPU cpu(bus);
    cpu.reset();
    for (unsigned i = 0; i < 500000 && !cpu.cpu_halt_; ++i) { cpu.step(); }
}
