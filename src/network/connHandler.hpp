#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <iostream>
// this class should handle connection/disconnection to server's using port 47001
class ConnHandler
{
   private:
    // a tcp resolver;
    boost::asio::ip::tcp::resolver resolver_;
    // host ip our server works on
    std::string host_ = "127.0.0.1";
    // port number our server will work as service
    std::string port_ = "47001";
    boost::asio::ip::tcp::socket socket_;

   public:
    // here in constructer we assign resolver and socket to our iocontext
    ConnHandler(boost::asio::io_context& iocontext) : resolver_(iocontext), socket_(iocontext)
    {
        std::cout << "ConnHandler got created" << std::endl;
    }
    // this will connect to socket
    void startConnection();
    // this is for test purposes currently and is for only send text data
    void sendData();
    // this will work as callback function of async_write but tbh i don't even need this i can
    // simply use lambda
    void handleEndOfSendData(const boost::system::error_code& error, std::size_t bytes_transferred);
};
