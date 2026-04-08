#include <filesystem>
#include <iostream>
#include <vector>
class eventWatcher {
public:
  eventWatcher(std::vector<std::filesystem::path> paths) : watchedPaths(paths) {
    if (watchedPaths.empty()) {
      std::cerr << "ERROR: either no path is specified or watched paths failed "
                   "to pass to eventWatcher"
                << std::endl;
    }
  }
  void startWatching();

private:
  std::vector<std::filesystem::path> watchedPaths_;
};
