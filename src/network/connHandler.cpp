#include "connHandler.hpp"

#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <nlohmann/json.hpp>

#include "dataWrapper.hpp"
using boost::asio::ip::tcp;
void ConnHandler::startConnection()
{
    tcp::resolver::results_type endpoints = (resolver_.resolve(host_, port_));

    boost::asio::connect(socket_, endpoints);
}
void ConnHandler::endConnection()
{
    boost::system::error_code ec;
    // apparently here we close send and receive streams, and write error code separately
    socket_.shutdown(tcp::socket::shutdown_both, ec);
    // and this is to free system resources. i will need to write a socket RAIII function later
    // TODO: socket handler raiii guard
    socket_.close(ec);
}

void ConnHandler::sendData()
{
    json dataSent = wrapData(queueToSend_->popAndReturn());
    std::string strDataSent = dataSent.dump();
    boost::asio::async_write(
        socket_, boost::asio::buffer(strDataSent),
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
