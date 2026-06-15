#include "controller.hpp"

#include <linux/fanotify.h>
#include <unistd.h>

#include <boost/asio/executor_work_guard.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "eventData.hpp"
#include "hasher/hashEngine.hpp"
#include "utility/configHandler.hpp"
#include "utility/serverInfo.hpp"
Controller::Controller() {}
/*
 * this is basically for initializing everything like needed variables for different tasks.
 * Especially threads, i need to be able to get output of every task
 * or i will have really bad time with debugging
 */
void Controller::initGod()
{
    initWatcherMask();
    initServerCredentials();

    auto work = boost::asio::make_work_guard(io_context_);

    std::thread ioThread(&Controller::io_context_, this);

    std::thread watcherThread(&Controller::initWatcher, this);

    std::thread connThread(&Controller::initConnProtocol, this);
    std::thread hashEngineThread(&Controller::initHashEngine, this);
    work.reset();
    if (ioThread.joinable())
    {
        ioThread.join();
    }
    if (watcherThread.joinable())
    {
        watcherThread.join();
    }
    if (connThread.joinable())
    {
        connThread.join();
    }
    if (hashEngineThread.joinable())
    {
        hashEngineThread.join();
    }
}
// for fanotify module to be initialized
void Controller::initWatcher()
{
    /* here we set watched directory to be /home/jesus/Documents/ASCII/ for
     * testing purposes. It should get this watched directories from config file
     * in close future
     */
    std::vector<std::filesystem::path> paths = {"/home/jesus/Documents/ASCII/"};
    eventWatcher watcher(watcherMask_, paths, hashQueueHandler_);
    try
    {
        watcher.startWatching();
    }
    catch (std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
}
// Init watcherMask basically create an eventWatcher object with given
// directories and masks. Currently it just gives directories, I need to
// refactor it.

void Controller::initWatcherMask()
{
    std::map<std::string, bool> maskMap = configHandler_.getMaskMap();
    uint64_t builtMask = 0;
    std::map<std::string, uint64_t> allMasks = {
        {"use_fan_del", FAN_DELETE},
        {"use_fan_mod", FAN_MODIFY},
        {"use_fan_cl_wr", FAN_CLOSE_WRITE},
        {"use_fan_acc", FAN_ACCESS},
        {"use_fan_attr", FAN_ATTRIB},
        {"use_fan_del_self", FAN_DELETE_SELF},
        {"use_fan_mv_from", FAN_MOVED_FROM},
        {"use_fan_mv_to", FAN_MOVED_TO},
        {"use_fan_mv_self", FAN_MOVE_SELF},
        {"use_fan_op", FAN_OPEN},
        {"use_fan_ev_on_child", FAN_EVENT_ON_CHILD},
        {"use_fan_crt", FAN_CREATE},
    };
    // uint64_t test_all_mask_enabled = 0;
    // for (const auto [key, value] : allMasks) {
    //   test_all_mask_enabled |= value;
    // }
    // std::cout << test_all_mask_enabled << std::endl;
    for (const auto [key, enabled] : maskMap)

    {
        if (enabled)
        {
            auto it = allMasks.find(key);
            if (it != allMasks.end())
            {
                builtMask |= it->second;
            }
        }
    }
    watcherMask_ = builtMask;

    std::cout << watcherMask_ << std::endl;
}

// here new thread for networking should be created but i still have doubts how to do it.
// this is for only demo purposes, i will definitely go over them for optimizations
void Controller::initConnProtocol()
{
    try
    {
        std::cout << "trying connection\n";
        ConnHandler connHandler(io_context_, outgoingQueueHandler_, serverInfo_, keepConnection_);
        connHandler.startConnection();
        while (keepConnection_)
        {
            if (connHandler.getState() == ConnState::ConnectionFailed ||
                connHandler.getState() == ConnState::Disconnected)
            {
                connHandler.scheduleReconnect(5);
                continue;
            }

            connHandler.sendData();
        }
        connHandler.endConnection();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Controller::initServerCredentials() { serverInfo_ = configHandler_.getServerInfo(); }
void Controller::initHashEngine()
{
    try
    {
        HashEngine hashEngine(hashQueueHandler_, outgoingQueueHandler_, keepHashing_);
        while (keepHashing_)

        {
            std::cout << "start waiting for condition\n";

            eventData hashToNetData = hashQueueHandler_->waitReturnPopData();
            std::cout << "waiting for condition passed\n";
            std::cout << hashToNetData.dateTime << "\t-\t" << hashToNetData.filename << "\t-\t"
                      << hashToNetData.mask << "\n";
            hashToNetData.fileHash = "";
            if ((hashToNetData.mask & FAN_MODIFY) || (hashToNetData.mask & FAN_CREATE))
            {
                std::string fileHash = hashEngine.hashFile(hashToNetData);
                hashToNetData.fileHash = fileHash;
                std::cout << "fileHash\t-\t" << hashToNetData.fileHash << std::endl;
            }
            hashEngine.populateOutgoingQueue(hashToNetData);
        }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
