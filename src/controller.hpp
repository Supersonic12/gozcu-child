#include <cstdint>

#include "gatherer/eventWatcher.hpp"
#include "utility/configHandler.hpp"

/* This class is core of gozcu-child project
 * It should get watch Mask from config file
 * It should basically create an eventWatcher class object
 */
class Controller
{
   public:
    Controller();
    void initWatcherMask();

   private:
    maskStruct myMask;
    uint64_t watcherMask_ = 0;
};
