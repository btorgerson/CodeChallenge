
#include "Network.h"
#include "Worker.h"

#include <chrono>
#include <future>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using namespace std;

#define ZERO_TIME std::chrono::milliseconds(0)

Network::Network(boost::asio::io_service* io_service): m_newSocket(0), m_io_service(io_service)
{
}

Network::~Network()
{
}

void Network::Start()
{
    std::thread([this]()
    {
        //Notify the io service of work. The io_service object will run as long as this work object is not destroyed
        boost::asio::io_service::work work(*m_io_service); 

        //Acceptor of connections on TCP port 6000. The interface bound to is not specified but that is possible
        boost::asio::ip::tcp::acceptor acceptor(*m_io_service, tcp::endpoint(tcp::v4(), 6000));

        //Todo: no way to signal termination yet
        while(1)
        {
            //Accept a new connection asynchronously
            if(!m_newSocket)
            {
                m_newSocket = make_shared<tcp::socket>( *m_io_service );

                acceptor.async_accept( *m_newSocket,
                                    boost::bind(&Network::ConnectionHandler, this, m_newSocket,
                                    boost::asio::placeholders::error));
            }

            //Starts async reads for all connections not already reading, starts asynchronous work when all data is read, and writes the results of asynchronous operations
            for(auto conn : m_connections)
            {
                //Check if the connection already has a pending async read
                if(!conn->pendingRead.valid())
                { 
                    //If the received size is 0, then we haven't received a header yet
                    if(conn->received == 0)
                    {
                        //Read the header first
                        conn->pendingRead = async_read(*conn->socket, boost::asio::buffer((uint8_t*)&conn->recvHeader, sizeof(PrimeTestMsgHdr)), boost::asio::use_future );
                    }
                    else if(conn->received == sizeof(PrimeTestMsgHdr))
                    {
                        //Read the remainder of the message
                        uint32_t packetSize = conn->recvHeader.size;
                        conn->recvBuffer.resize(packetSize, 0);
                        conn->pendingRead = async_read(*conn->socket, boost::asio::buffer(conn->recvBuffer.data(), packetSize), boost::asio::use_future );
                    }
                }
                else
                {
                    //A read request was issued, check if it's completed. no blocking and no waiting
                    future_status status = conn->pendingRead.wait_for(std::chrono::milliseconds(0));
                    
                    //An asynchronous read has completed
                    if(status == future_status::ready)
                    {
                        //The future result is the read length
                        size_t readLen = conn->pendingRead.get();

                        if(conn->received == 0)
                        {    
                            //No bytes received yet so this is a header
                            if(readLen != sizeof(PrimeTestMsgHdr))
                            {
                                cout << "Read error" << endl;
                            }
                            else
                            {
                                conn->received = readLen;
                            }
                        }
                        else
                        {
                            //Bytes have been received so this is the integer data
                            std::shared_ptr<Worker> worker = make_shared<Worker>(conn->recvBuffer.data(), conn->recvBuffer.size());

                            //Start an async thread with the worker functor
                            //A possible improvement to this approach would be to use packaged_task to queue up
                            //requests while only working on a fixed number of requests at a time.
                            worker->workResult = async(launch::async, *worker);

                            //The packet is complete, reset buffer and recv count
                            conn->received = 0;
                            conn->recvBuffer.resize(0);
                            conn->pendingWork.push_back(worker);
                        }
                    }
                }//else

                //Clear the write future if the async write has completed
                if(conn->pendingWrite.valid())
                {
                    future_status status = conn->pendingWrite.wait_for(std::chrono::milliseconds(0));
                    if(status == future_status::ready)
                    {
                        size_t written = conn->pendingWrite.get();
                        if(written != sizeof(PrimeResponseMsg))
                            cout << "Error writing response packet." << endl;
                    }
                }

                //Check for work completed and perform a write of the response
                auto workerIter = conn->pendingWork.begin();
                for(; workerIter != conn->pendingWork.end(); ++workerIter)
                {
                    if((*workerIter)->workResult.valid())
                    {
                        //Processing is happening but is the result ready? Do not block waiting for the result
                        future_status status = (*workerIter)->workResult.wait_for(std::chrono::milliseconds(0));
                        if(status == future_status::ready)
                        {
                            //The processing result is ready
                            bool isPrime = (*workerIter)->workResult.get();
                            
                            //Return key indicating to the client which request is being responded to
                            conn->writeBuffer.key     = conn->recvHeader.key;
                            conn->writeBuffer.isPrime = isPrime;

                            conn->pendingWrite = async_write(*conn->socket, boost::asio::buffer((char*)&conn->writeBuffer, sizeof(PrimeResponseMsg)), boost::asio::use_future );
                        }
                    }
                    else
                    {
                        workerIter = conn->pendingWork.erase(workerIter);
                    }
                }//for

            } //for

            //Sanity sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        } //while
    }).detach(); //thread
}

void Network::ConnectionHandler(shared_ptr<tcp::socket> newSocket, const boost::system::error_code& error)
{
    std::shared_ptr<ClientConnection> newConnection = make_shared<ClientConnection>();
    newConnection->socket   = newSocket;
    newConnection->received = 0;

    newConnection->writeBuffer.key     = 0;
    newConnection->writeBuffer.isPrime = 0;
    
    m_connections.push_back(newConnection);

    //Set the new socket to nullptr to indicate that a new async accept is required
    m_newSocket = nullptr;
}
