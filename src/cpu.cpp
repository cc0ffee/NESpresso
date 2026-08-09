#include "cpu.hpp"
#include <stdexcept>

void CPU::reset() {
    reg_a_ = 0;
    reg_x_ = 0;
    reg_y_ = 0;
    pc_ = 0x8000;
    status_ = 0x24;
}

std::uint8_t CPU::step() {
    const std::uint8_t opcode = memory_[pc_++];
    switch (opcode) {
        case 0xEA:
            return 2;
        case 0xA9:
            reg_a_ = memory_[pc_++];
            return 2;
        default:
            throw std::runtime_error("Unhandled opcode");
    }
}
