#pragma once

#include <cstddef>
#include <cstdint>

namespace rv32 {

    // Fixed-width architectural types
    using Byte     = std::uint8_t;
    using HalfWord = std::uint16_t;
    using Word     = std::uint32_t;
    using i32      = std::int32_t;
    
    using RegisterIndex = std::uint8_t;

    // Architectural constants
    inline constexpr std::size_t REG_COUNT = 32;
    inline constexpr Word PC_START = 0x0000'0000;

} // namespace rv32
