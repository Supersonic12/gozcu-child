#include "eventWatcher.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <sys/fanotify.h>
void eventWatcher::startWatching() {
  // check if watched paths exists
  if (watchedPaths_.empty()) {
    std::cerr << "ERROR: there is not any path to watch" << std::endl;

    int fd = fanotify_init(FAN_CLASS_CONTENT | FAN_NONBLOCK,
                           O_RDONLY | O_LARGEFILE | O_NONBLOCK);
    if (fd == -1) {
      std::cerr << std::strerror(errno) << std::endl;
      return;
    }
    for (const auto entry : watchedPaths_) {
      // I will need to add options to ui to be able to set
      int mark = fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                               FAN_ACCESS | FAN_MODIFY | FAN_CLOSE_WRITE |
                                   FAN_OPEN | FAN_OPEN_EXEC | FAN_ATTRIB |
                                   FAN_CREATE | FAN_DELETE | FAN_DELETE_SELF |
                                   FAN_MOVE | FAN_RENAME | FAN_MOVE_SELF,
                               AT_FDCWD, entry.c_str());
    }
  }
}
