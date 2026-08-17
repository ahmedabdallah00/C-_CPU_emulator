# RV32I CPU Simulator

A clean, maintainable C++23 implementation of a RISC-V RV32I CPU emulator with explicit state machine, strong typing, and comprehensive test coverage.

## Project Overview

This project evolves a RISC-V RV32I emulator into a well-architected CPU simulator that prioritizes:

- **Explicit execution semantics**: Clear fetch-decode-execute-writeback pipeline with type-safe state machine
- **Separation of concerns**: Distinct layers for instruction decoding, execution, memory management, and CPU control flow
- **Error handling without exceptions**: Uses `std::expected<T, Trap>` for deterministic architectural failures
- **Strong typing**: Fixed-width architectural types (`Byte`, `HalfWord`, `Word`) instead of platform-dependent types
- **Maintainability**: Extensible dispatch architecture for adding new instructions without monolithic switch statements
- **Comprehensive testing**: 31 unit and integration tests verifying correctness

## Current Status

**Implemented Instructions:**
- Immediate ALU: `ADDI`
- Register ALU: `ADD`, `SUB`, `AND`, `OR`, `XOR`
- Architecture: x0 hardwiring, proper sign extension, PC progression

**Test Coverage:**
- ✅ 12 CPU execution tests (initial state, register operations, sign extension, traps, integration)
- ✅ 19 memory access tests (alignment, bounds, byte ordering)
- ✅ All 31 tests passing

**Build Status:**
- Builds cleanly with C++23
- No errors, no warnings (except suppressed `[[maybe_unused]]`)
- Three targets: `simulator` (main), `test_cpu`, `test_memory`

## Architecture

### Layered Design

```
┌────────────────────────────────────────────┐
│          CPU Execution Loop                │  (Cpu::step())
├────────────────────────────────────────────┤
│   State Machine (Fetch/Decode/Execute)     │  (CpuStage enum, stage handlers)
├────────────────────────────────────────────┤
│  Instruction Dispatch (opcode → handler)   │  (Per-instruction exec_* functions)
├────────────────────────────────────────────┤
│      Instruction Decoding                  │  (decode() function)
├────────────────────────────────────────────┤
│  CPU State (Registers, PC, Stage Info)     │  (CpuArchState, ExecutionState)
├────────────────────────────────────────────┤
│      Memory Abstraction                    │  (read8/16/32, write8/16/32)
└────────────────────────────────────────────┘
```

### Fetch-Decode-Execute Pipeline

```
     ┌─────────────┐
     │Memory (256B)│
     └──────┬──────┘
            │
          Fetch
            │
            ▼
     ┌─────────────┐
     │Decode Instr │
     └──────┬──────┘
            │
          Decode
            │
            ▼
     ┌─────────────┐
     │  Execute    │
     └──────┬──────┘
            │
         Memory
            │
            ▼
     ┌─────────────┐
     │  Writeback  │
     └──────┬──────┘
            │
        [loop back to Fetch]
```

### CPU Execution State Machine

```
FETCH
  │ [load instruction from PC]
  ▼
DECODE
  │ [extract instruction fields]
  ▼
EXECUTE
  │ [dispatch to instruction handler]
  │ [compute result]
  ▼
MEMORY
  │ [perform memory access if needed]
  │ [(currently a passthrough stage)]
  ▼
WRITEBACK
  │ [write register or advance PC]
  │ [move to next state]
  ▼
[loop back to FETCH]

Trap / Halt (terminal states)
```

**Design Justification:** The pipeline separates concerns and makes each stage's responsibility explicit. The MEMORY stage is currently a passthrough but allows future extensions (e.g., caching, access logging). Each stage is implemented as a member function (`fetch_stage()`, `decode_stage()`, etc.), making the state machine logic clear and testable.

## Code Organization

```
include/
  types.hpp           # Fixed-width types (Byte, HalfWord, Word, i32, RegisterIndex)
  trap.hpp            # Trap enum (LoadAccessFault, StoreAccessFault, etc.)
  memory.hpp          # Memory interface (read*/write* with alignment checks)
  instruction.hpp     # DecodedInstruction struct and decode() function
  cpu_state.hpp       # CpuArchState (registers), ExecutionState (pipeline state), CpuStage enum
  cpu.hpp             # Cpu class: main interface, step(), tracing control

src/
  memory.cpp          # Memory implementation (little-endian, alignment checking)
  instruction.cpp     # Instruction decoder, instruction_name() for mnemonics
  cpu.cpp             # CPU state machine (all stage handlers and instruction handlers)
  main.cpp            # Example usage: 3-instruction program with tracing

tests/
  test_memory.cpp     # 19 memory access tests
  test_cpu.cpp        # 12 CPU execution tests

build/
  test_cpu            # CPU test executable
  test_memory         # Memory test executable
  simulator           # Main simulator executable

CMakeLists.txt        # Build configuration
.gitignore            # Version control ignores
```

## Instruction Dispatch Strategy

Instead of a monolithic `execute()` function with nested switches, the simulator uses a per-opcode dispatch to instruction handlers:

**Current Approach (Dispatch Pseudocode):**
```
step():
  fetch_stage():        Load instruction from memory at PC
  decode_stage():       Extract opcode, rd, rs1, rs2, immediate
  execute_stage():
    switch(opcode):
      case 0x13:        # I-type (ADDI, etc.)
        switch(funct3):
          case 0:       exec_addi()
      case 0x33:        # R-type (ADD, etc.)
        switch(funct3):
          case 0:
            switch(funct7):
              case 0:   exec_add()
              case 32:  exec_sub()
          case 4:       exec_xor()
          case 6:       exec_or()
          case 7:       exec_and()
  writeback_stage():    Write result to register, advance PC

Instruction handlers (exec_addi, exec_add, etc.):
  Perform ALU operation
  Write result to rd (or skip if rd == 0)
  Return (state machine moves to next stage)
```

**Why This Design?**
- **Clarity**: Each instruction's logic is isolated in its own function (`exec_add`, `exec_sub`, etc.)
- **Scalability**: Adding an instruction requires implementing a new `exec_*()` function and updating the dispatch logic
- **No virtual functions**: Uses function calls, not dynamic dispatch (zero overhead)
- **Testing**: Individual instructions can be tested independently
- **Future optimization**: Can transition to a `constexpr std::array` lookup table when instruction count justifies it

**Next Step:** When 15+ instructions are implemented, consider refactoring to:
```cpp
using InstructionHandler = std::expected<void, Trap> (*)(Cpu&, const DecodedInstruction&);
constexpr std::array<InstructionHandler, 128> dispatch_table = { /* ... */ };
// In execute_stage(): dispatch_table[opcode](this, decoded_instr);
```

## Memory Model

### Architecture
- 256 bytes of byte-addressable memory
- Little-endian byte ordering
- Alignment checking for multi-byte accesses

### Operations
```cpp
std::expected<Byte, Trap> read8(Addr addr)        // Byte load
std::expected<HalfWord, Trap> read16(Addr addr)   // 16-bit load (aligned)
std::expected<Word, Trap> read32(Addr addr)       // 32-bit load (aligned)

std::expected<void, Trap> write8(Addr addr, Byte val)      // Byte store
std::expected<void, Trap> write16(Addr addr, HalfWord val) // 16-bit store (aligned)
std::expected<void, Trap> write32(Addr addr, Word val)     // 32-bit store (aligned)
```

### Error Handling
- `LoadAddressMisaligned`: Non-word-aligned address for 32-bit load
- `StoreAddressMisaligned`: Non-word-aligned address for 32-bit store
- `LoadAccessFault`: Address out of bounds for any load
- `StoreAccessFault`: Address out of bounds for any store

**Example:**
```cpp
auto result = memory.read32(addr);
if (!result) {
    trap_code = result.error();  // Get the trap
    // Handle trap...
}
```

## CPU State

### Architectural State (CpuArchState)
Represents RISC-V architectural state:
- **PC** (Program Counter): Current instruction address
- **Registers**: x0-x31 (32 general-purpose registers)
  - x0 is hardwired to 0 (enforced in `set_reg()`)
  - x1-x31 are general-purpose

### Execution State (ExecutionState)
Represents pipeline state (not architectural):
- **Current stage** (CpuStage enum)
- **Fetched instruction** (32-bit word)
- **Decoded instruction** (DecodedInstruction struct)
- **Pending result** (value to write to register)
- **Trap info** (if a trap occurred)
- **Tracing flag** (enable/disable debug output)

**Design Justification:** Separating architectural state from execution state allows:
1. Clean state snapshots (for debugging/saving)
2. Clear pipeline semantics (fetched/decoded/result buffers are temporary)
3. Easy CPU reset (just reset arch_state, leave exec_state for diagnostics)

### x0 Hardwiring

The simulator enforces the RISC-V rule that x0 is always zero:
```cpp
void CpuArchState::set_reg(RegisterIndex idx, i32 value) noexcept
{
    if (idx == 0) return;  // x0 cannot be written
    registers_[idx] = value;
}
```

This check is performed **at register write time** (in `writeback_stage()`), ensuring all instructions (including those explicitly trying to write x0) respect the constraint.

**Test:** Verifies that `ADDI x0, x0, 100` results in x0 remaining 0.

## Instruction Encoding Reference

All RISC-V RV32I instructions follow one of 6 formats:

### R-Type (Register-Register ALU)
Bits:   `[31:25] funct7 | [24:20] rs2 | [19:15] rs1 | [14:12] funct3 | [11:7] rd | [6:0] opcode`

Example: `ADD x3, x1, x2 = 0x002081B3`
- rs1=1, rs2=2, rd=3, funct3=0, funct7=0, opcode=0x33

### I-Type (Immediate ALU, Load)
Bits:   `[31:20] immediate | [19:15] rs1 | [14:12] funct3 | [11:7] rd | [6:0] opcode`

Example: `ADDI x5, x0, 10 = 0x00A00293`
- immediate=10, rs1=0, rd=5, funct3=0, opcode=0x13

Sign extension: If bit 31 is set, all higher bits are 1s (two's complement).

### S-Type (Store)
Bits:   `[31:25] funct7 | [24:20] rs2 | [19:15] rs1 | [14:12] funct3 | [11:7] imm[4:0] | [6:0] opcode`

Immediate (signed): `imm = [31:25] funct7 || [11:7] rd` (12-bit signed)

### B-Type (Branch)
Bits:   `[31] imm[12] | [30:25] imm[10:5] | [24:20] rs2 | [19:15] rs1 | [14:12] funct3 | [11:8] imm[4:1] | [7] imm[11] | [6:0] opcode`

Immediate (signed): 12-bit offset (×2), so branches can skip ±4KB

### U-Type (Upper Immediate)
Bits:   `[31:12] immediate | [11:7] rd | [6:0] opcode`

Immediate: 20-bit value, left-shifted 12 bits

### J-Type (Jump)
Bits:   `[31] imm[20] | [30:21] imm[10:1] | [20] imm[11] | [19:12] imm[19:12] | [11:7] rd | [6:0] opcode`

Immediate (signed): 20-bit offset (×2), so jumps can reach ±1MB

## Trap Handling

The simulator uses `std::expected<T, Trap>` instead of exceptions for deterministic error handling.

### Trap Types
```cpp
enum class Trap : std::uint8_t
{
    LoadAddressMisaligned   = 0,
    LoadAccessFault         = 1,
    StoreAddressMisaligned  = 2,
    StoreAccessFault        = 3,
    IllegalInstruction      = 4,
};
```

### Design Justification
- **No exceptions**: Traps are architectural events, not errors. Using exceptions would be a category error.
- **Explicit**: Every operation that can fail returns `std::expected<T, Trap>`, making error paths visible in code.
- **noexcept-compatible**: All operations are marked `noexcept`, enabling compiler optimizations and making performance guarantees explicit.
- **Deterministic**: No hidden unwinding, easy to trace with debugger.

### Example Usage
```cpp
auto result = cpu.step();
if (!result) {
    Trap t = result.error();
    std::cout << "Trap: " << trap_name(t) << "\n";
    return 1;
}
```

## Instruction Tracing

The simulator includes an optional instruction tracing mechanism for debugging:

```cpp
cpu.enable_tracing(true);
```

**Trace Output Example:**
```
PC=00000000 INST=00A00293 ADDI x5, x0, 10
  x5: 0 → 10

PC=00000004 INST=01400113 ADDI x2, x0, 20
  x2: 0 → 20

PC=00000008 INST=002081b3 ADD x3, x1, x2
  x3: 0 → 30
```

Shows:
- Instruction address (PC)
- Instruction encoding (hex)
- Mnemonic (e.g., "ADDI")
- Operands (e.g., "x5, x0, 10")
- Register changes (source value → new value)

**Design:** Tracing output is generated in `trace_instruction()` after writeback, capturing the actual register changes. No dynamic allocation, minimal overhead.

## C++ Design Decisions

| Feature | Justification | Example |
|---------|---------------|---------|
| **Fixed-width types** | RISC-V is 32-bit; `int` varies by platform. Use `std::uint32_t` for PC/registers. | `using Word = std::uint32_t;` |
| **`std::expected<T, Trap>`** | Architectural failures aren't exceptions. Explicit error handling makes code paths clear. | `auto result = memory.read32(addr);` |
| **`noexcept`** | All CPU operations are deterministic; enables compiler optimizations. | `void Cpu::step() noexcept { ... }` |
| **`[[nodiscard]]`** | Results of operations like `step()` must be checked. | `[[nodiscard]] std::expected<void, Trap> Cpu::step() noexcept;` |
| **`constexpr`** | Instruction opcodes and traps are constants. Enables compile-time checking. | `enum class Trap : std::uint8_t { ... };` |
| **`std::array<i32, 32>`** | Fixed 32 registers. Safer than raw arrays, no dynamic allocation. | `std::array<i32, 32> registers_;` |
| **`std::string_view`** | Instruction mnemonics are static strings. No allocation. | `std::string_view instruction_name(const DecodedInstruction& instr);` |
| **`std::span`** | Memory is fixed-size; use `std::span` for safe bounds. | (Reserved for future memory abstraction) |
| **Member functions** | CPU stages are intimately coupled to state; member functions are appropriate. | `void fetch_stage() noexcept;` |
| **Avoid:** Inheritance | CPU behavior is monolithic; inheritance would overcomplicate. | (No base classes) |
| **Avoid:** Virtual functions | Instruction dispatch is fixed-size; function pointers suffice. | `(this->*exec_handler)();` vs `virtual execute()` |
| **Avoid:** Smart pointers | Memory is owned by CPU; no shared/unique_ptr needed. | `Memory memory;` not `std::unique_ptr<Memory>` |
| **Avoid:** Templates | No generic programming needed for 32-bit RISC-V. | (No template code) |
| **Avoid:** Exceptions | Traps are architectural, not exceptional. | `std::expected<T, Trap>` instead of `try/catch` |

## Build Instructions

### Prerequisites
- GCC 11+ or Clang 13+ (C++23 support required)
- CMake 3.20+

### Clean Build
```bash
cd /home/ahmed/Mentor/OS_Projetcs/C++_CPU_Simulator
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run Simulator
```bash
./build/simulator
```

**Output:**
```
RV32I CPU Simulator
===================

Executing program:
  ADDI x1, x0, 10  (x1 = 10)
  ADDI x2, x0, 20  (x2 = 20)
  ADD x3, x1, x2   (x3 = 30)

PC=00000000 INST=00a00093 ADDI x1, x0, 10
  x1: 0 → 10

PC=00000004 INST=01400113 ADDI x2, x0, 20
  x2: 0 → 20

PC=00000008 INST=002081b3 ADD x3, x1, x2
  x3: 0 → 30

Final register values:
  x1 = 10
  x2 = 20
  x3 = 30
  PC = 0xc
```

## Test Instructions

### Run All Tests
```bash
cd /home/ahmed/Mentor/OS_Projetcs/C++_CPU_Simulator/build
./test_cpu
./test_memory
```

### Test Coverage

**CPU Tests (12 total):**
1. Initial CPU state (PC=0, all registers=0)
2. ADDI immediate load
3. ADD multi-operand
4. SUB subtraction
5. x0 hardwiring (write attempts are ignored)
6. Sign extension (negative immediates)
7. Illegal instruction trap
8. XOR bitwise operation
9. OR bitwise operation
10. AND bitwise operation
11. PC progression (PC += 4 per instruction)
12. Integration test (4-instruction program)

**Memory Tests (19 total):**
1. Single byte read/write
2. Byte alignment
3. Half-word read/write
4. Half-word alignment
5. Word read/write
6. Word alignment
7. Boundary checks
8. Little-endian byte order
9. Multiple accesses
10-19. Additional edge cases

**Test Exit Code:** 0 on success, 1 on failure

## Example Execution

### Simple Program
```
ADDI x1, x0, 10    (x1 = 10)
ADDI x2, x0, 20    (x2 = 20)
ADD  x3, x1, x2    (x3 = 30)
SUB  x4, x3, x1    (x4 = 20)
```

**Execution Trace:**
```
Step 1: Fetch 0x00A00093 (ADDI)
        Decode: opcode=0x13, rd=1, rs1=0, imm=10
        Execute: x1 = 0 + 10 = 10
        PC: 0x0 → 0x4

Step 2: Fetch 0x01400113 (ADDI)
        Decode: opcode=0x13, rd=2, rs1=0, imm=20
        Execute: x2 = 0 + 20 = 20
        PC: 0x4 → 0x8

Step 3: Fetch 0x002081B3 (ADD)
        Decode: opcode=0x33, rd=3, rs1=1, rs2=2, funct3=0, funct7=0
        Execute: x3 = 10 + 20 = 30
        PC: 0x8 → 0xC

Step 4: Fetch 0x40118233 (SUB)
        Decode: opcode=0x33, rd=4, rs1=3, rs2=1, funct3=0, funct7=32
        Execute: x4 = 30 - 10 = 20
        PC: 0xC → 0x10
```

**Final State:**
```
Registers: x1=10, x2=20, x3=30, x4=20, PC=0x10
```

## Roadmap

### Phase 1: ✅ Complete
- [x] Explicit CPU state machine with CpuStage enum
- [x] Function-pointer dispatch for instruction execution
- [x] Basic ALU: ADDI, ADD, SUB, AND, OR, XOR
- [x] Memory abstraction with alignment checking
- [x] Trap handling with std::expected
- [x] Instruction tracing
- [x] Comprehensive tests

### Phase 2: Load/Store Instructions
- [ ] Implement load operations: LB, LH, LW, LBU, LHU
- [ ] Implement store operations: SB, SH, SW
- [ ] Sign/zero extension logic for loads
- [ ] Memory stage becomes non-trivial (execute load/store here)
- [ ] Test load/store with various alignments

### Phase 3: Branches and Jumps
- [ ] Implement branches: BEQ, BNE, BLT, BGE, BLTU, BGEU
- [ ] Implement jumps: JAL, JALR
- [ ] PC-relative immediate decoding
- [ ] Branch prediction (optional)
- [ ] Test branch taken/not-taken paths

### Phase 4: Advanced ALU
- [ ] Shift operations: SLLI, SRLI, SRAI
- [ ] Comparison: SLT, SLTU, SLTI, SLTIU
- [ ] Test with boundary values

### Phase 5: Environment and Exceptions
- [ ] ECALL (environment call)
- [ ] EBREAK (breakpoint)
- [ ] CSR instructions (MTVEC, MEPC, MSTATUS, etc.)
- [ ] Exception handling framework

### Phase 6: Tracing and Debugging
- [ ] Breakpoints (halt on address, trap, or condition)
- [ ] State snapshots (save/restore full CPU state)
- [ ] Instruction count statistics
- [ ] Memory access profiling
- [ ] Disassembler (print program as mnemonics)

### Phase 7: Compliance and Optimization
- [ ] RISC-V compliance testing (against official test suite)
- [ ] Pipelined execution (true 5-stage pipeline)
- [ ] Just-in-time compilation (optional)
- [ ] Performance profiling

## References

- **RISC-V Specification**: https://riscv.org/technical/specifications/
- **RV32I Base Instruction Set**: Chapter 2 of RISC-V Unprivileged ISA Manual
- **RISC-V Instruction Formats**: https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
- **C++23 Standard**: https://en.cppreference.com/w/cpp/23
- **`std::expected` Reference**: https://en.cppreference.com/w/cpp/utility/expected

## License

This project is educational. Use freely.

---

**Author:** Ahmed (using GitHub Copilot CLI)  
**Last Updated:** 2024
