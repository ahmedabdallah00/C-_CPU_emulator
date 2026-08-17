#include "instruction.hpp"

namespace rv32 {

    DecodedInstruction decode(Word instruction) noexcept
    {
        DecodedInstruction result{};

        result.raw    = instruction;
        result.opcode = instruction & 0x7F;
        result.rd     = (instruction >> 7) & 0x1F;
        result.funct3 = (instruction >> 12) & 0x07;
        result.rs1    = (instruction >> 15) & 0x1F;
        result.rs2    = (instruction >> 20) & 0x1F;
        result.funct7 = (instruction >> 25) & 0x7F;

        // For I-type instructions: immediate is bits [31:20]
        // For S-type instructions: immediate is bits [31:25] concatenated with [11:7]
        // For B-type instructions: immediate is bits [31], [7], [30:25], [11:8]
        // For U-type instructions: immediate is bits [31:12]
        // For J-type instructions: immediate is bits [31:20], [10:1], [11], [19:12]
        //
        // We extract I-type immediate by default (most common)
        const Word immediate = instruction >> 20;
        result.immediate = sign_extend(immediate, 12);

        return result;
    }

    std::string_view instruction_name(const DecodedInstruction& instr) noexcept
    {
        constexpr Word OP_IMM = 0x13;
        constexpr Word OP_REG = 0x33;

        switch (instr.opcode)
        {
            case OP_IMM:
                switch (instr.funct3)
                {
                    case 0x0: return "ADDI";
                    case 0x1: return "SLLI";
                    case 0x2: return "SLTI";
                    case 0x3: return "SLTIU";
                    case 0x4: return "XORI";
                    case 0x5: return "SRLI/SRAI";
                    case 0x6: return "ORI";
                    case 0x7: return "ANDI";
                    default:  return "UNKNOWN";
                }

            case OP_REG:
                switch (instr.funct3)
                {
                    case 0x0:
                        return (instr.funct7 == 0x00) ? "ADD" : 
                               (instr.funct7 == 0x20) ? "SUB" : "UNKNOWN";
                    case 0x1: return "SLL";
                    case 0x2: return "SLT";
                    case 0x3: return "SLTU";
                    case 0x4: return "XOR";
                    case 0x5:
                        return (instr.funct7 == 0x00) ? "SRL" : 
                               (instr.funct7 == 0x20) ? "SRA" : "UNKNOWN";
                    case 0x6: return "OR";
                    case 0x7: return "AND";
                    default:  return "UNKNOWN";
                }

            default:
                return "UNKNOWN";
        }
    }

} // namespace rv32
