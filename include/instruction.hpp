#pragma once

#include <cstdint>
#include <string_view>

#include "types.hpp"

namespace rv32 {

    // Decoded instruction with all RV32I fields extracted
    struct DecodedInstruction
    {
        Word raw{0};           // Raw 32-bit instruction
        
        Word opcode{0};        // Bits [6:0]
        
        Word rd{0};            // Bits [11:7]   - destination register
        Word rs1{0};           // Bits [19:15]  - source register 1
        Word rs2{0};           // Bits [24:20]  - source register 2
        
        Word funct3{0};        // Bits [14:12]  - 3-bit function code
        Word funct7{0};        // Bits [31:25]  - 7-bit function code (R-type)
        
        Word immediate{0};     // Decoded immediate (sign-extended)
        
        // Helper to identify instruction format
        bool is_immediate() const noexcept { return funct3 == 0 && funct7 == 0 && rs2 == 0; }
    };

    // Sign-extend a value from 'bits' bits
    [[nodiscard]]
    constexpr Word sign_extend(Word value, unsigned bits) noexcept
    {
        const Word sign_bit = Word{1} << (bits - 1);
        if ((value & sign_bit) == 0) {
            return value;
        }
        const Word mask = ~Word{0} << bits;
        return value | mask;
    }

    // Decode raw instruction into DecodedInstruction
    [[nodiscard]]
    DecodedInstruction decode(Word instruction) noexcept;

    // Convert instruction mnemonic to name
    [[nodiscard]]
    std::string_view instruction_name(const DecodedInstruction& instr) noexcept;

} // namespace rv32
