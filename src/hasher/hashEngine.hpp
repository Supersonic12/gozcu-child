#pragma once
#include <atomic>
#include <memory>

#include "../eventData.hpp"
#include "../network/queueHandler.hpp"
class HashEngine
{
   private:
    std::shared_ptr<queueHandler>& hashQueueHandler_;
    std::shared_ptr<queueHandler>& outgoingQueueHandler_;
    std::atomic<bool>& keepHashing_;

   public:
    HashEngine(std::shared_ptr<queueHandler>& hashQueueHandler,
               std::shared_ptr<queueHandler>& outgoingQueueHandler, std::atomic<bool>& keepHashing)
        : hashQueueHandler_(hashQueueHandler),
          outgoingQueueHandler_(outgoingQueueHandler),
          keepHashing_(keepHashing)
    {
    }
    std::string hashFile(eventData& data);

    void populateOutgoingQueue(eventData& data);
    void cleanUpHashEngine();
};
