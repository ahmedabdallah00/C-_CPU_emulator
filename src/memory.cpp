#include "memory.hpp"

namespace rv32{
    Memory::Memory(std::size_t size) : data_(size,0)
    {

    }

    std::expected<Byte, Trap> Memory::read8(Word addr) const noexcept
    {
        if (addr >= data_.size())
        {
            return std::unexpected(Trap::LoadAccessFault);
        }

        return data_[addr];
    }

    std::expected<void, Trap> Memory::write8(Word addr, Byte data) noexcept
    {
        if (addr >= data_.size())
        {
            return std::unexpected(Trap::StoreAccessFault);
        }
        data_[addr]= data;
        return {};
    }

    std::expected<HalfWord, Trap> Memory::read16(Word addr) const noexcept
    {
        if (addr %2 != 0)
        {
            return std::unexpected(Trap::LoadAddressMisaligned);
        }
        
        const auto address = static_cast<std::size_t>(addr);

        if (address >= data_.size())
        {
            return std::unexpected(Trap::LoadAccessFault);
        }

        if (data_.size() - address < 2)
        {
            return std::unexpected(Trap::LoadAccessFault);
        }
        const HalfWord low = data_[addr];
        const HalfWord high = data_[addr+1];
        return static_cast<HalfWord>(low | (high << 8));
    }
    std::expected<void, Trap> Memory::write16(Word addr, HalfWord data) noexcept
    {
        if (addr %2 != 0)
        {
            return std::unexpected(Trap::StoreAddressMisaligned);
        }
        
        const auto address = static_cast<std::size_t>(addr);

        if (address >= data_.size())
        {
            return std::unexpected(Trap::StoreAccessFault);
        }

        if (data_.size() - address < 2)
        {
            return std::unexpected(Trap::StoreAccessFault);
        }
        data_[address] = static_cast<Byte>(data & 0xFF);
        data_[address+1] = static_cast<Byte>((data >> 8) & 0xFF );
        
        return {};
    }
    
    std::expected<Word, Trap> Memory::read32(Word addr) const noexcept
    {
        if (addr %4 != 0)
        {
            return std::unexpected(Trap::LoadAddressMisaligned);
        }
        
        const auto address = static_cast<std::size_t>(addr);

        if (address >= data_.size())
        {
            return std::unexpected(Trap::LoadAccessFault);
        }

        if (data_.size() - address < 4)
        {
            return std::unexpected(Trap::LoadAccessFault);
        }
        const Word b0 = data_[address];
        const Word b1 = data_[address + 1];
        const Word b2 = data_[address + 2];
        const Word b3 = data_[address + 3];
        return b0 | (b1<<8) | (b2<<16) | (b3<<24);
    }

    std::expected<void, Trap> Memory::write32(Word addr, Word data) noexcept
    {
        if (addr %4 != 0)
        {
            return std::unexpected(Trap::StoreAddressMisaligned);
        }
        
        const auto address = static_cast<std::size_t>(addr);

        if (address >= data_.size())
        {
            return std::unexpected(Trap::StoreAccessFault);
        }

        if (data_.size() - address < 4)
        {
            return std::unexpected(Trap::StoreAccessFault);
        }
        
        data_[address] = static_cast<Byte>(data & 0xFF);
        data_[address + 1] = static_cast<Byte>((data >> 8) & 0xFF);
        data_[address + 2] = static_cast<Byte>((data >> 16) & 0xFF);
        data_[address + 3] = static_cast<Byte>((data >> 24) & 0xFF);
        
        return {};
    }
}