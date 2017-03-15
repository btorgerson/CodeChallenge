#include "Network.h"

#include <boost/asio.hpp>
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;
    Network network(&io_service);
    network.Start();

    //Not so good way of preventing a race condition
    //A better way would be the use of signals
    std::this_thread::sleep_for(std::chrono::seconds(1));

    io_service.run();
}
