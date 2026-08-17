#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>

#include "types.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include "trap.hpp"

namespace rv32 {

    // Explicit CPU execution stage - the CPU state machine operates through these stages
    enum class CpuStage : std::uint8_t
    {
        Fetch,      // Load instruction from memory at PC
        Decode,     // Extract fields from raw instruction
        Execute,    // Perform operation, access memory if needed
        Memory,     // Complete memory operations (for future pipeline)
        Writeback,  // Write results to registers
        Halt,       // CPU halted (program finished or breakpoint)
        Trap,       // Trap/exception occurred
    };

    // Convert stage to human-readable name
    constexpr std::string_view stage_name(CpuStage stage) noexcept
    {
        switch (stage)
        {
            case CpuStage::Fetch:    return "Fetch";
            case CpuStage::Decode:   return "Decode";
            case CpuStage::Execute:  return "Execute";
            case CpuStage::Memory:   return "Memory";
            case CpuStage::Writeback: return "Writeback";
            case CpuStage::Halt:     return "Halt";
            case CpuStage::Trap:     return "Trap";
        }
        return "UnknownStage";
    }

    // Architectural CPU state (registers, PC)
    // Separate from execution machinery
    class CpuArchState
    {
    public:
        constexpr CpuArchState() noexcept = default;
        
        [[nodiscard]]
        constexpr Word pc() const noexcept
        {
            return pc_;
        }
        
        [[nodiscard]]
        constexpr Word reg(std::size_t index) const noexcept
        {
            if (index >= REG_COUNT) {
                return 0;
            }
            return regs_[index];
        }
        
        constexpr void set_pc(Word value) noexcept
        {
            pc_ = value;
        }
        
        constexpr void set_reg(std::size_t index, Word value) noexcept
        {
            if (index == 0) {
                return; // x0 is hardwired to zero
            }
            if (index < REG_COUNT) {
                regs_[index] = value;
            }
        }
        
        // Reset to initial state
        constexpr void reset() noexcept
        {
            pc_ = PC_START;
            regs_.fill(0);
        }

    private:
        Word pc_{PC_START};
        std::array<Word, REG_COUNT> regs_{};
    };

    // Execution state (pipeline, trap info, tracing)
    class ExecutionState
    {
    public:
        constexpr ExecutionState() noexcept = default;
        
        [[nodiscard]]
        constexpr CpuStage stage() const noexcept { return stage_; }
        
        constexpr void set_stage(CpuStage new_stage) noexcept { stage_ = new_stage; }
        
        [[nodiscard]]
        constexpr Word fetched_instruction() const noexcept { return fetched_instr_; }
        
        constexpr void set_fetched_instruction(Word instr) noexcept { fetched_instr_ = instr; }
        
        [[nodiscard]]
        constexpr const DecodedInstruction& decoded_instruction() const noexcept 
        { 
            return decoded_instr_; 
        }
        
        constexpr void set_decoded_instruction(const DecodedInstruction& instr) noexcept 
        { 
            decoded_instr_ = instr; 
        }
        
        [[nodiscard]]
        constexpr Trap trap_code() const noexcept { return trap_code_; }
        
        constexpr void set_trap(Trap trap) noexcept { trap_code_ = trap; }
        
        constexpr void reset() noexcept
        {
            stage_ = CpuStage::Fetch;
            fetched_instr_ = 0;
            decoded_instr_ = {};
            trap_code_ = Trap::IllegalInstruction; // placeholder
        }
        
        // Tracing support
        [[nodiscard]]
        constexpr bool tracing_enabled() const noexcept { return tracing_; }
        
        constexpr void enable_tracing(bool enable = true) noexcept { tracing_ = enable; }

    private:
        CpuStage stage_{CpuStage::Fetch};
        Word fetched_instr_{0};
        DecodedInstruction decoded_instr_{};
        Trap trap_code_{Trap::IllegalInstruction};
        bool tracing_{false};
    };

} // namespace rv32
