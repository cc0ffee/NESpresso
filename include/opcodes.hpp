#pragma once
#include <array>
#include <cstdint>
#include <string_view>

enum class AddressingMode : std::uint8_t {
    Implied,
    Accumulator,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndexedIndirect,
    IndirectIndexed
};

enum class Operation : std::uint8_t {
    Invalid,ADC,AND,ASL,BCC,BCS,BEQ,BIT,BMI,BNE,BPL,BRK,BVC,BVS,
    CLC,CLD,CLI,CLV,CMP,CPX,CPY,DEC,DEX,DEY,EOR,HLT,INC,INX,INY,JMP,
    JSR,LDA,LDX,LDY,LSR,NOP,ORA,PHA,PHP,PLA,PLP,ROL,ROR,RTI,RTS,
    SBC,SEC,SED,SEI,STA,STX,STY,TAX,TAY,TSX,TXA,TXS,TYA
};

struct Opcode {
    std::string_view name = "???";
    Operation operation = Operation::Invalid;
    AddressingMode mode = AddressingMode::Implied;

    std::uint8_t length = 1;
    std::uint8_t cycles = 0;

    bool page_crossed = false;
};

inline constexpr auto opcode_table = [] {
    std::array<Opcode, 256> table{};

    auto set = [&table](
        std::uint8_t code,
        std::string_view name,
        Operation operation,
        AddressingMode mode,
        std::uint8_t length,
        std::uint8_t cycles,
        bool page_crossed = false
    ) {
        table[code] = {
            name,
            operation,
            mode,
            length,
            cycles,
            page_crossed
        };
    };

    // ADC
    set(0x69, "ADC", Operation::ADC, AddressingMode::Immediate, 2, 2);
    set(0x65, "ADC", Operation::ADC, AddressingMode::ZeroPage, 2, 3);
    set(0x75, "ADC", Operation::ADC, AddressingMode::ZeroPageX, 2, 4);
    set(0x6D, "ADC", Operation::ADC, AddressingMode::Absolute, 3, 4);
    set(0x7D, "ADC", Operation::ADC, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x79, "ADC", Operation::ADC, AddressingMode::AbsoluteY, 3, 4, true);
    set(0x61, "ADC", Operation::ADC, AddressingMode::IndexedIndirect, 2, 6);
    set(0x71, "ADC", Operation::ADC, AddressingMode::IndirectIndexed, 2, 5, true);

    // AND
    set(0x29, "AND", Operation::AND, AddressingMode::Immediate, 2, 2);
    set(0x25, "AND", Operation::AND, AddressingMode::ZeroPage, 2, 3);
    set(0x35, "AND", Operation::AND, AddressingMode::ZeroPageX, 2, 4);
    set(0x2D, "AND", Operation::AND, AddressingMode::Absolute, 3, 4);
    set(0x3D, "AND", Operation::AND, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x39, "AND", Operation::AND, AddressingMode::AbsoluteY, 3, 4, true);
    set(0x21, "AND", Operation::AND, AddressingMode::IndexedIndirect, 2, 6);
    set(0x31, "AND", Operation::AND, AddressingMode::IndirectIndexed, 2, 5, true);

    // ASL

    // Branches

    // BIT
    set(0x24, "BIT", Operation::BIT, AddressingMode::ZeroPage, 2, 3);
    set(0x2C, "BIT", Operation::BIT, AddressingMode::Absolute, 3, 4);

    // BRK

    // Flag instructions

    // CMP
    set(0xC9, "CMP", Operation::CMP, AddressingMode::Immediate, 2, 2);
    set(0xC5, "CMP", Operation::CMP, AddressingMode::ZeroPage, 2, 3);
    set(0xD5, "CMP", Operation::CMP, AddressingMode::ZeroPageX, 2, 4);
    set(0xCD, "CMP", Operation::CMP, AddressingMode::Absolute, 3, 4);
    set(0xDD, "CMP", Operation::CMP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0xD9, "CMP", Operation::CMP, AddressingMode::AbsoluteY, 3, 4, true);
    set(0xC1, "CMP", Operation::CMP, AddressingMode::IndexedIndirect,  2, 6);
    set(0xD1, "CMP", Operation::CMP, AddressingMode::IndirectIndexed, 2, 5, true);

    // CPX
    set(0xE0, "CPX", Operation::CPX, AddressingMode::Immediate, 2, 2);
    set(0xE4, "CPX", Operation::CPX, AddressingMode::ZeroPage, 2, 3);
    set(0xEC, "CPX", Operation::CPX, AddressingMode::Absolute, 3, 4);

    // CPY
    set(0xC0, "CPY", Operation::CPY, AddressingMode::Immediate, 2, 2);
    set(0xC4, "CPY", Operation::CPY, AddressingMode::ZeroPage, 2, 3);
    set(0xCC, "CPY", Operation::CPY, AddressingMode::Absolute, 3, 4);

    // DEC
    set(0xC6, "DEC", Operation::DEC, AddressingMode::ZeroPage,  2, 5);
    set(0xD6, "DEC", Operation::DEC, AddressingMode::ZeroPageX, 2, 6);
    set(0xCE, "DEC", Operation::DEC, AddressingMode::Absolute, 3, 6);
    set(0xDE, "DEC", Operation::DEC, AddressingMode::AbsoluteX, 3, 7);

    set(0xCA, "DEX", Operation::DEX, AddressingMode::Implied, 1, 2);
    set(0x88, "DEY", Operation::DEY, AddressingMode::Implied, 1, 2);

    // EOR
    set(0x49, "EOR", Operation::EOR, AddressingMode::Immediate, 2, 2);
    set(0x45, "EOR", Operation::EOR, AddressingMode::ZeroPage, 2, 3);
    set(0x55, "EOR", Operation::EOR, AddressingMode::ZeroPageX, 2, 4);
    set(0x4D, "EOR", Operation::EOR, AddressingMode::Absolute, 3, 4);
    set(0x5D, "EOR", Operation::EOR, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x59, "EOR", Operation::EOR, AddressingMode::AbsoluteY, 3, 4, true);
    set(0x41, "EOR", Operation::EOR, AddressingMode::IndexedIndirect,2, 6);
    set(0x51, "EOR", Operation::EOR, AddressingMode::IndirectIndexed, 2, 5, true);

    // HLT
    set(0x02, "HLT", Operation::HLT, AddressingMode::Implied, 2, 2);

    // INC
    set(0xE6, "INC", Operation::INC, AddressingMode::ZeroPage, 2, 5);
    set(0xF6, "INC", Operation::INC, AddressingMode::ZeroPageX, 2, 6);
    set(0xEE, "INC", Operation::INC, AddressingMode::Absolute, 3, 6);
    set(0xFE, "INC", Operation::INC, AddressingMode::AbsoluteX, 3, 7);

    set(0xE8, "INX", Operation::INX, AddressingMode::Implied, 1, 2);
    set(0xC8, "INY", Operation::INY, AddressingMode::Implied, 1, 2);

    // JMP / JSR

    // LDA
    set(0xA9, "LDA", Operation::LDA, AddressingMode::Immediate, 2, 2);
    set(0xA5, "LDA", Operation::LDA, AddressingMode::ZeroPage, 2, 3);
    set(0xB5, "LDA", Operation::LDA, AddressingMode::ZeroPageX, 2, 4);
    set(0xAD, "LDA", Operation::LDA, AddressingMode::Absolute, 3, 4);
    set(0xBD, "LDA", Operation::LDA, AddressingMode::AbsoluteX, 3, 4, true);
    set(0xB9, "LDA", Operation::LDA, AddressingMode::AbsoluteY, 3, 4, true);
    set(0xA1, "LDA", Operation::LDA, AddressingMode::IndexedIndirect, 2, 6);
    set(0xB1, "LDA", Operation::LDA, AddressingMode::IndirectIndexed, 2, 5, true);

    // LDX
    set(0xA2, "LDX", Operation::LDX, AddressingMode::Immediate, 2, 2);
    set(0xA6, "LDX", Operation::LDX, AddressingMode::ZeroPage, 2, 3);
    set(0xB6, "LDX", Operation::LDX, AddressingMode::ZeroPageY, 2, 4);
    set(0xAE, "LDX", Operation::LDX, AddressingMode::Absolute, 3, 4);
    set(0xBE, "LDX", Operation::LDX, AddressingMode::AbsoluteY, 3, 4, true);

    // LDY
    set(0xA0, "LDY", Operation::LDY, AddressingMode::Immediate, 2, 2);
    set(0xA4, "LDY", Operation::LDY, AddressingMode::ZeroPage, 2, 3);
    set(0xB4, "LDY", Operation::LDY, AddressingMode::ZeroPageX, 2, 4);
    set(0xAC, "LDY", Operation::LDY, AddressingMode::Absolute, 3, 4);
    set(0xBC, "LDY", Operation::LDY, AddressingMode::AbsoluteX, 3, 4, true);

    // LSR

    // NOP
    set(0xEA, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);

    // ORA
    set(0x09, "ORA", Operation::ORA, AddressingMode::Immediate, 2, 2);
    set(0x05, "ORA", Operation::ORA, AddressingMode::ZeroPage, 2, 3);
    set(0x15, "ORA", Operation::ORA, AddressingMode::ZeroPageX, 2, 4);
    set(0x0D, "ORA", Operation::ORA, AddressingMode::Absolute, 3, 4);
    set(0x1D, "ORA", Operation::ORA, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x19, "ORA", Operation::ORA, AddressingMode::AbsoluteY, 3, 4, true);
    set(0x01, "ORA", Operation::ORA, AddressingMode::IndexedIndirect, 2, 6);
    set(0x11, "ORA", Operation::ORA, AddressingMode::IndirectIndexed, 2, 5, true);

    // Stack

    // RLA

    // ROL

    // ROR

    // Returns

    // SBC
    set(0xE9, "SBC", Operation::SBC, AddressingMode::Immediate, 2, 2);
    set(0xE5, "SBC", Operation::SBC, AddressingMode::ZeroPage, 2, 3);
    set(0xF5, "SBC", Operation::SBC, AddressingMode::ZeroPageX, 2, 4);
    set(0xED, "SBC", Operation::SBC, AddressingMode::Absolute, 3, 4);
    set(0xFD, "SBC", Operation::SBC, AddressingMode::AbsoluteX, 3, 4, true);
    set(0xF9, "SBC", Operation::SBC, AddressingMode::AbsoluteY, 3, 4, true);
    set(0xE1, "SBC", Operation::SBC, AddressingMode::IndexedIndirect, 2, 6);
    set(0xF1, "SBC", Operation::SBC, AddressingMode::IndirectIndexed, 2, 5, true);

    // Set flags

    // STA
    set(0x85, "STA", Operation::STA, AddressingMode::ZeroPage, 2, 3);
    set(0x95, "STA", Operation::STA, AddressingMode::ZeroPageX,2, 4);
    set(0x8D, "STA", Operation::STA, AddressingMode::Absolute, 3, 4);
    set(0x9D, "STA", Operation::STA, AddressingMode::AbsoluteX, 3, 5);
    set(0x99, "STA", Operation::STA, AddressingMode::AbsoluteY, 3, 5);
    set(0x81, "STA", Operation::STA, AddressingMode::IndexedIndirect, 2, 6);
    set(0x91, "STA", Operation::STA, AddressingMode::IndirectIndexed, 2, 6);

    // STX
    set(0x86, "STX", Operation::STX, AddressingMode::ZeroPage, 2, 3);
    set(0x96, "STX", Operation::STX, AddressingMode::ZeroPageY, 2, 4);
    set(0x8E, "STX", Operation::STX, AddressingMode::Absolute, 3, 4);

    // STY
    set(0x84, "STY", Operation::STY, AddressingMode::ZeroPage, 2, 3);
    set(0x94, "STY", Operation::STY, AddressingMode::ZeroPageX, 2, 4);
    set(0x8C, "STY", Operation::STY, AddressingMode::Absolute, 3, 4);

    // Register transfers
    set(0xAA, "TAX", Operation::TAX, AddressingMode::Implied, 1, 2);
    set(0xA8, "TAY", Operation::TAY, AddressingMode::Implied, 1, 2);
    set(0xBA, "TSX", Operation::TSX, AddressingMode::Implied, 1, 2);
    set(0x8A, "TXA", Operation::TXA, AddressingMode::Implied, 1, 2);
    set(0x9A, "TXS", Operation::TXS, AddressingMode::Implied, 1, 2);
    set(0x98, "TYA", Operation::TYA, AddressingMode::Implied, 1, 2);

    // BELOW ARE UNOFFICAL OPCODES



    return table;
}();
