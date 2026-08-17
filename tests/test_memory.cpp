#include <cassert>

#include "memory.hpp"

int main()
{
    // Normal memory used for most tests.
    rv32::Memory memory{16};

    // Special memory used for the boundary test.
    rv32::Memory boundary_memory{17};


    // --------------------------------------------------
    // Test 1: read8() from initialized memory
    // --------------------------------------------------

    auto result = memory.read8(0);

    assert(result.has_value());
    assert(*result == 0);


    // --------------------------------------------------
    // Test 2: write8() and read8()
    // --------------------------------------------------

    auto write_result = memory.write8(3, 0x93);

    assert(write_result.has_value());

    auto read_result = memory.read8(3);

    assert(read_result.has_value());
    assert(*read_result == 0x93);


    // --------------------------------------------------
    // Test 3: invalid read8()
    // --------------------------------------------------

    auto invalid_read = memory.read8(16);

    assert(!invalid_read.has_value());
    assert(
        invalid_read.error()
        == rv32::Trap::LoadAccessFault
    );


    // --------------------------------------------------
    // Test 4: invalid write8()
    // --------------------------------------------------

    auto invalid_write = memory.write8(16, 0xFF);

    assert(!invalid_write.has_value());
    assert(
        invalid_write.error()
        == rv32::Trap::StoreAccessFault
    );

    // --------------------------------------------------
    // Test 5: aligned read16()
    // --------------------------------------------------

    auto aligned_read = memory.read16(4);

    assert(aligned_read.has_value());
    assert(*aligned_read == 0);


    // --------------------------------------------------
    // Test 6: misaligned read16()
    // --------------------------------------------------

    auto misaligned_read = memory.read16(3);

    assert(!misaligned_read.has_value());
    assert(
        misaligned_read.error()
        == rv32::Trap::LoadAddressMisaligned
    );


    // --------------------------------------------------
    // Test 7: read16() at memory boundary
    //
    // boundary_memory has addresses:
    //
    // 0 ... 16
    //
    // read16(16) needs:
    //
    // 16 -> valid
    // 17 -> invalid
    //
    // Address 16 itself is aligned:
    //
    // 16 % 2 == 0
    //
    // Therefore the correct error is:
    //
    // LoadAccessFault
    // --------------------------------------------------

    auto boundary_read = boundary_memory.read16(16);

    assert(!boundary_read.has_value());

    assert(
        boundary_read.error()
        == rv32::Trap::LoadAccessFault
    );


    // --------------------------------------------------
    // Test 8: little-endian read16()
    // --------------------------------------------------

    // Store:
    //
    // address 4 = 0x34
    // address 5 = 0x12
    //
    // Little-endian representation:
    //
    // address 5     address 4
    //    0x12          0x34
    //
    // Therefore:
    //
    // read16(4) == 0x1234
    // --------------------------------------------------

    assert(
        memory.write8(4, 0x34).has_value()
    );

    assert(
        memory.write8(5, 0x12).has_value()
    );

    auto endian_read = memory.read16(4);

    assert(endian_read.has_value());
    assert(*endian_read == 0x1234);
    // ==================================================
    // write16() tests
    // ==================================================

    // --------------------------------------------------
    // Test 9: normal write16() and read16()
    // --------------------------------------------------

    auto write16_result = memory.write16(6, 0x1234);

    assert(write16_result.has_value());

    auto read16_result = memory.read16(6);

    assert(read16_result.has_value());
    assert(*read16_result == 0x1234);


    // --------------------------------------------------
    // Test 10: verify little-endian byte layout
    // --------------------------------------------------

    auto low_byte = memory.read8(6);
    auto high_byte = memory.read8(7);

    assert(low_byte.has_value());
    assert(high_byte.has_value());

    // 0x1234 is stored as:
    // address 6 -> 0x34
    // address 7 -> 0x12

    assert(*low_byte == 0x34);
    assert(*high_byte == 0x12);


    // --------------------------------------------------
    // Test 11: misaligned write16()
    // --------------------------------------------------

    auto misaligned_write16 = memory.write16(3, 0x1234);

    assert(!misaligned_write16.has_value());

    assert(
        misaligned_write16.error()
        == rv32::Trap::StoreAddressMisaligned
    );


    // --------------------------------------------------
    // Test 12: boundary write16()
    // --------------------------------------------------

    rv32::Memory boundary_write_memory{17};

    auto boundary_write16 =
        boundary_write_memory.write16(16, 0x1234);

    assert(!boundary_write16.has_value());

    assert(
        boundary_write16.error()
        == rv32::Trap::StoreAccessFault
    );
    // ==================================================
    // read32() tests
    // ==================================================

    // --------------------------------------------------
    // Test 13: read32() little-endian
    // --------------------------------------------------

    assert(memory.write8(8, 0x78).has_value());
    assert(memory.write8(9, 0x56).has_value());
    assert(memory.write8(10, 0x34).has_value());
    assert(memory.write8(11, 0x12).has_value());

    auto read32_result = memory.read32(8);

    assert(read32_result.has_value());
    assert(*read32_result == 0x12345678);


    // --------------------------------------------------
    // Test 14: misaligned read32()
    // --------------------------------------------------

    auto misaligned_read32 = memory.read32(6);

    assert(!misaligned_read32.has_value());

    assert(
        misaligned_read32.error()
        == rv32::Trap::LoadAddressMisaligned
    );


    // --------------------------------------------------
    // Test 15: boundary read32()
    // --------------------------------------------------

    rv32::Memory boundary_read32_memory{17};

    auto boundary_read32 =
        boundary_read32_memory.read32(16);

    assert(!boundary_read32.has_value());

    assert(
        boundary_read32.error()
        == rv32::Trap::LoadAccessFault
    );
    // ==================================================
    // write32() tests
    // ==================================================

    // --------------------------------------------------
    // Test 16: write32() and read32()
    // --------------------------------------------------

    auto write32_result = memory.write32(12, 0x12345678);

    assert(write32_result.has_value());

    auto read_after_write32 = memory.read32(12);

    assert(read_after_write32.has_value());
    assert(*read_after_write32 == 0x12345678);


    // --------------------------------------------------
    // Test 17: verify little-endian byte layout
    // --------------------------------------------------

    auto byte0 = memory.read8(12);
    auto byte1 = memory.read8(13);
    auto byte2 = memory.read8(14);
    auto byte3 = memory.read8(15);

    assert(byte0.has_value());
    assert(byte1.has_value());
    assert(byte2.has_value());
    assert(byte3.has_value());

    assert(*byte0 == 0x78);
    assert(*byte1 == 0x56);
    assert(*byte2 == 0x34);
    assert(*byte3 == 0x12);


    // --------------------------------------------------
    // Test 18: misaligned write32()
    // --------------------------------------------------

    auto misaligned_write32 =
        memory.write32(6, 0x12345678);

    assert(!misaligned_write32.has_value());

    assert(
        misaligned_write32.error()
        == rv32::Trap::StoreAddressMisaligned
    );


    // --------------------------------------------------
    // Test 19: boundary write32()
    // --------------------------------------------------

    rv32::Memory boundary_write32_memory{17};

    auto boundary_write32 =
        boundary_write32_memory.write32(16, 0x12345678);

    assert(!boundary_write32.has_value());

    assert(
        boundary_write32.error()
        == rv32::Trap::StoreAccessFault
    );
    return 0;
}