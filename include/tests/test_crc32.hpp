#ifndef WEAK_TEST_CRC32_HPP
#define WEAK_TEST_CRC32_HPP

#include "../crc32.hpp"

#include "../error/error.hpp"

namespace crc32_detail {

void run(const char* payload, unsigned long expected_hash)
{
    if (auto hash = crc32::create(payload); hash != expected_hash) {
        throw RuntimeError("CRC32 error: got {}, expected {}", hash, expected_hash);
    }
}

} // namespace crc32_detail

void run_crc32_tests()
{
    crc32_detail::run("", 0);
    crc32_detail::run("123456789", 0xCBF43926);
    crc32_detail::run("00000101010101001000101001101011001101010", 0x46C6EA5E);
}

#endif // WEAK_TEST_CRC32_HPP
