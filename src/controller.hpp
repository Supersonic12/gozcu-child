#include <boost/asio.hpp>
#include <cstdint>
#include <memory>

#include "gatherer/eventWatcher.hpp"
#include "hasher/hashEngine.hpp"
#include "network/connHandler.hpp"
#include "network/queueHandler.hpp"
#include "utility/configHandler.hpp"
#include "utility/serverInfo.hpp"
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
    void initServerCredentials();
    void initConnProtocol();
    void initWatcher();
    void initHashEngine();

   private:
    // this variable is for saving fanotify_mark mask's built version
    uint64_t watcherMask_ = 0;
    std::shared_ptr<queueHandler> hashQueueHandler_ = std::make_shared<queueHandler>();
    std::shared_ptr<queueHandler> outgoingQueueHandler_ = std::make_shared<queueHandler>();

    boost::asio::io_context io_context_;

    ConfigHandler configHandler_;
    ServerInfo serverInfo_;
    ConnState connState_;
    std::atomic<bool> keepConnection_ = true;

    std::atomic<bool> keepHashing_ = true;
};
