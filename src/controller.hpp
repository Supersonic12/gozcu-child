#include <cstdint>

#include "gatherer/eventWatcher.hpp"
#include "utility/configHandler.hpp"

/* This class is core of gozcu-child project
 * It should get watch Mask from config file
 * It should basically create an eventWatcher class object
 */
class Controller {
 public:
  Controller();
  // init watcherMask is for creating maskStruct object and build mask.
  void initWatcherMask();

 private:
  // this struct is for getting FANOTIFY_MARK event mask from config and build
  // it
  maskStruct myMask;
  // this variable is for saving result of building mask in maskStruct
  uint64_t watcherMask_ = 0;
};
