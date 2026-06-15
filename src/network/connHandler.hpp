#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <memory>

#include "../utility/serverInfo.hpp"
#include "connState.hpp"
#include "queueHandler.hpp"
// this class should handle connection/disconnection to server's
// using port 47001
class ConnHandler
{
   private:
    // a tcp resolver;
    boost::asio::ip::tcp::resolver resolver_;
    /*
    // host ip our server works on
    std::string host_ = "127.0.0.1";
    // port number our server will work as service
    std::string port_ = "47001";
    */
    std::optional<boost::asio::ip::tcp::socket> socket_;
    std::shared_ptr<queueHandler> queueToSend_;
    ServerInfo serverInfo_;

    boost::asio::steady_timer reconnectTimer_;
    int reconnectCounter_;
    std::atomic<bool>& keepConnection_;
    boost::asio::io_context& ioContext_;

   public:
    // here in constructer we assign resolver and socket to our iocontext
    ConnHandler(boost::asio::io_context& iocontext, std::shared_ptr<queueHandler>& queueToSend,
                ServerInfo& serverinfo, std::atomic<bool>& keepConnection)
        : resolver_(iocontext),
          socket_(iocontext),
          queueToSend_(queueToSend),
          serverInfo_(serverinfo),
          reconnectTimer_(iocontext),
          keepConnection_(keepConnection),
          ioContext_(iocontext)
    {
        std::cout << "ConnHandler got created" << std::endl;
    }
    // this will connect to socket
    void startConnection();
    // this shall end connection to socket
    void endConnection();
    // this is for test purposes currently and is for only send text data
    void sendData();
    void setState(ConnState state);
    ConnState getState();
    void scheduleReconnect(int seconds);
    ConnState state_ = ConnState::Disconnected;
};
