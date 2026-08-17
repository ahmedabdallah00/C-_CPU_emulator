#pragma once

#include <cstdint>

enum class AluOp { ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND };

[[nodiscard]] std::uint32_t alu_execute(AluOp operation, std::uint32_t lhs,
                                        std::uint32_t rhs) noexcept;
