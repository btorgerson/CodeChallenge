#include "Network.h"

#include <sys/socket.h>

#include <boost/asio.hpp>

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;
    Network network(&io_service);
    network.Start();

    io_service.run();
}
