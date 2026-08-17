#include "cpu.hpp"
#include <stdexcept>

void CPU::reset() {
    reg_a_ = 0;
    reg_x_ = 0;
    reg_y_ = 0;
    pc_ = 0x8000;
    status_ = 0x24;
}

void CPU::update_nz_flags(std::uint8_t val) {
    status_ = static_cast<std::uint8_t>(
        (status_ & 0x7D) | (val == 0 ? 0x02 : 0) | (val & 0x80)
    );
}

std::uint8_t CPU::step() {
    const std::uint8_t opcode = memory_[pc_++];
    switch (opcode) {
        case 0xEA:
            return 2;
        case 0xA9:
            reg_a_ = memory_[pc_++];
            update_nz_flags(reg_x_);
            return 2;
        case 0xA5:
            reg_a_ = memory_[memory_[pc_++]];
            update_nz_flags(reg_x_);
            return 3;
        case 0x85:
            memory_[memory_[pc_++]] = reg_a_;
            return 3;
        case 0xAD: {
            const std::uint8_t low = memory_[pc_++];
            const std::uint8_t high = memory_[pc_++];
            reg_a_ = memory_[low | (high << 8)];
            update_nz_flags(reg_a_);
            return 4;
        }
        case 0x8D: {
            const std::uint8_t low = memory_[pc_++];
            const std::uint8_t high = memory_[pc_++];
            memory_[low | (high << 8)] = reg_a_;
            return 4;
        }
        case 0xAA:
            reg_x_ = reg_a_;
            update_nz_flags(reg_x_);
            return 2;
        case 0x8A:
            reg_a_ = reg_x_;
            update_nz_flags(reg_a_);
            return 2;
        case 0xE8:
            ++reg_x_;
            update_nz_flags(reg_x_);
            return 2;
        case 0xCA:
            --reg_x_;
            update_nz_flags(reg_x_);
            return 2;
        default:
            throw std::runtime_error("Unhandled opcode");
    }
}
