#include <mutex>
#include <queue>

#include "eventData.hpp"
class queueHandler
{
   private:
    //
   public:
    // this is where the event data is pushed by engine
    // and popped by network;
    std::shared_ptr<std::queue<eventData>> eventDatas = std::make_shared<std::queue<eventData>>();
    std::mutex eventDatasMutex;

    //
};
