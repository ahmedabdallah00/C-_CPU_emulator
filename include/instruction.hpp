#pragma once

#include "types.hpp"

namespace rv32{
    struct DecodedInstruction
    {
        Word raw{0};

        Word opcode{0};

        Word rd{0};
        Word funct3{0};

        Word rs1{0};
        Word rs2{0};

        Word funct7{0};

        Word immediate{0};
    };

    [[nodiscard]]
    Word sign_extend(Word value, unsigned bits) noexcept;

    [[nodiscard]]
    DecodedInstruction decode(Word instruction) noexcept;
}