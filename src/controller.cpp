#include "controller.hpp"

#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <thread>

#include "utility/configHandler.hpp"
Controller::Controller() {}
/*
 * this is basically for initializing everything like needed variables for different tasks.
 * Especially threads, i need to be able to get output of every task
 * or i will have really bad time with debugging
 */
void Controller::initGod()
{
    initWatcherMask();

    std::thread watcherThread(&Controller::initWatcher, this);

    std::thread connThread(&Controller::initConnProtocol, this);

    watcherThread.join();
    connThread.join();
}
// for fanotify module to be initialized
void Controller::initWatcher()
{
    /* here we set watched directory to be /home/jesus/Documents/ASCII/ for
     * testing purposes. It should get this watched directories from config file
     * in close future
     */
    std::vector<std::filesystem::path> paths = {"/home/jesus/Documents/ASCII/"};
    eventWatcher watcher(watcherMask_, paths, outgoingQueueHandler_);
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
    std::map<std::string, bool> maskMap = handler_.getMaskMap();
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
    ConnHandler connHandler_(io_context_, outgoingQueueHandler_);
    connHandler_.startConnection();
    while (true)
    {
        if (!outgoingQueueHandler_->isQueueEmpty())
        {
            connHandler_.sendData();
        }
        outgoingQueueHandler_->waitForCondition();
    }
}

void Controller::initServerCredentials()
{
    //
}
