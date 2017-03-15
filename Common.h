#ifndef COMMON_H
#define COMMON_H

#include <cstdint>

struct PrimeTestMsgHdr
{
    std::uint32_t size;
    std::uint32_t key;
};

struct PrimeResponseMsg
{
    std::uint32_t key;
    std::uint32_t isPrime;
};

#endif