#include "instruction.hpp"

namespace rv32{
    Word sign_extend(Word value, unsigned bits) noexcept
    {
        const Word sign_bit = Word{1} << (bits - 1);
        if((value & sign_bit) == 0)
        {
            return value;
        }
        // Create 1s above the original 'bits'.
        const Word mask = ~Word{0} << bits;
         // Keep the original value and fill upper bits with 1s.
        return value | mask;
    }
    DecodedInstruction decode(Word instruction) noexcept
    {
        DecodedInstruction result{};

        result.raw = instruction;
        result.opcode = instruction & 0x7F;
        result.rd = (instruction >> 7) & 0x1F;
        result.funct3 = (instruction >> 12) & 0x07;
        result.rs1 = (instruction >> 15) & 0x1F;
        result.funct7 = (instruction >> 25) & 0x7F;
        const Word immediate = instruction >> 20;

        result.immediate = sign_extend(immediate,12);

        return result;
        
    }
}