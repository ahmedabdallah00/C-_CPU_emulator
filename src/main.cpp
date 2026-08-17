#include <iostream>
#include "cpu.hpp"
#include "instruction.hpp"

int main()
{
    std::cout << "RV32I CPU Simulator\n";
    std::cout << "===================\n\n";

    // Create 256 bytes of memory
    rv32::Memory memory{256};
    rv32::Cpu cpu{memory};

    // Enable tracing
    cpu.enable_tracing(true);

    // Simple test program:
    // ADDI x1, x0, 10
    // ADDI x2, x0, 20
    // ADD x3, x1, x2
    
    constexpr rv32::Word program[] = {
        0x00A00093,  // ADDI x1, x0, 10
        0x01400113,  // ADDI x2, x0, 20
        0x002081b3,  // ADD x3, x1, x2
    };

    for (std::size_t i = 0; i < 3; ++i) {
        [[maybe_unused]] auto _ = memory.write32(i * 4, program[i]);
    }

    std::cout << "Executing program:\n";
    std::cout << "  ADDI x1, x0, 10  (x1 = 10)\n";
    std::cout << "  ADDI x2, x0, 20  (x2 = 20)\n";
    std::cout << "  ADD x3, x1, x2   (x3 = 30)\n\n";

    // Execute each instruction
    for (int i = 0; i < 3; ++i) {
        auto result = cpu.step();
        if (!result.has_value()) {
            std::cout << "Error: " << static_cast<int>(result.error()) << "\n";
            return 1;
        }
    }

    std::cout << "\nFinal register values:\n";
    std::cout << "  x1 = " << cpu.arch_state().reg(1) << "\n";
    std::cout << "  x2 = " << cpu.arch_state().reg(2) << "\n";
    std::cout << "  x3 = " << cpu.arch_state().reg(3) << "\n";
    std::cout << "  PC = " << std::hex << cpu.arch_state().pc() << "\n";

    return 0;
}
