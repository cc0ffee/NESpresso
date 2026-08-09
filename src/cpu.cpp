#include "cpu.hpp"
#include <stdexcept>

void CPU::reset() {
    reg_a_ = 0;
    reg_x_ = 0;
    reg_y_ = 0;
    pc_ = 0x8000;
    status_ = 0x24;
}
