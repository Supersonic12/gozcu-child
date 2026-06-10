#include <boost/asio.hpp>
#include <cstdint>
#include <memory>

#include "gatherer/eventWatcher.hpp"
#include "network/connHandler.hpp"
#include "network/queueHandler.hpp"
#include "utility/configHandler.hpp"
/* This class is core of gozcu-child project
 * It should get watch Mask from config file
 * It should basically create an eventWatcher class object
 */
class Controller
{
   public:
    Controller();
    // i need something to initialize all objects inside controller like mother of initialization
    void initGod();
    // init watcherMask is for creating maskStruct object and build mask.
    void initWatcherMask();

    /*
     * i need a queue to where our agent push data and network will process it as frames and send
     * it. and also an incoming queue but it will be implemented later
     * std::queue<eventData> sentDataQueue;
     * i need a wrapper for this queue to be able to work safely
     */

    void initConnProtocol();
    void initWatcher();

   private:
    // this struct is for getting FANOTIFY_MARK event mask from config and build
    // it
    maskStruct myMask;
    // this variable is for saving result of building mask in maskStruct
    uint64_t watcherMask_ = 0;
    std::shared_ptr<queueHandler> outgoingQueueHandler_ = std::make_shared<queueHandler>();
    boost::asio::io_context io_context_;
};
