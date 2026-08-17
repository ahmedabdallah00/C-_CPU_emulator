#pragma once

#include <cstddef>
#include <expected>
#include <vector>

#include "types.hpp"
#include "trap.hpp"

namespace rv32 {

    // Memory abstraction with alignment checking and access-fault handling
    class Memory
    {
    public:
        explicit Memory(std::size_t size) noexcept;
        
        // 8-bit access
        [[nodiscard]]
        std::expected<Byte, Trap> read8(Word addr) const noexcept;

        [[nodiscard]]
        std::expected<void, Trap> write8(Word addr, Byte data) noexcept;

        // 16-bit access (requires 2-byte alignment)
        [[nodiscard]]
        std::expected<HalfWord, Trap> read16(Word addr) const noexcept;

        [[nodiscard]]
        std::expected<void, Trap> write16(Word addr, HalfWord data) noexcept;

        // 32-bit access (requires 4-byte alignment)
        [[nodiscard]]
        std::expected<Word, Trap> read32(Word addr) const noexcept;

        [[nodiscard]]
        std::expected<void, Trap> write32(Word addr, Word data) noexcept;

        [[nodiscard]]
        constexpr std::size_t size() const noexcept { return data_.size(); }

    private:
        std::vector<Byte> data_;
    };

} // namespace rv32
