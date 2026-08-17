#include <iomanip>

#include "cpu.hpp"

namespace rv32 {

    Cpu::Cpu(Memory& memory) noexcept
        : memory_(memory)
    {
    }

    void Cpu::reset() noexcept
    {
        arch_state_.reset();
        exec_state_.reset();
    }

    std::expected<void, Trap> Cpu::step() noexcept
    {
        // Execute stages in sequence
        // Normal flow: Fetch -> Decode -> Execute -> Writeback
        
        // Fetch: Load instruction from memory
        fetch_stage();
        
        // Decode: Extract fields from instruction
        decode_stage();
        
        // Execute: Perform instruction semantics
        execute_stage();
        if (exec_state_.stage() == CpuStage::Trap) {
            return std::unexpected(exec_state_.trap_code());
        }
        
        // Memory: Complete memory operations (passthrough for now)
        memory_stage();
        
        // Writeback: Write results to registers
        writeback_stage();
        
        return {};
    }

    // ================================================================
    // Stage implementations
    // ================================================================

    void Cpu::fetch_stage() noexcept
    {
        const Word pc = arch_state_.pc();
        auto result = memory_.read32(pc);
        
        if (!result.has_value()) {
            exec_state_.set_stage(CpuStage::Trap);
            exec_state_.set_trap(result.error());
            return;
        }
        
        exec_state_.set_fetched_instruction(*result);
        exec_state_.set_stage(CpuStage::Decode);
    }

    void Cpu::decode_stage() noexcept
    {
        const Word raw_instr = exec_state_.fetched_instruction();
        const auto decoded = decode(raw_instr);
        exec_state_.set_decoded_instruction(decoded);
        exec_state_.set_stage(CpuStage::Execute);
    }

    void Cpu::execute_stage() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        constexpr Word OP_IMM = 0x13;
        constexpr Word OP_REG = 0x33;

        std::expected<void, Trap> exec_result{std::unexpected(Trap::IllegalInstruction)};

        switch (instr.opcode)
        {
            case OP_IMM:
                exec_result = exec_addi();
                break;

            case OP_REG:
                switch (instr.funct3)
                {
                    case 0x0:
                        exec_result = (instr.funct7 == 0x00) ? exec_add() : 
                                      (instr.funct7 == 0x20) ? exec_sub() :
                                      std::unexpected(Trap::IllegalInstruction);
                        break;
                    case 0x4:
                        exec_result = exec_xor();
                        break;
                    case 0x6:
                        exec_result = exec_or();
                        break;
                    case 0x7:
                        exec_result = exec_and();
                        break;
                    default:
                        exec_result = std::unexpected(Trap::IllegalInstruction);
                }
                break;

            default:
                exec_result = std::unexpected(Trap::IllegalInstruction);
        }

        if (!exec_result.has_value()) {
            exec_state_.set_stage(CpuStage::Trap);
            exec_state_.set_trap(exec_result.error());
            return;
        }

        exec_state_.set_stage(CpuStage::Memory);
    }

    void Cpu::writeback_stage() noexcept
    {
        // At this point, all execution is complete and results are already
        // written to architectural state by individual instruction handlers
        exec_state_.set_stage(CpuStage::Fetch);
    }

    // ================================================================
    // Instruction execution handlers
    // ================================================================

    std::expected<void, Trap> Cpu::exec_addi() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // ADDI rd, rs1, imm
        if (instr.funct3 != 0x0) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const i32 result = static_cast<i32>(lhs) + static_cast<i32>(instr.immediate);

        arch_state_.set_reg(instr.rd, static_cast<Word>(result));
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("ADDI");
        }

        return {};
    }

    std::expected<void, Trap> Cpu::exec_add() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // ADD rd, rs1, rs2
        if (instr.funct3 != 0x0 || instr.funct7 != 0x00) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const Word rhs = arch_state_.reg(instr.rs2);
        const i32 result = static_cast<i32>(lhs) + static_cast<i32>(rhs);

        arch_state_.set_reg(instr.rd, static_cast<Word>(result));
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("ADD");
        }

        return {};
    }

    std::expected<void, Trap> Cpu::exec_sub() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // SUB rd, rs1, rs2
        if (instr.funct3 != 0x0 || instr.funct7 != 0x20) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const Word rhs = arch_state_.reg(instr.rs2);
        const i32 result = static_cast<i32>(lhs) - static_cast<i32>(rhs);

        arch_state_.set_reg(instr.rd, static_cast<Word>(result));
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("SUB");
        }

        return {};
    }

    std::expected<void, Trap> Cpu::exec_xor() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // XOR rd, rs1, rs2
        if (instr.funct3 != 0x4 || instr.funct7 != 0x00) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const Word rhs = arch_state_.reg(instr.rs2);
        const Word result = lhs ^ rhs;

        arch_state_.set_reg(instr.rd, result);
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("XOR");
        }

        return {};
    }

    std::expected<void, Trap> Cpu::exec_or() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // OR rd, rs1, rs2
        if (instr.funct3 != 0x6 || instr.funct7 != 0x00) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const Word rhs = arch_state_.reg(instr.rs2);
        const Word result = lhs | rhs;

        arch_state_.set_reg(instr.rd, result);
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("OR");
        }

        return {};
    }

    std::expected<void, Trap> Cpu::exec_and() noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        
        // AND rd, rs1, rs2
        if (instr.funct3 != 0x7 || instr.funct7 != 0x00) {
            return std::unexpected(Trap::IllegalInstruction);
        }

        const Word lhs = arch_state_.reg(instr.rs1);
        const Word rhs = arch_state_.reg(instr.rs2);
        const Word result = lhs & rhs;

        arch_state_.set_reg(instr.rd, result);
        arch_state_.set_pc(arch_state_.pc() + 4);

        if (exec_state_.tracing_enabled()) {
            trace_instruction("AND");
        }

        return {};
    }

    void Cpu::trace_instruction(std::string_view mnemonic) noexcept
    {
        const auto& instr = exec_state_.decoded_instruction();
        std::cout << "PC=" << std::hex << std::setfill('0') << std::setw(8) << arch_state_.pc() - 4
                  << " INST=" << std::setw(8) << instr.raw
                  << " " << mnemonic
                  << " x" << std::dec << static_cast<int>(instr.rd)
                  << std::endl;
    }

} // namespace rv32
