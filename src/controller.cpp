#include "controller.hpp"

#include <iostream>
#include <stdexcept>
Controller::Controller() {
  /* here we set watched directory to be /home/jesus/Documents/ASCII/ for
   * testing purposes. It should get this watched directories from config file
   * in close future */
  std::vector<std::filesystem::path> paths = {"/home/jesus/Documents/ASCII/"};
  initWatcherMask();
  eventWatcher watcher(watcherMask_, paths);
  try {
    watcher.startWatching();
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl;
  }
}
// Init watcherMask basically create an eventWatcher object with given
// directories and masks. Currently it just gives directories, I need to
// refactor it.

void Controller::initWatcherMask() {
  // fetch from config
  myMask.fromHandler();
  watcherMask_ = myMask.buildMask();
  std::cout << watcherMask_ << std::endl;
}
