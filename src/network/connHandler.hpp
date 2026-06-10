#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <iostream>
#include <memory>

#include "queueHandler.hpp"
// this class should handle connection/disconnection to server's
// using port 47001
using boost::asio::ip::tcp;
class ConnHandler
{
   private:
    // a tcp resolver;
    tcp::resolver resolver_;
    // host ip our server works on
    std::string host_ = "127.0.0.1";
    // port number our server will work as service
    std::string port_ = "47001";
    tcp::socket socket_;
    std::shared_ptr<queueHandler> queueToSend_;

   public:
    // here in constructer we assign resolver and socket to our iocontext
    ConnHandler(boost::asio::io_context& iocontext, std::shared_ptr<queueHandler>& queueToSend)
        : resolver_(iocontext), socket_(iocontext), queueToSend_(queueToSend)
    {
        std::cout << "ConnHandler got created" << std::endl;
    }
    // this will connect to socket
    void startConnection();
    // this shall end connection to socket
    void endConnection();
    // this is for test purposes currently and is for only send text data
    void sendData();
    // this will work as callback function of async_write but tbh i don't even need this i can
    // simply use lambda
    void handleEndOfSendData(const boost::system::error_code& error, std::size_t bytes_transferred);
};
