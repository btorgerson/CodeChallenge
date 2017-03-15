#include "Common.h"

#include <list>
#include <map>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/bind.hpp>

#include "NTL/ZZ.h"

using boost::asio::ip::tcp;

struct PrimeCheckRequest
{
    uint32_t key;
    NTL::ZZ  number;
};

class ClientNetwork
{
    public:
        ClientNetwork();
        ~ClientNetwork();

        void Connect(std::string ipAddress);
        void SendRequest(PrimeCheckRequest req);
        void ReadResults();

        std::list<std::pair<uint32_t,bool>>&& GetResults();
    private:
        std::list<std::pair<uint32_t,bool>>          m_requestResults;
        boost::asio::io_service                      m_io_service;
        tcp::socket                                  m_socket;
};