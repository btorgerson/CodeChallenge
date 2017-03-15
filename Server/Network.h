#ifndef NETWORK_H
#define NETWORK_H

#include "Common.h"
#include "Worker.h"

#include <list>
#include <future>
#include <memory>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#define READ_BUFFER_SIZE 1024


struct ClientConnection
{
    std::shared_ptr<tcp::socket>       socket;
    std::future<size_t>                pendingRead;
    std::future<size_t>                pendingWrite;
    std::list<std::shared_ptr<Worker>> pendingWork;
    std::vector<uint8_t>               recvBuffer;
    PrimeTestMsgHdr                    recvHeader;
    uint8_t                            received;
    PrimeResponseMsg                   writeBuffer;
};

class Network
{
    public:
        Network(boost::asio::io_service* io_service);
        ~Network();

        void Start();
        void ConnectionHandler(std::shared_ptr<tcp::socket>, const boost::system::error_code&);

    private:
        std::list<std::shared_ptr<ClientConnection>> m_connections;
        boost::asio::io_service*                     m_io_service;
        std::shared_ptr<tcp::socket>                 m_newSocket;
};

#endif