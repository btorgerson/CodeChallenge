#include "Worker.h"

#include <cstring>

#include "NTL/ZZ.h"

Worker::Worker(uint8_t* buffer, size_t dataLen)
{
    m_largeInt = NTL::ZZFromBytes(buffer, dataLen);
}

Worker::Worker(const Worker& toCopy)
{
    m_largeInt = toCopy.m_largeInt;
}

Worker::~Worker()
{
}

bool Worker::operator() ()
{
    long result = ProbPrime(m_largeInt, 30);

    if(result == 0)
        return false;
    else
        return true;
}