# C++ RISC-V CPU Simulator

A C++23 implementation of a RISC-V RV32I CPU simulator built from scratch.

The project is focused on understanding how a processor actually executes instructions, from instruction fetching and decoding through execution, memory access, and register writeback.

Rather than treating the CPU as a collection of instruction handlers, the simulator is being designed around an explicit execution state machine and a modular instruction-dispatch architecture.

## Architecture

The intended execution flow is:

```text
              ┌──────────────┐
              │    Memory    │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │    Fetch     │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │    Decode    │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │   Execute    │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │    Memory    │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │   Writeback  │
              └──────┬───────┘
                     │
                     ▼
                   Fetch
```

The simulator separates:

* Architectural CPU state
* Memory
* Instruction decoding
* Instruction execution
* CPU execution state
* Trap/error handling

## Current Status

### Memory

Implemented:

* `read8`
* `write8`
* `read16`
* `write16`
* `read32`
* `write32`
* Little-endian memory representation
* Load/store alignment checking
* Load/store access-fault checking

### CPU State

Implemented:

* Program counter
* 32 general-purpose registers
* `x0` hardwired to zero
* Register read/write operations
* PC updates

### CPU Pipeline

Initial fetch/decode/execute architecture:

```text
Fetch
  ↓
Decode
  ↓
Execute
  ↓
Writeback
  ↓
Fetch
```

### Instructions

The initial instruction set includes foundational RV32I integer operations such as:

```text
ADDI
ADD
SUB
AND
OR
XOR
```

The instruction architecture is being extended toward the broader RV32I base instruction set.

## Design Goals

The project has several goals:

1. Understand RISC-V instruction encoding at the bit level.
2. Understand the fetch-decode-execute cycle.
3. Model CPU state explicitly.
4. Build a deterministic memory subsystem.
5. Use explicit architectural traps instead of exceptions for normal CPU faults.
6. Keep instruction decoding separate from instruction execution.
7. Build an extensible instruction-dispatch mechanism.
8. Explore modern C++23 features where they provide real architectural value.

## C++ Design

The simulator uses modern C++ features including:

* C++23
* `std::expected`
* `enum class`
* `std::array`
* `constexpr`
* `[[nodiscard]]`
* `noexcept`
* fixed-width architectural types
* function-pointer/member-function dispatch

Advanced language features are introduced only when they solve a concrete design problem.

The goal is not to use C++ features for their own sake, but to understand how they can be applied to systems programming and CPU simulation.

## CPU State Machine

The simulator is being designed around explicit CPU execution states:

```text
FETCH
  ↓
DECODE
  ↓
EXECUTE
  ↓
MEMORY
  ↓
WRITEBACK
  ↓
FETCH
```

A member-function pointer can be used to represent the active CPU stage:

```cpp
using StageHandler = void (Cpu::*)() noexcept;
```

This allows the CPU to transition explicitly between execution stages.

The architecture also leaves room for future experiments involving:

* multi-cycle instructions
* pipeline stages
* stalls
* tracing
* interrupts
* instruction timing
* debugging

## Error Handling

Architectural failures are represented explicitly using `std::expected`.

Examples include:

```text
LoadAccessFault
StoreAccessFault
LoadAddressMisaligned
StoreAddressMisaligned
IllegalInstruction
```

Normal architectural traps are not implemented using C++ exceptions.

## Testing

The project uses dedicated tests for:

* Memory operations
* Alignment handling
* Boundary conditions
* Little-endian behavior
* CPU register behavior
* `x0` protection
* Instruction execution
* Program-counter behavior

Build:

```bash
cmake -S . -B build
cmake --build build
```

Run CPU tests:

```bash
./build/test_cpu
```

Run memory tests:

```bash
./build/test_memory
```

A successful test returns:

```text
0
```

## Roadmap

### Phase 1 — Foundation

* [x] Memory abstraction
* [x] CPU state
* [x] Register file
* [x] PC management
* [x] Instruction fetch
* [x] Instruction decoding
* [x] Basic ALU instructions

### Phase 2 — CPU Architecture

* [ ] Explicit CPU state machine
* [ ] Function-pointer stage dispatch
* [ ] Instruction dispatch table
* [ ] Expanded RV32I ALU instructions
* [ ] Instruction tracing
* [ ] Improved integration tests

### Phase 3 — Memory Instructions

* [ ] `LB`
* [ ] `LH`
* [ ] `LW`
* [ ] `LBU`
* [ ] `LHU`
* [ ] `SB`
* [ ] `SH`
* [ ] `SW`

### Phase 4 — Control Flow

* [ ] Conditional branches
* [ ] `JAL`
* [ ] `JALR`
* [ ] PC-relative execution
* [ ] Branch testing

### Phase 5 — System Architecture

* [ ] Illegal-instruction handling
* [ ] More complete trap behavior
* [ ] Instruction tracing/debugging
* [ ] Program loading
* [ ] Small executable programs
* [ ] RV32I compliance-oriented testing

## Example

A small program can eventually be represented as:

```text
ADDI x1, x0, 10
ADDI x2, x0, 20
ADD  x3, x1, x2
SUB  x4, x3, x1
```

Expected state:

```text
x1 = 10
x2 = 20
x3 = 30
x4 = 20
```

The important part is not only the final register values, but the complete execution path:

```text
Instruction
    ↓
Fetch
    ↓
Decode
    ↓
Execute
    ↓
Register/Memory update
    ↓
PC update
```

## References

The implementation is developed alongside study of:

* RISC-V ISA specifications
* Computer architecture concepts
* C++ systems programming
* CPU emulator design

## Project Status

This is an educational and experimental CPU simulator under active development.

The project prioritizes architectural understanding, correctness, testability, and maintainable C++ design over claiming complete RV32I compliance prematurely.
