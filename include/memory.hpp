#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <vector>

#include "types.hpp"

namespace rv32{

    enum class Trap{
        LoadAddressMisaligned,
        LoadAccessFault,
        StoreAddressMisaligned,
        StoreAccessFault,
    };

    class Memory{
        public:
            explicit Memory(std::size_t size);
            
            [[nodiscard]]
            std::expected<Byte, Trap>
            read8(Word addr) const noexcept;

            [[nodiscard]]
            std::expected<HalfWord, Trap>
            read16(Word addr) const noexcept;

            [[nodiscard]]
            std::expected<Word, Trap>
            read32(Word addr) const noexcept;

            [[nodiscard]]
            std::expected<void, Trap>
            write8(Word addr, Byte data) noexcept;

            [[nodiscard]]
            std::expected<void, Trap>
            write16(Word addr, HalfWord data) noexcept;

            [[nodiscard]]
            std::expected<void, Trap>
            write32(Word addr, Word data) noexcept;

        private:
            std::vector<Byte> data_;

    };
}