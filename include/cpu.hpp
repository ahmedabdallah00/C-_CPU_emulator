#pragma once

#include <array>
#include <cstddef>
#include <cassert> 
#include <expected>

#include "types.hpp"
#include "memory.hpp"
#include "instruction.hpp"

namespace rv32{
    class CpuState{
    public:
        constexpr CpuState() noexcept = default;
        
        [[nodiscard]]
        constexpr Word pc() const noexcept{
            return pc_;
        }
        [[nodiscard]]
        constexpr Word reg(std::size_t index) const {
            assert(index < REG_COUNT);
            return regs_[index];
        }
        
        constexpr void set_pc(Word value) noexcept{
            pc_ = value;
        }
        
        constexpr void set_reg(std::size_t index, Word value) noexcept{
            if(index ==0){
                return ;
            }
            regs_[index] = value;
        }
    private:
    // Default member initializer: PC starts at PC_START.
        Word pc_{PC_START};

    // std::array is a zero-overhead wrapper around a C array.
    // The {} value-initializes every element to zero.
        std::array<Word, REG_COUNT> regs_{};
    };
    class Cpu{
    public:
        explicit Cpu(Memory& memory);
        
        [[nodiscard]]
        std::expected<Word, Trap> fetch() const noexcept;

        [[nodiscard]]
        std::expected<void, Trap> execute(const DecodedInstruction& instruction) noexcept;

        [[nodiscard]]
        CpuState& state() noexcept
        {
            return state_;
        }

        [[nodiscard]]
        const CpuState& state() const noexcept
        {
            return state_;
        }
    private:
        CpuState state_;
        Memory&  memory_;
    };
}

