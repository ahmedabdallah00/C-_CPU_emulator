#pragma once

#include <cstdint>

enum class InstrFormat { R, I, S, B, U, J, Unknown };

struct DecodedInstr {
    std::uint32_t raw{};
    std::uint32_t opcode{};
    std::uint32_t rd{};
    std::uint32_t funct3{};
    std::uint32_t rs1{};
    std::uint32_t rs2{};
    std::uint32_t funct7{};
    std::int32_t immediate{};
    InstrFormat format{InstrFormat::Unknown};
};

[[nodiscard]] DecodedInstr decode(std::uint32_t instruction) noexcept;
