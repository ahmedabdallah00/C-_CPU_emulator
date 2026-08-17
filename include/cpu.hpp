#pragma once

#include <expected>
#include <iostream>

#include "types.hpp"
#include "cpu_state.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include "trap.hpp"

namespace rv32 {

    // Main CPU class implementing the RV32I state machine
    // Uses function pointers for zero-overhead dispatch
    class Cpu
    {
    public:
        explicit Cpu(Memory& memory) noexcept;

        // Architectural state access
        [[nodiscard]]
        CpuArchState& arch_state() noexcept { return arch_state_; }

        [[nodiscard]]
        const CpuArchState& arch_state() const noexcept { return arch_state_; }

        // Execution state access
        [[nodiscard]]
        ExecutionState& exec_state() noexcept { return exec_state_; }

        [[nodiscard]]
        const ExecutionState& exec_state() const noexcept { return exec_state_; }

        // Execute one complete instruction (fetch -> decode -> execute -> writeback)
        // Returns error if a trap occurred
        [[nodiscard]]
        std::expected<void, Trap> step() noexcept;

        // Enable instruction tracing for debugging
        void enable_tracing(bool enable = true) noexcept
        {
            exec_state_.enable_tracing(enable);
        }

        // Reset CPU to initial state
        void reset() noexcept;

    private:
        // Reference to shared memory
        Memory& memory_;

        // Architectural state (registers, PC)
        CpuArchState arch_state_;

        // Execution state (stage, fetched/decoded instructions, trap info)
        ExecutionState exec_state_;

        // ================================================================
        // Stage handlers - function pointers for efficient dispatch
        // ================================================================
        // Each stage handler decides which stage executes next
        // Normal flow: Fetch -> Decode -> Execute -> Writeback -> Fetch

        // Fetch: Load instruction from memory[PC]
        void fetch_stage() noexcept;

        // Decode: Extract fields from fetched instruction
        void decode_stage() noexcept;

        // Execute: Perform instruction semantics (ALU, memory ops)
        void execute_stage() noexcept;

        // Memory: Complete load/store (future pipelined design)
        void memory_stage() noexcept { exec_state_.set_stage(CpuStage::Writeback); }

        // Writeback: Write results to architectural registers
        void writeback_stage() noexcept;

        // Halt: CPU has halted
        void halt_stage() noexcept { /* Already halted */ }

        // Trap: Exception occurred
        void trap_stage() noexcept { /* Trap state - no further execution */ }

        // ================================================================
        // Instruction execution handlers - dispatched by opcode
        // ================================================================
        
        // Immediate-ALU instructions (opcode 0x13)
        [[nodiscard]] std::expected<void, Trap> exec_addi() noexcept;
        
        // Register-ALU instructions (opcode 0x33)
        [[nodiscard]] std::expected<void, Trap> exec_add() noexcept;
        [[nodiscard]] std::expected<void, Trap> exec_sub() noexcept;
        [[nodiscard]] std::expected<void, Trap> exec_xor() noexcept;
        [[nodiscard]] std::expected<void, Trap> exec_or() noexcept;
        [[nodiscard]] std::expected<void, Trap> exec_and() noexcept;

        // Helper for instruction tracing
        void trace_instruction(std::string_view mnemonic) noexcept;
    };

} // namespace rv32
