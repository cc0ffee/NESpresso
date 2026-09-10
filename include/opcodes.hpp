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
    SBC,SEC,SED,SEI,STA,STX,STY,TAX,TAY,TSX,TXA,TXS,TYA,

    SLO,RLA,SRE,RRA,SAX,LAX,DCP,ISC,SHA,SHS,SHY,SHX,LAE,
    ANC,ASR,ARR,ANE,LXA,AXS
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
    set(0x0A, "ASL", Operation::ASL, AddressingMode::Accumulator, 1, 2);
    set(0x06, "ASL", Operation::ASL, AddressingMode::ZeroPage, 2, 5);
    set(0x16, "ASL", Operation::ASL, AddressingMode::ZeroPageX, 2, 6);
    set(0x0E, "ASL", Operation::ASL, AddressingMode::Absolute, 3, 6);
    set(0x1E, "ASL", Operation::ASL, AddressingMode::AbsoluteX, 3, 7);

    // Branches
    set(0x90, "BCC", Operation::BCC, AddressingMode::Relative, 2, 2);
    set(0xB0, "BCS", Operation::BCS, AddressingMode::Relative, 2, 2);
    set(0xF0, "BEQ", Operation::BEQ, AddressingMode::Relative, 2, 2);
    set(0x30, "BMI", Operation::BMI, AddressingMode::Relative, 2, 2);
    set(0xD0, "BNE", Operation::BNE, AddressingMode::Relative, 2, 2);
    set(0x10, "BPL", Operation::BPL, AddressingMode::Relative, 2, 2);
    set(0x50, "BVC", Operation::BVC, AddressingMode::Relative, 2, 2);
    set(0x70, "BVS", Operation::BVS, AddressingMode::Relative, 2, 2);

    // BIT
    set(0x24, "BIT", Operation::BIT, AddressingMode::ZeroPage, 2, 3);
    set(0x2C, "BIT", Operation::BIT, AddressingMode::Absolute, 3, 4);

    // BRK
    set(0x00, "BRK", Operation::BRK, AddressingMode::Implied, 1, 7);

    // Flag instructions
    set(0x18, "CLC", Operation::CLC, AddressingMode::Implied, 1, 2);
    set(0xD8, "CLD", Operation::CLD, AddressingMode::Implied, 1, 2);
    set(0x58, "CLI", Operation::CLI, AddressingMode::Implied, 1, 2);
    set(0xB8, "CLV", Operation::CLV, AddressingMode::Implied, 1, 2);

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
    set(0x4C, "JMP", Operation::JMP, AddressingMode::Absolute, 3, 3);
    set(0x6C, "JMP", Operation::JMP, AddressingMode::Indirect, 3, 5);
    set(0x20, "JSR", Operation::JSR, AddressingMode::Absolute, 3, 6);

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
    set(0x4A, "LSR", Operation::LSR, AddressingMode::Accumulator, 1, 2);
    set(0x46, "LSR", Operation::LSR, AddressingMode::ZeroPage, 2, 5);
    set(0x56, "LSR", Operation::LSR, AddressingMode::ZeroPageX, 2, 6);
    set(0x4E, "LSR", Operation::LSR, AddressingMode::Absolute, 3, 6);
    set(0x5E, "LSR", Operation::LSR, AddressingMode::AbsoluteX, 3, 7);

    // NOP
    set(0x1A, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0x3A, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0x5A, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0x7A, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0xDA, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0xEA, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0xFA, "NOP", Operation::NOP, AddressingMode::Implied, 1, 2);
    set(0x80, "NOP", Operation::NOP, AddressingMode::Immediate, 2, 2);
    set(0x82, "NOP", Operation::NOP, AddressingMode::Immediate, 2, 2);
    set(0x89, "NOP", Operation::NOP, AddressingMode::Immediate, 2, 2);
    set(0xC2, "NOP", Operation::NOP, AddressingMode::Immediate, 2, 2);
    set(0xE2, "NOP", Operation::NOP, AddressingMode::Immediate, 2, 2);
    set(0x04, "NOP", Operation::NOP, AddressingMode::ZeroPage, 2, 3);
    set(0x44, "NOP", Operation::NOP, AddressingMode::ZeroPage, 2, 3);
    set(0x64, "NOP", Operation::NOP, AddressingMode::ZeroPage, 2, 3);
    set(0x14, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0x34, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0x54, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0x74, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0xD4, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0xF4, "NOP", Operation::NOP, AddressingMode::ZeroPageX, 2, 4);
    set(0x0C, "NOP", Operation::NOP, AddressingMode::Absolute, 3, 4);
    set(0x1C, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x3C, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x5C, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0x7C, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0xDC, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);
    set(0xFC, "NOP", Operation::NOP, AddressingMode::AbsoluteX, 3, 4, true);

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
    set(0x48, "PHA", Operation::PHA, AddressingMode::Implied, 1, 3);
    set(0x08, "PHP", Operation::PHP, AddressingMode::Implied, 1, 3);
    set(0x68, "PLA", Operation::PLA, AddressingMode::Implied, 1, 4);
    set(0x28, "PLP", Operation::PLP, AddressingMode::Implied, 1, 4);

    // RLA

    // ROL
    set(0x2A, "ROL", Operation::ROL, AddressingMode::Accumulator, 1, 2);
    set(0x26, "ROL", Operation::ROL, AddressingMode::ZeroPage, 2, 5);
    set(0x36, "ROL", Operation::ROL, AddressingMode::ZeroPageX, 2, 6);
    set(0x2E, "ROL", Operation::ROL, AddressingMode::Absolute, 3, 6);
    set(0x3E, "ROL", Operation::ROL, AddressingMode::AbsoluteX, 3, 7);

    // ROR
    set(0x6A, "ROR", Operation::ROR, AddressingMode::Accumulator, 1, 2);
    set(0x66, "ROR", Operation::ROR, AddressingMode::ZeroPage, 2, 5);
    set(0x76, "ROR", Operation::ROR, AddressingMode::ZeroPageX, 2, 6);
    set(0x6E, "ROR", Operation::ROR, AddressingMode::Absolute, 3, 6);
    set(0x7E, "ROR", Operation::ROR, AddressingMode::AbsoluteX, 3, 7);

    // Returns
    set(0x40, "RTI", Operation::RTI, AddressingMode::Implied, 1, 6);
    set(0x60, "RTS", Operation::RTS, AddressingMode::Implied, 1, 6);

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
    set(0x38, "SEC", Operation::SEC, AddressingMode::Implied, 1, 2);
    set(0xF8, "SED", Operation::SED, AddressingMode::Implied, 1, 2);
    set(0x78, "SEI", Operation::SEI, AddressingMode::Implied, 1, 2);

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

    set(0x07, "SLO", Operation::SLO, AddressingMode::ZeroPage, 2, 5);
    set(0x17, "SLO", Operation::SLO, AddressingMode::ZeroPageX, 2, 6);
    set(0x0F, "SLO", Operation::SLO, AddressingMode::Absolute, 3, 6);
    set(0x1F, "SLO", Operation::SLO, AddressingMode::AbsoluteX, 3, 7);
    set(0x1B, "SLO", Operation::SLO, AddressingMode::AbsoluteY, 3, 7);
    set(0x03, "SLO", Operation::SLO, AddressingMode::IndexedIndirect, 2, 8);
    set(0x13, "SLO", Operation::SLO, AddressingMode::IndirectIndexed, 2, 8);

    set(0x27, "RLA", Operation::RLA, AddressingMode::ZeroPage, 2, 5);
    set(0x37, "RLA", Operation::RLA, AddressingMode::ZeroPageX, 2, 6);
    set(0x2F, "RLA", Operation::RLA, AddressingMode::Absolute, 3, 6);
    set(0x3F, "RLA", Operation::RLA, AddressingMode::AbsoluteX, 3, 7);
    set(0x3B, "RLA", Operation::RLA, AddressingMode::AbsoluteY, 3, 7);
    set(0x23, "RLA", Operation::RLA, AddressingMode::IndexedIndirect, 2, 8);
    set(0x33, "RLA", Operation::RLA, AddressingMode::IndirectIndexed, 2, 8);
    
    set(0x47, "SRE", Operation::SRE, AddressingMode::ZeroPage, 2, 5);
    set(0x57, "SRE", Operation::SRE, AddressingMode::ZeroPageX, 2, 6);
    set(0x4F, "SRE", Operation::SRE, AddressingMode::Absolute, 3, 6);
    set(0x5F, "SRE", Operation::SRE, AddressingMode::AbsoluteX, 3, 7);
    set(0x5B, "SRE", Operation::SRE, AddressingMode::AbsoluteY, 3, 7);
    set(0x43, "SRE", Operation::SRE, AddressingMode::IndexedIndirect, 2, 8);
    set(0x53, "SRE", Operation::SRE, AddressingMode::IndirectIndexed, 2, 8);

    set(0x67, "RRA", Operation::RRA, AddressingMode::ZeroPage, 2, 5);
    set(0x77, "RRA", Operation::RRA, AddressingMode::ZeroPageX, 2, 6);
    set(0x6F, "RRA", Operation::RRA, AddressingMode::Absolute, 3, 6);
    set(0x7F, "RRA", Operation::RRA, AddressingMode::AbsoluteX, 3, 7);
    set(0x7B, "RRA", Operation::RRA, AddressingMode::AbsoluteY, 3, 7);
    set(0x63, "RRA", Operation::RRA, AddressingMode::IndexedIndirect, 2, 8);
    set(0x73, "RRA", Operation::RRA, AddressingMode::IndirectIndexed, 2, 8);

    set(0x87, "SAX", Operation::SAX, AddressingMode::ZeroPage, 2, 3);
    set(0x97, "SAX", Operation::SAX, AddressingMode::ZeroPageY, 2, 4);
    set(0x8F, "SAX", Operation::SAX, AddressingMode::Absolute, 3, 4);
    set(0x83, "SAX", Operation::SAX, AddressingMode::IndexedIndirect, 2, 6);

    set(0xA7, "LAX", Operation::LAX, AddressingMode::ZeroPage, 2, 3);
    set(0xB7, "LAX", Operation::LAX, AddressingMode::ZeroPageY, 2, 4);
    set(0xAF, "LAX", Operation::LAX, AddressingMode::Absolute, 3, 4);
    set(0xBF, "LAX", Operation::LAX, AddressingMode::AbsoluteY, 3, 4);
    set(0xA3, "LAX", Operation::LAX, AddressingMode::IndexedIndirect, 2, 6);
    set(0xB3, "LAX", Operation::LAX, AddressingMode::IndirectIndexed, 2, 5);
    
    set(0xC7, "DCP", Operation::DCP, AddressingMode::ZeroPage, 2, 5);
    set(0xD7, "DCP", Operation::DCP, AddressingMode::ZeroPageX, 2, 6);
    set(0xCF, "DCP", Operation::DCP, AddressingMode::Absolute, 3, 6);
    set(0xDF, "DCP", Operation::DCP, AddressingMode::AbsoluteX, 3, 7);
    set(0xDB, "DCP", Operation::DCP, AddressingMode::AbsoluteY, 3, 7);
    set(0xC3, "DCP", Operation::DCP, AddressingMode::IndexedIndirect, 2, 8);
    set(0xD3, "DCP", Operation::DCP, AddressingMode::IndirectIndexed, 2, 8);

    set(0xE7, "ISC", Operation::ISC, AddressingMode::ZeroPage, 2, 5);
    set(0xF7, "ISC", Operation::ISC, AddressingMode::ZeroPageX, 2, 6);
    set(0xEF, "ISC", Operation::ISC, AddressingMode::Absolute, 3, 6);
    set(0xFF, "ISC", Operation::ISC, AddressingMode::AbsoluteX, 3, 7);
    set(0xFB, "ISC", Operation::ISC, AddressingMode::AbsoluteY, 3, 7);
    set(0xE3, "ISC", Operation::ISC, AddressingMode::IndexedIndirect, 2, 8);
    set(0xF3, "ISC", Operation::ISC, AddressingMode::IndirectIndexed, 2, 8);

    set(0x9F, "SHA", Operation::SHA, AddressingMode::Absolute, 3, 5);
    set(0x93, "SHA", Operation::SHA, AddressingMode::IndirectIndexed, 2, 6);

    set(0x9E, "SHX", Operation::SHX, AddressingMode::AbsoluteY, 3, 5);

    set(0x9C, "SHY", Operation::SHY, AddressingMode::AbsoluteX, 3, 5);

    set(0xBB, "LAE", Operation::LAE, AddressingMode::AbsoluteX, 3, 5);

    set(0x0b, "ANC", Operation::ANC, AddressingMode::Immediate, 2, 2);
    set(0x2B, "ANC", Operation::ANC, AddressingMode::Immediate, 2, 2);
    set(0x4B, "ASR", Operation::ASR, AddressingMode::Immediate, 2, 2);
    set(0x6B, "ARR", Operation::ARR, AddressingMode::Immediate, 2, 2);
    set(0x8B, "ANE", Operation::ANE, AddressingMode::Immediate, 2, 2);
    set(0xAB, "LXA", Operation::LXA, AddressingMode::Immediate, 2, 2);
    set(0xCB, "AXS", Operation::AXS, AddressingMode::Immediate, 2, 2);
    set(0xEB, "SBC", Operation::SBC, AddressingMode::Immediate, 2, 2);


    return table;
}();
