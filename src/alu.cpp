#include "alu.hpp"

#include <limits>

std::uint32_t alu_execute(AluOp operation, std::uint32_t lhs, std::uint32_t rhs) noexcept {
    switch (operation) {
    case AluOp::ADD: return lhs + rhs;
    case AluOp::SUB: return lhs - rhs;
    case AluOp::SLL: return lhs << (rhs & 0x1FU);
    case AluOp::SLT: return static_cast<std::int32_t>(lhs) < static_cast<std::int32_t>(rhs);
    case AluOp::SLTU: return lhs < rhs;
    case AluOp::XOR: return lhs ^ rhs;
    case AluOp::SRL: return lhs >> (rhs & 0x1FU);
    case AluOp::SRA: return static_cast<std::uint32_t>(static_cast<std::int32_t>(lhs) >> (rhs & 0x1FU));
    case AluOp::OR: return lhs | rhs;
    case AluOp::AND: return lhs & rhs;
    }
    return 0;
}
