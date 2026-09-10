#include "cpu.hpp"
#include <iomanip>
#include <iostream>
#include <format>

CPU::CPU(Bus& bus) : bus_(bus) {}

void CPU::reset() {
    reg_a_ = 0;
    reg_x_ = 0;
    reg_y_ = 0;

    status_ = InterruptDisable | Unused;
    sp_ = 0xFD;

    const std::uint8_t low = bus_.cpu_memRead(0xFFFC);
    const std::uint8_t high = bus_.cpu_memRead(0xFFFD);

    pc_ = static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);

    total_cycles_ = 7;
}

void CPU::push(std::uint8_t val) {
    bus_.cpu_memWrite(static_cast<std::uint16_t>(0x0100 | sp_), val);
    --sp_;
}

std::uint8_t CPU::pop() {
    ++sp_;
    return bus_.cpu_memRead(static_cast<std::uint16_t>(0x0100 | sp_));
}

std::uint16_t CPU::read_16_from_pc() {
    const std::uint8_t low = bus_.cpu_memRead(pc_++);
    const std::uint8_t high = bus_.cpu_memRead(pc_++);

    return static_cast<std::uint16_t>(low) |  (static_cast<std::uint16_t>(high) << 8);
}

void CPU::push16(std::uint16_t val) {
    push(static_cast<std::uint8_t>(val >> 8));
    push(static_cast<std::uint8_t>(val & 0xFF));
}

std::uint16_t CPU::pop16() {
    const std::uint8_t low = pop();
    const std::uint8_t high = pop();

    return static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);
}

void CPU::set_flag(StatusFlag flag, bool val) {
    const auto mask = static_cast<std::uint8_t>(flag);

    if (val) {
        status_ |= mask;
    } else {
        status_ &= static_cast<std::uint8_t>(~mask);
    }
}

bool CPU::get_flag(StatusFlag flag) const {
    return (status_ & static_cast<std::uint8_t>(flag)) != 0;
}

void CPU::update_nz_flags(std::uint8_t val) {
    set_flag(Zero, val == 0);
    set_flag(Negative, (val & 0x80) != 0);
}

void CPU::add_to_accumulator(uint8_t val) {
    std::uint16_t sum = static_cast<std::uint16_t>(reg_a_) + static_cast<std::uint16_t>(val) + (get_flag(Carry) ? 1 : 0);
    const std::uint8_t result = static_cast<std::uint8_t>(sum);
    set_flag(Carry, sum > 0xFF);
    set_flag(Overflow, (~(reg_a_ ^ val) & (reg_a_ ^ result) & 0x80) != 0);
    reg_a_ = result;
    update_nz_flags(reg_a_);
}

void CPU::compare_register(uint8_t reg, uint8_t val) {
    set_flag(Carry, reg >= val);
    update_nz_flags(static_cast<std::uint8_t>(reg - val));
}

void CPU::branch(bool condition, const AddressResult& operand, std::uint8_t& cycles) {
    if (!condition) {
        return;
    }

    ++cycles;

    if (operand.page_crossed) {
        ++cycles;
    }

    pc_ = operand.address;
}


CPU::AddressResult CPU::resolve_address(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Implied:
        case AddressingMode::Accumulator:
            return {};
        
        case AddressingMode::Immediate:
            return {pc_++, false};
        case AddressingMode::ZeroPage: {
            const std::uint8_t address = bus_.cpu_memRead(pc_++);
            return {address, false};
        };
        case AddressingMode::ZeroPageX: {
            const std::uint8_t base = bus_.cpu_memRead(pc_++);
            const std::uint8_t address = static_cast<uint8_t>(base + reg_x_);
            return {address, false};
        };
        case AddressingMode::ZeroPageY: {
            const std::uint8_t base = bus_.cpu_memRead(pc_++);
            const std::uint8_t address = static_cast<uint8_t>(base + reg_y_);
            return {address, false};
        };
        case AddressingMode::Relative: {
            const std::uint8_t raw_offset = static_cast<std::uint8_t>(bus_.cpu_memRead(pc_++));
            const std::int16_t offset = raw_offset < 0x80 ? static_cast<std::int16_t>(raw_offset) : static_cast<std::int16_t>(raw_offset) - 0x100;
            const std::uint16_t base = pc_;
            const std::uint16_t relative_address = static_cast<uint16_t>(static_cast<uint32_t>(base) + offset);
            return { relative_address, (base & 0xFF00) != (relative_address & 0xFF00)};
        };
        case AddressingMode::Absolute: {
            return {read_16_from_pc(), false};
        };
        case AddressingMode::AbsoluteX: {
            const std::uint16_t base = read_16_from_pc();
            const std::uint16_t address = static_cast<uint16_t>(base + reg_x_);
            return {address, (base & 0xFF00) != (address & 0xFF00)};
        };
        case AddressingMode::AbsoluteY: {
            const std::uint16_t base = read_16_from_pc();
            const std::uint16_t address = static_cast<uint16_t>(base + reg_y_);
            return {address, (base & 0xFF00) != (address & 0xFF00)};
        };
        case AddressingMode::Indirect: {
            const std::uint16_t ptr = read_16_from_pc();
            const std::uint8_t low = bus_.cpu_memRead(ptr);
            const std::uint16_t high_address = (ptr & 0xFF00) | static_cast<std::uint8_t>(ptr + 1);
            const std::uint8_t high = bus_.cpu_memRead(high_address);
            const std::uint16_t address = static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);
            return {address, false};
        }
        case AddressingMode::IndexedIndirect: {
            const std::uint8_t base = bus_.cpu_memRead(pc_++);
            const std::uint16_t ptr = static_cast<std::uint8_t>(base + reg_x_);
            const std::uint16_t next_ptr = static_cast<std::uint8_t>(ptr + 1);
            const std::uint8_t low = bus_.cpu_memRead(ptr);
            const std::uint8_t high = bus_.cpu_memRead(next_ptr);
            const std::uint16_t address = static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);
            return {address, false};
        }
        case AddressingMode::IndirectIndexed: {
            const std::uint8_t ptr = bus_.cpu_memRead(pc_++);
            const std::uint16_t next_ptr = static_cast<std::uint8_t>(ptr + 1);
            const std::uint8_t low = bus_.cpu_memRead(ptr);
            const std::uint8_t high = bus_.cpu_memRead(next_ptr);
            const std::uint16_t base = static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);
            const std::uint16_t address = static_cast<std::uint16_t>(base + reg_y_);
            return {address, (base & 0xFF00) != (address & 0xFF00)};
        }
    }

    throw std::runtime_error(
        "Unhandled addressing mode"
    );
}

std::uint8_t CPU::read_operand(AddressingMode mode, const AddressResult& operand) {
    if (mode == AddressingMode::Accumulator) {
        return reg_a_;
    }

    return bus_.cpu_memRead(operand.address);
}

void CPU::write_operand(AddressingMode mode, const AddressResult& operand, std::uint8_t val) {
    if (mode == AddressingMode::Accumulator) {
        reg_a_ = val;
        return;
    }

    bus_.cpu_memWrite(operand.address, val);
}

void CPU::op_asl(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t val = read_operand(mode, operand);
    set_flag(Carry, (val & 0x80) != 0);
    const std::uint8_t result = static_cast<std::uint8_t>(val << 1);
    write_operand(mode, operand, result);
    update_nz_flags(result);
}

void CPU::op_ora(AddressingMode mode, const AddressResult& operand) {
    reg_a_ |= read_operand(mode, operand);
    update_nz_flags(reg_a_);
}

void CPU::op_rol(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t val = read_operand(mode, operand);
    const bool old_carry = get_flag(Carry);
    set_flag(Carry, (val & 0x80) != 0);
    const std::uint8_t result = static_cast<std::uint8_t>((val << 1) | (old_carry ? 1 : 0));
    write_operand(mode, operand, result);
    update_nz_flags(result);
}

void CPU::op_and(AddressingMode mode, const AddressResult& operand) {
    reg_a_ &= read_operand(mode, operand);
    update_nz_flags(reg_a_);
}

void CPU::op_adc(AddressingMode mode, const AddressResult& operand) {
    add_to_accumulator(read_operand(mode, operand));
}

void CPU::op_cmp(AddressingMode mode, const AddressResult& operand) {
    compare_register(reg_a_, read_operand(mode, operand));
}

void CPU::op_dec(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t result = static_cast<std::uint8_t>(read_operand(mode, operand) - 1);
    write_operand(mode , operand, result);
    update_nz_flags(result);
}

void CPU::op_eor(AddressingMode mode, const AddressResult& operand) {
    reg_a_ ^= read_operand(mode, operand);
    update_nz_flags(reg_a_);
}

void CPU::op_lda(AddressingMode mode, const AddressResult& operand) {
    reg_a_ = read_operand(mode, operand);
    update_nz_flags(reg_a_);
}

void CPU::op_ldx(AddressingMode mode, const AddressResult& operand) {
    reg_x_ = read_operand(mode, operand);
    update_nz_flags(reg_x_);
}


void CPU::op_lsr(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t val = read_operand(mode, operand);
    set_flag(Carry, (val & 0x01) != 0);
    const std::uint8_t result = static_cast<std::uint8_t>(val >> 1);
    write_operand(mode, operand, result);
    update_nz_flags(result);
}


void CPU::op_ror(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t val = read_operand(mode, operand);
    const bool old_carry = get_flag(Carry);
    set_flag(Carry, (val & 0x01) != 0);
    const std::uint8_t result = static_cast<std::uint8_t>((val >> 1) | (old_carry ? 0x80 : 0));
    write_operand(mode, operand, result);
    update_nz_flags(result);
}


void CPU::op_inc(AddressingMode mode, const AddressResult& operand) {
    const std::uint8_t result = static_cast<std::uint8_t>(read_operand(mode, operand) + 1);
    write_operand(mode, operand, result);
    update_nz_flags(result);
}

void CPU::op_sbc(AddressingMode mode, const AddressResult& operand) {
    add_to_accumulator(read_operand(mode, operand) ^ 0xFF);
}


void CPU::execute(Operation opcode, AddressingMode mode, const AddressResult& operand, std::uint8_t& cycles) {

    switch (opcode) {
        case Operation::ADC:
            add_to_accumulator(read_operand(mode, operand));
            break;
        case Operation::AND:
            op_and(mode, operand);
            break;
        case Operation::ASL: {
            op_asl(mode, operand);
            break;
        };
        case Operation::BCC:
            branch(!get_flag(Carry), operand, cycles);
            break;
        case Operation::BCS:
            branch(get_flag(Carry), operand, cycles);
            break;
        case Operation::BEQ:
            branch(get_flag(Zero), operand, cycles);
            break;
        case Operation::BIT: {
            const std::uint8_t val = read_operand(mode, operand);
            set_flag(Zero, (reg_a_ & val) == 0);
            set_flag(Overflow, (val & 0x40) != 0);
            set_flag(Negative, (val & 0x80) != 0);
            break;
        }
        case Operation::BMI:
            branch(get_flag(Negative), operand, cycles);
            break;
        case Operation::BNE:
            branch(!get_flag(Zero), operand, cycles);
            break;
        case Operation::BPL:
            branch(!get_flag(Negative), operand, cycles);
            break;
        case Operation::BRK: {
            if (!DoNMI) {
                ++pc_;
            }
            push16(pc_);
            std::uint8_t pushed_status = status_ | Unused;
            if (DoNMI) {
                pushed_status &= static_cast<std::uint8_t>(~Break);
            } else {
                pushed_status |= Break;
            }

            push(pushed_status);
            set_flag(InterruptDisable, true);
            const std::uint8_t low = bus_.cpu_memRead(DoNMI ? 0xFFFA : 0xFFFE);
            const std::uint8_t high = bus_.cpu_memRead(static_cast<std::uint16_t>(DoNMI ? 0xFFFB : 0xFFFF));
            pc_ = static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8);
            DoNMI = false;
            break;
        };
        case Operation::BVC:
            branch(!get_flag(Overflow), operand, cycles);
            break;
        case Operation::BVS:
            branch(get_flag(Overflow), operand, cycles);
            break;
        case Operation::CLC:
            set_flag(Carry, false);
            break;
        case Operation::CLD:
            set_flag(Decimal, false);
            break;
        case Operation::CLI:
            set_flag(InterruptDisable, false);
            break;
        case Operation::CLV:
            set_flag(Overflow, false);
            break;
        case Operation::CMP:
            compare_register(reg_a_, read_operand(mode, operand));
            break;
        case Operation::CPX:
            compare_register(reg_x_, read_operand(mode, operand));
            break;
        case Operation::CPY:
            compare_register(reg_y_, read_operand(mode, operand));
            break;
        case Operation::DEC: {
            op_dec(mode, operand);
            break;
        };
        case Operation::DEX:
            --reg_x_;
            update_nz_flags(reg_x_);
            break;
        case Operation::DEY:
            --reg_y_;
            update_nz_flags(reg_y_);
            break;
        case Operation::EOR: 
            op_eor(mode, operand);
            break;
        case Operation::HLT:
            cpu_halt_ = true;
            break;
        case Operation::INC: {
            op_inc(mode, operand);
            break;
        }
        case Operation::INX:
            ++reg_x_;
            update_nz_flags(reg_x_);
            break;
        case Operation::INY:
            ++reg_y_;
            update_nz_flags(reg_y_);
            break;
        case Operation::JMP:
            pc_ = operand.address;
            break;
        case Operation::JSR:
            push16(static_cast<std::uint16_t>(pc_ - 1));
            pc_ = operand.address;
            break;
        case Operation::LDA:
            op_lda(mode, operand);
            break;
        case Operation::LDX:
            op_ldx(mode, operand);
            break;
        case Operation::LDY:
            reg_y_ = read_operand(mode, operand);
            update_nz_flags(reg_y_);
            break;
        case Operation::LSR: {
            op_lsr(mode, operand);
            break;
        };
        case Operation::NOP: {
            if (mode != AddressingMode::Implied) {
                read_operand(mode, operand);
            }
            break;
        };
        case Operation::ORA:
            op_ora(mode, operand);
            break;
        case Operation::PHA:
            push(reg_a_);
            break;
        case Operation::PHP:
            push(status_ | Break | Unused);
            break;
        case Operation::PLA:
            reg_a_ = pop();
            update_nz_flags(reg_a_);
            break;
        case Operation::PLP:
            status_ = pop();
            status_ &= ~Break;
            status_ |= Unused;
            break;
        case Operation::ROL: {
            op_rol(mode, operand);
            break;
        }
        case Operation::ROR: {
            op_ror(mode, operand);
            break;
        }
        case Operation::RTI:
            status_ = pop();
            set_flag(Break, false);
            set_flag(Unused, true);
            pc_ = pop16();
            break;
        case Operation::RTS:
            pc_ = static_cast<std::uint16_t>(pop16() + 1);
            break;
        case Operation::SBC:
            op_sbc(mode, operand);
            break;
        case Operation::SEC:
            set_flag(Carry, true);
            break;
        case Operation::SED:
            set_flag(Decimal, true);
            break;
        case Operation::SEI:
            set_flag(InterruptDisable, true);
            break;
        case Operation::STA:
            write_operand(mode, operand, reg_a_);
            break;
        case Operation::STX:
            write_operand(mode, operand, reg_x_);
            break;
        case Operation::STY:
            write_operand(mode, operand, reg_y_);
            break;
        case Operation::TAX:
            reg_x_ = reg_a_;
            update_nz_flags(reg_x_);
            break;
        case Operation::TAY:
            reg_y_ = reg_a_;
            update_nz_flags(reg_y_);
            break;
        case Operation::TSX:
            reg_x_ = sp_;
            update_nz_flags(reg_x_);
            break;
        case Operation::TXA:
            reg_a_ = reg_x_;
            update_nz_flags(reg_a_);
            break;
        case Operation::TXS:
            sp_ = reg_x_;
            break;
        case Operation::TYA:
            reg_a_ = reg_y_;
            update_nz_flags(reg_a_);
            break;
        case Operation::SLO: 
            op_asl(mode, operand);
            op_ora(mode, operand);
            break;
        case Operation::RLA: 
            op_rol(mode, operand);
            op_and(mode, operand);
            break;
        case Operation::SAX: {
            std::uint8_t val = reg_a_ & reg_x_;
            write_operand(mode, operand, val);
            break;
        }
        case Operation::SRE:
            op_lsr(mode, operand);
            op_eor(mode, operand);
            break;
        case Operation::RRA: 
            op_ror(mode, operand);
            op_adc(mode, operand);
            break;
        case Operation::LAX: 
            op_lda(mode, operand);
            op_ldx(mode, operand);
            break;
        case Operation::DCP: 
            op_dec(mode, operand);
            op_cmp(mode, operand);
            break;
        case Operation::ISC:
            op_inc(mode, operand);
            op_sbc(mode, operand);
            break;
        case Operation::ANC:
            reg_a_ &= read_operand(mode, operand);
            set_flag(Carry, reg_a_ & 0x80);
            update_nz_flags(reg_a_);
            break;
        case Operation::ASR:
            reg_a_ &= read_operand(mode, operand);
            set_flag(Carry, reg_a_ & 0x01);
            reg_a_ >>= 1;
            update_nz_flags(reg_a_);
            break;
        case Operation::ARR: {
            reg_a_ &= read_operand(mode, operand);
            bool old_carry = get_flag(Carry);
            reg_a_ = (reg_a_ >> 1) | (old_carry << 7);
            set_flag(Carry, reg_a_ & 0x40);
            set_flag(Overflow, ((reg_a_ >> 6) ^ (reg_a_ >> 5)) & 1);
            update_nz_flags(reg_a_);
            break;
        }
        case Operation::ANE:
            reg_a_ = reg_x_ & read_operand(mode, operand);
            update_nz_flags(reg_a_);
            break;
        case Operation::LXA:
            reg_a_ = read_operand(mode, operand);
            reg_x_ = reg_a_;
            update_nz_flags(reg_a_);
            break;
        case Operation::AXS: {
            std::uint8_t val = read_operand(mode, operand);
            std::uint8_t and_ax = (reg_a_ & reg_x_);
            reg_x_ = static_cast<std::uint8_t>(and_ax - val);
            compare_register(and_ax, val);
            break;
        }
        case Operation::Invalid: {
            //throw std::runtime_error("Attempted to execute invalid opcode");
        }
    }    
}

std::uint8_t CPU::step() {

    bool prevNMILevel = NMILevelDetector;
    const bool new_nmi_level = bus_.calc_nmi_vblank();
    if(!prevNMILevel && new_nmi_level) {
        DoNMI = true;
        ++nmi_count_;
    }
    NMILevelDetector = new_nmi_level;

    if (!DoNMI) {
        curr_op_ = bus_.cpu_memRead(pc_++);
    } else {
        curr_op_ = 0x00;
    }

    const Opcode& opcode = opcode_table[curr_op_];

    if (opcode.operation == Operation::Invalid) {
        //throw std::runtime_error("Unhandled opcode");
    }

    AddressResult operand = resolve_address(opcode.mode);
    std::uint8_t cycles = opcode.cycles;

    if (opcode.page_crossed && operand.page_crossed) {
        ++cycles;
    }

    execute(opcode.operation, opcode.mode, operand, cycles);

    set_flag(Break, false);
    set_flag(Unused, true);

    total_cycles_ += cycles;
    return cycles;
}
