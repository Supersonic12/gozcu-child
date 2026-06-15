#include "connHandler.hpp"

#include <boost/asio/error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <memory>
#include <nlohmann/json.hpp>

#include "../utility/raiiguardsocket.hpp"
#include "dataWrapper.hpp"
using boost::asio::ip::tcp;
void ConnHandler::startConnection()
{
    if (getState() == Connected)
    {
        return;
    }
    setState(ConnState::Connecting);
    tcp::resolver::results_type endpoints =
        (resolver_.resolve(serverInfo_.ipAddress, serverInfo_.portAddress));
    boost::system::error_code ec;
    boost::asio::connect(*socket_, endpoints, ec);

    if (ec)
    {
        setState(ConnectionFailed);
        std::cerr << ec.message() << std::endl;
        resetSocket(socket_, ioContext_);
        return;
    }
    else
    {
        setState(Connected);

        std::shared_ptr<std::string> greetMsg = std::make_shared<std::string>("Client connected\n");
        boost::system::error_code errorCode;
        boost::asio::write(*socket_, boost::asio::buffer(*greetMsg), errorCode);
        if (errorCode)
        {
            std::cerr << errorCode.message() << std::endl;
            setState(ConnState::ConnectionFailed);
            resetSocket(socket_, ioContext_);
            return;
        }
    }
}
void ConnHandler::endConnection()
{
    if (getState() == ConnState::Disconnected)
    {
        return;
    }
    // these are error codes
    boost::system::error_code shutdownCode, closeCode;
    socketEnd(socket_, shutdownCode, closeCode);
    if (shutdownCode)
    {
        std::cerr << "problem ending connection:" << shutdownCode.message() << std::endl;
        setState(ConnState::DisconnectionFailed);
    }
    if (closeCode)
    {
        std::cerr << "problem closing socket" << closeCode.message() << std::endl;
        setState(ConnState::DisconnectionFailed);
    }
    else
    {
        setState(ConnState::Disconnected);
    }
}

void ConnHandler::sendData()
{
    if (getState() != ConnState::Connected)
    {
        return;
    }
    eventData dataSent = queueToSend_->waitReturnData();
    json dataSentJson;
    if (dataSent.fileHash == "")
    {
        dataSentJson = wrapData(dataSent, false);
    }
    else
    {
        dataSentJson = wrapData(dataSent, true);
    }
    std::shared_ptr<std::string> strDataSent =
        std::make_shared<std::string>(dataSentJson.dump() + "\n");
    boost::system::error_code errorCode;
    boost::asio::write(*socket_, boost::asio::buffer(*strDataSent), errorCode);
    if (errorCode)
    {
        if (errorCode == boost::asio::error::broken_pipe ||
            errorCode == boost::asio::error::connection_reset ||
            errorCode == boost::asio::error::connection_aborted ||
            errorCode == boost::asio::error::not_connected ||
            errorCode == boost::asio::error::bad_descriptor ||
            errorCode == boost::asio::error::timed_out)
        {
            setState(ConnState::ConnectionFailed);
            resetSocket(socket_, ioContext_);
        }
        std::cerr << "Data failed to send:" << errorCode.message() << std::endl;
    }
    else
    {
        queueToSend_->PopData();
    }
}
void ConnHandler::scheduleReconnect(int seconds)
{
    reconnectCounter_++;
    reconnectTimer_.expires_after(std::chrono::seconds(seconds));
    reconnectTimer_.wait();
    startConnection();
}
ConnState ConnHandler::getState() { return state_; }
void ConnHandler::setState(ConnState status) { state_ = status; }
