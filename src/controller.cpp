#include "controller.hpp"

#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <thread>
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
    // fetch from config
    myMask.fromHandler();
    watcherMask_ = myMask.buildMask();
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
