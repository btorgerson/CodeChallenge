#ifndef WORKER_H
#define WORKER_H

#include <future>
#include <memory>

#include "NTL/ZZ.h"

class Worker: public std::enable_shared_from_this<Worker>
{
    public:
        Worker(uint8_t* buffer, size_t dataLen);
        Worker(const Worker& toCopy);
        ~Worker();
        bool operator() ();

        std::future<bool> workResult;

    private:
        NTL::ZZ  m_largeInt;
};

#endif