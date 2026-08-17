#include "decoder.hpp"
#include "utils.hpp"

DecodedInstr decode(std::uint32_t instruction) noexcept {
    DecodedInstr decoded;
    decoded.raw = instruction;
    decoded.opcode = extract_bits(instruction, 0, 6);
    decoded.rd = extract_bits(instruction, 7, 11);
    decoded.funct3 = extract_bits(instruction, 12, 14);
    decoded.rs1 = extract_bits(instruction, 15, 19);
    decoded.rs2 = extract_bits(instruction, 20, 24);
    decoded.funct7 = extract_bits(instruction, 25, 31);

    switch (decoded.opcode) {
    case 0x33: decoded.format = InstrFormat::R; break;
    case 0x03: case 0x13: case 0x67: case 0x73:
        decoded.format = InstrFormat::I;
        decoded.immediate = sign_extend<std::int32_t>(extract_bits(instruction, 20, 31), 12);
        break;
    case 0x23:
        decoded.format = InstrFormat::S;
        decoded.immediate = sign_extend<std::int32_t>((extract_bits(instruction, 25, 31) << 5U) |
                                                       extract_bits(instruction, 7, 11), 12);
        break;
    case 0x63: {
        decoded.format = InstrFormat::B;
        const auto immediate = (extract_bits(instruction, 31, 31) << 12U) |
                               (extract_bits(instruction, 7, 7) << 11U) |
                               (extract_bits(instruction, 25, 30) << 5U) |
                               (extract_bits(instruction, 8, 11) << 1U);
        decoded.immediate = sign_extend<std::int32_t>(immediate, 13);
        break;
    }
    case 0x37: case 0x17:
        decoded.format = InstrFormat::U;
        decoded.immediate = static_cast<std::int32_t>(instruction & 0xFFFFF000U);
        break;
    case 0x6F: {
        decoded.format = InstrFormat::J;
        const auto immediate = (extract_bits(instruction, 31, 31) << 20U) |
                               (extract_bits(instruction, 12, 19) << 12U) |
                               (extract_bits(instruction, 20, 20) << 11U) |
                               (extract_bits(instruction, 21, 30) << 1U);
        decoded.immediate = sign_extend<std::int32_t>(immediate, 21);
        break;
    }
    default: decoded.format = InstrFormat::Unknown; break;
    }
    return decoded;
}
