#include "cpu.hpp"

namespace rv32
{
    Cpu::Cpu(Memory& memory)
        : memory_(memory)
    {
    }


    std::expected<Word, Trap> Cpu::fetch() const noexcept
    {
        return memory_.read32(state_.pc());
    }


    std::expected<void, Trap>
    Cpu::execute(const DecodedInstruction& instruction) noexcept
    {
        constexpr Word OP_IMM = 0x13;
        constexpr Word OP_REG = 0x33;

        switch (instruction.opcode)
        {
            case OP_IMM:
            {
                // ADDI
                if (instruction.funct3 == 0x0)
                {
                    const Word lhs =
                        state_.reg(instruction.rs1);

                    const Word result =
                        lhs + instruction.immediate;

                    state_.set_reg(
                        instruction.rd,
                        result
                    );

                    state_.set_pc(
                        state_.pc() + 4
                    );

                    return {};
                }

                break;
            }


            case OP_REG:
            {
                const Word lhs =
                    state_.reg(instruction.rs1);

                const Word rhs =
                    state_.reg(instruction.rs2);

                Word result = 0;

                switch (instruction.funct3)
                {
                    // ADD / SUB
                    case 0x0:
                    {
                        if (instruction.funct7 == 0x00)
                        {
                            // ADD
                            result = lhs + rhs;
                        }
                        else if (instruction.funct7 == 0x20)
                        {
                            // SUB
                            result = lhs - rhs;
                        }
                        else
                        {
                            return std::unexpected(
                                Trap::LoadAccessFault
                            );
                        }

                        break;
                    }


                    // XOR
                    case 0x4:
                    {
                        if (instruction.funct7 != 0x00)
                        {
                            return std::unexpected(
                                Trap::LoadAccessFault
                            );
                        }

                        result = lhs ^ rhs;
                        break;
                    }


                    // OR
                    case 0x6:
                    {
                        if (instruction.funct7 != 0x00)
                        {
                            return std::unexpected(
                                Trap::LoadAccessFault
                            );
                        }

                        result = lhs | rhs;
                        break;
                    }


                    // AND
                    case 0x7:
                    {
                        if (instruction.funct7 != 0x00)
                        {
                            return std::unexpected(
                                Trap::LoadAccessFault
                            );
                        }

                        result = lhs & rhs;
                        break;
                    }


                    default:
                        return std::unexpected(
                            Trap::LoadAccessFault
                        );
                }

                state_.set_reg(
                    instruction.rd,
                    result
                );

                state_.set_pc(
                    state_.pc() + 4
                );

                return {};
            }


            default:
                return std::unexpected(
                    Trap::LoadAccessFault
                );
        }

        return std::unexpected(
            Trap::LoadAccessFault
        );
    }
}