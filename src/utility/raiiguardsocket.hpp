#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <optional>
inline void resetSocket(std::optional<boost::asio::ip::tcp::socket>& socket,
                        boost::asio::io_context& ioContext)
{
    if (socket && socket->is_open())
    {
        boost::system::error_code ec;
        socket->close(ec);
    }
    std::cerr << "Socket closed" << std::endl;
    socket.emplace(ioContext);
    std::cerr << "Socket emplaced" << std::endl;
}
inline void socketEnd(std::optional<boost::asio::ip::tcp::socket>& socket,
                      boost::system::error_code& shutdownCode, boost::system::error_code& closeCode)
{
    socket->cancel();
    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdownCode);
    if (socket->is_open())
    {
        socket->close(closeCode);
    }
}
