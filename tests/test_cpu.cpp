#include <cassert>

#include "cpu.hpp"
#include "instruction.hpp"
#include "memory.hpp"

int main()
{
    rv32::Memory memory{64};
    rv32::Cpu cpu{memory};


    // ==================================================
    // CPU state
    // ==================================================

    assert(cpu.state().pc() == rv32::PC_START);

    for (std::size_t i = 0; i < rv32::REG_COUNT; ++i)
    {
        assert(cpu.state().reg(i) == 0);
    }


    // ==================================================
    // ADDI x5, x0, 10
    //
    // Expected:
    //
    // x5 = 10
    // PC = PC + 4
    // ==================================================

    constexpr rv32::Word instruction = 0x00A00293;

    auto write_result =
        memory.write32(0, instruction);

    assert(write_result.has_value());


    // --------------------------------------------------
    // Fetch
    // --------------------------------------------------

    auto fetched = cpu.fetch();

    assert(fetched.has_value());
    assert(*fetched == instruction);


    // --------------------------------------------------
    // Decode
    // --------------------------------------------------

    auto decoded =
        rv32::decode(*fetched);

    assert(decoded.raw == instruction);

    assert(decoded.opcode == 0x13);
    assert(decoded.rd == 5);
    assert(decoded.rs1 == 0);
    assert(decoded.funct3 == 0);
    assert(decoded.immediate == 10);


    // --------------------------------------------------
    // Execute
    // --------------------------------------------------

    auto execute_result =
        cpu.execute(decoded);

    assert(execute_result.has_value());


    // --------------------------------------------------
    // Verify result
    // --------------------------------------------------

    assert(cpu.state().reg(5) == 10);

    assert(
        cpu.state().pc()
        == rv32::PC_START + 4
    );


    // ==================================================
    // x0 must remain zero
    // ==================================================

    assert(cpu.state().reg(0) == 0);


    return 0;
}