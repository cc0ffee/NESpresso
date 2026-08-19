#pragma once
#include "bus.hpp"
#include "opcodes.hpp"

class CPU {
public:
    explicit CPU(Bus& bus);
    void reset();

    std::uint8_t step();
    bool cpu_halt_{false};

    bool NMILevelDetector{false};
    bool DoNMI{false};
    std::uint64_t nmi_count_{0};

private:
    void Tracelogger(std::uint16_t instruction_pc, const Opcode& opcode);
    enum StatusFlag : std::uint8_t {
        Carry = 1U << 0,
        Zero = 1U << 1,
        InterruptDisable = 1U << 2,
        Decimal = 1U << 3,
        Break = 1U << 4,
        Unused = 1U << 5,
        Overflow = 1U << 6,
        Negative = 1U << 7
    };

    struct AddressResult {
        std::uint16_t address = 0;
        bool page_crossed = false;
    };

    Bus& bus_;

    bool get_flag(StatusFlag flag) const;
    void set_flag(StatusFlag flag, bool val);
    void update_nz_flags(std::uint8_t val);

    void add_to_accumulator(uint8_t val);
    void compare_register(uint8_t reg, uint8_t val);
    void branch(bool condition, const AddressResult& operand, std::uint8_t& cycles);

    void push(std::uint8_t val);
    std::uint8_t pop();

    void push16(std::uint16_t val);
    std::uint16_t pop16();

    std::uint16_t read_16_from_pc();

    AddressResult resolve_address(AddressingMode mode);

    std::uint8_t read_operand(AddressingMode mode, const AddressResult& operand);
    void write_operand(AddressingMode mode, const AddressResult& operand, std::uint8_t val);
    void execute(Operation opcode, AddressingMode mode, const AddressResult& operand, std::uint8_t& cycles);

    std::uint8_t reg_a_{0};
    std::uint8_t reg_x_{0};
    std::uint8_t reg_y_{0};

    std::uint8_t sp_{0xFD};
    std::uint16_t pc_{0};
    std::uint8_t status_{0x24};

    std::uint8_t curr_op_{0};

    std::uint64_t total_cycles_{0};

};
