#include "connHandler.hpp"

#include <boost/system/detail/error_code.hpp>
#include <cstddef>

void ConnHandler::startConnection()
{
    boost::asio::ip::tcp::resolver::results_type endpoints = (resolver_.resolve(host_, port_));

    boost::asio::connect(socket_, endpoints);
}
void ConnHandler::sendData()
{
    std::string sentData = "some bullshit";
    boost::asio::async_write(
        socket_, boost::asio::buffer(sentData),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred)
        { handleEndOfSendData(error, bytes_transferred); });
}
void ConnHandler::handleEndOfSendData(const boost::system::error_code& error,
                                      std::size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "Success!" << std::endl;
    }
}
