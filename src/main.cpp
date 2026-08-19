#include "cpu.hpp"

int main() {
    Bus bus;
    bus.cpu_memWrite(0xFFFC, 0);
    bus.cpu_memWrite(0xFFFD, 0x80);
    bus.cpu_memWrite(0x8000, 0xA9);
    bus.cpu_memWrite(0x8001, 0x42);
    bus.cpu_memWrite(0x8002, 0x85);
    bus.cpu_memWrite(0x8003, 0x10);
    CPU cpu(bus);
    cpu.reset();
    cpu.step(); cpu.step();
    return bus.cpu_memRead(0x10) == 0x42 ? 0 : 1;
}
