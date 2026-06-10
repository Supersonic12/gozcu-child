#pragma once
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

#include "../eventData.hpp"
class queueHandler
{
   private:
    //
   public:
    // this is where the event data is pushed by engine
    // I mean engine pushes event datas using queue in this class.
    // Then also network gets reference of this class object
    // and pop from it.
    // queueHandler(queueHandler& newQueue) { eventDatas = newQueue.eventDatas; }
    std::shared_ptr<std::queue<eventData>> eventDatas = std::make_shared<std::queue<eventData>>();
    std::mutex eventDatasMutex;
    std::condition_variable cv;
    bool data_ready = false;
    // while a push happens queue is locked.
    void pushData(eventData newData)
    {
        std::unique_lock<std::mutex> lock(eventDatasMutex);

        eventDatas->push(newData);
        data_ready = true;
        cv.notify_one();
    }

    // while a pop happens again queue should be locked.
    eventData popAndReturn()
    {
        std::unique_lock<std::mutex> lock(eventDatasMutex);

        eventData poppedData = eventDatas->front();
        eventDatas->pop();

        return poppedData;
    }
    bool isQueueEmpty()
    {
        std::unique_lock<std::mutex> lock(eventDatasMutex);
        bool output = eventDatas->empty();
        return output;
    }
    void waitForCondition()
    {
        std::unique_lock<std::mutex> lock(eventDatasMutex);
        cv.wait(lock, [this] { return data_ready; });
    }
};
