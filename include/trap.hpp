#pragma once

#include <cstdint>
#include <string_view>

namespace rv32 {

    // Trap enum for explicit error handling
    // Using strongly-typed enum to distinguish architectural traps from memory faults
    enum class Trap : std::uint8_t
    {
        // Memory access faults
        LoadAccessFault,        // 5
        StoreAccessFault,       // 7
        
        // Misalignment faults
        LoadAddressMisaligned,  // 4
        StoreAddressMisaligned, // 6
        
        // Illegal instruction
        IllegalInstruction,     // 2
    };

    // Convert trap to human-readable name
    constexpr std::string_view trap_name(Trap trap) noexcept
    {
        switch (trap)
        {
            case Trap::LoadAddressMisaligned:   return "LoadAddressMisaligned";
            case Trap::LoadAccessFault:         return "LoadAccessFault";
            case Trap::StoreAddressMisaligned:  return "StoreAddressMisaligned";
            case Trap::StoreAccessFault:        return "StoreAccessFault";
            case Trap::IllegalInstruction:      return "IllegalInstruction";
        }
        return "UnknownTrap";
    }

} // namespace rv32
