#include "ClientNetwork.h"

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/bind.hpp>

#include "NTL/ZZ.h"

using namespace std;

using boost::asio::ip::tcp;
using namespace NTL;

ClientNetwork::ClientNetwork():
    m_socket(m_io_service)
{
}

ClientNetwork::~ClientNetwork()
{
}

//Connect to the specified host. ipAddress can be a hostname or an ip address
void ClientNetwork::Connect(string ipAddress)
{
    try
    {
        tcp::resolver resolver(m_io_service);
        tcp::resolver::query query(ipAddress, "6000");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        m_socket = tcp::socket(m_io_service);
        boost::asio::connect(m_socket, endpoint_iterator);
    }
    catch(...)
    {
        cout << "Connection Failed." << endl;
    }
}

//Sends a primality check request to the server. This function blocks until everything is sent
void ClientNetwork::SendRequest(PrimeCheckRequest req)
{
    try
    {
        uint32_t numberSize = NumBytes(req.number);

        uint8_t* buffer = new uint8_t[numberSize];
        
        BytesFromZZ(buffer, req.number, numberSize);

        PrimeTestMsgHdr header;
        header.size = numberSize;
        header.key = req.key;

        boost::system::error_code error;

        boost::asio::write(m_socket, boost::asio::buffer((uint8_t*)&header, sizeof(PrimeTestMsgHdr)), error);
        boost::asio::write(m_socket, boost::asio::buffer(buffer, numberSize), error);

        delete [] buffer;

        cout << "Request Sent" << endl;
    }
    catch(...)
    {
        cout << "Error sending request." << endl;
    }
}

//Fetches the next available result and stores it in the result list
void ClientNetwork::ReadResults()
{
    PrimeResponseMsg response;

    try
    {
        boost::asio::read(m_socket, boost::asio::buffer((uint8_t*)&response, sizeof(PrimeResponseMsg)), boost::asio::transfer_at_least(sizeof(PrimeResponseMsg)));

        m_requestResults.push_back(make_pair(response.key, response.isPrime == 0 ? false : true));

        cout << "Results read" << endl;
    }
    catch(...)
    {
        cout << "Error reading results." << endl;
    }
}

list<pair<uint32_t,bool>>&& ClientNetwork::GetResults()
{
    return move(m_requestResults);
}