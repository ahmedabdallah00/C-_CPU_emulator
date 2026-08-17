#include <format>
#include <iostream>

#include "cpu.hpp"

using namespace rv32;

int main(){

    CpuState cpu;
    std::cout << std::format("PC  = 0x{:08X}\n", cpu.pc());
    std::cout << std::format("x0  = {}\n", cpu.reg(0));
    std::cout << std::format("x5  = {}\n", cpu.reg(5));

    // Attempt to violate the x0 = 0 invariant
    cpu.set_reg(0, 999);
    cpu.set_reg(5, 42);

    std::cout << std::format("x0 after write attempt = {}\n", cpu.reg(0));
    std::cout << std::format("x5 after write attempt = {}\n", cpu.reg(5));

    return 0;
}