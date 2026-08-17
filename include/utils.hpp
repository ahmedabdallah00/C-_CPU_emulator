#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

template <typename T = std::uint32_t>
[[nodiscard]] constexpr T sign_extend(std::uint32_t value, int bits) noexcept {
    const std::uint32_t mask = (std::uint32_t{1} << bits) - 1U;
    const std::uint32_t extended = value & mask;
    const std::uint32_t sign_bit = std::uint32_t{1} << (bits - 1);
    return static_cast<T>((extended & sign_bit) == 0U ? extended : (extended | ~mask));
}

[[nodiscard]] constexpr std::uint32_t extract_bits(std::uint32_t value, int start, int end) noexcept {
    const auto width = end - start + 1;
    const auto mask = (std::uint32_t{1} << width) - 1U;
    return (value >> start) & mask;
}

[[nodiscard]] inline std::string print_hex(std::uint32_t value) {
    std::ostringstream stream;
    stream << "0x" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << value;
    return stream.str();
}
