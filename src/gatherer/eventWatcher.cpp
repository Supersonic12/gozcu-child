#include "eventWatcher.hpp"

#include <fcntl.h>
#include <limits.h>
#include <linux/fanotify.h>
#include <linux/limits.h>
#include <poll.h>
#include <sys/fanotify.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "../eventData.hpp"
#include "../utility/raiiguardfd.hpp"
void eventWatcher::startWatching() {
  // check if watched paths exists
  if (watchedPaths_.empty()) {
    std::cerr << "ERROR: there is not any path to watch" << std::endl;
    throw std::runtime_error("watchedPaths_ empty");
  }
  if (watchMask_ == 0) {
    std::cerr << "ERROR: watch mask isn't set";
    throw std::runtime_error("watchMask_ empty");
  }
  /*
   * Create a fd that is initialized by fanotify_init
   * Well it is currently just for notification purposes
   * It doesnt interfere with processes file access. It gives us Which file is
   * affected and event data in fanotify_event_metadata struct
   */
  int fd = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_DIR_FID |
                             FAN_REPORT_NAME | FAN_REPORT_FID | FAN_NONBLOCK,
                         O_RDONLY | O_LARGEFILE | O_NONBLOCK);
  if (fd == -1) {
    perror("FANOTIFY_INIT");
    throw std::runtime_error("FANOTIFY_INIT");
  }
  // Basically one is for stdin
  //  other is for our fanotify events.
  //  though i will need to remove this pollfd struct. It basically serves no
  //  purpose since this was supposed to be an agent like application no cli
  //  based one
  nfds_t nfds = 2;
  struct pollfd fds[2];
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;
  fds[1].fd = fd;
  fds[1].events = POLLIN;
  // Don't ever ever delete O_NONBLOCK inside root_fd
  int root_fd = open("/", O_RDONLY | O_DIRECTORY | O_NONBLOCK);
  fdguard mountguard(root_fd);
  fdguard stdinguard(fds[0].fd);
  fdguard fanguard(fds[1].fd);
  char buffer;
  int poll_num = 0;
  for (const auto entry : watchedPaths_) {
    std::cout << "Watching: " << entry << std::endl;
    // I will need to add options to ui to be able to set
    /*
     * this basically sets which events should be recorded and i have basically
     * set all notify events as mask this mask is needed as argument of
     * startWatching function
     */
    int mark = fanotify_mark(
        fd, FAN_MARK_ADD,
        /*FAN_MODIFY | FAN_CLOSE_WRITE | FAN_ACCESS | FAN_OPEN | FAN_ATTRIB
           | FAN_CREATE | FAN_DELETE | FAN_DELETE_SELF | FAN_MOVED_FROM |
            FAN_MOVED_TO | FAN_MOVE_SELF | FAN_EVENT_ON_CHILD*/
        watchMask_, AT_FDCWD, entry.c_str());
    if (mark == -1) {
      perror("FANOTIFY_MARK");
      throw std::runtime_error("FANOTIFY_MARK");
    }
  }
  /*
   * event loop of watcher. it basically never stops until enter pressed or
   * fanotify error happened it sets fanotify_event_metadata struct and iterates
   * in it to get needed data about event..
   */
  while (1) {
    poll_num = poll(fds, nfds, -1);
    if (poll_num == -1) {
      if (errno == EINTR) {
        continue;
      }
      perror("POLL");
      throw std::runtime_error(std::string("POLL"));
    }
    if (poll_num > 0) {
      if (fds[0].revents & POLLIN) {
        while (read(STDIN_FILENO, &buffer, 1) > 0 && buffer != '\n') {
          continue;
        }
        std::cout << "ENTER pressed\n";
        break;
      }
      if (fds[1].revents & POLLIN) {
        char buf[4096];
        char* ptr = buf;
        ssize_t remainingBufLen = read(fds[1].fd, buf, sizeof(buf));
        if (remainingBufLen < 0) {
          perror("READ");
          break;
        }
        /*
         * the fanotify_event_metadata struct is where we store all needed data.
         */
        struct fanotify_event_metadata* metadata =
            (struct fanotify_event_metadata*)ptr;
        if (metadata->vers != FANOTIFY_METADATA_VERSION) {
          break;
        }
        /*
         * iterating through metadata until there isn't any event left
         * here we set event structs variables. though I don't really understand
         * this code.
         */
        while (FAN_EVENT_OK(metadata, remainingBufLen)) {
          eventData event;
          event.fd = fd;
          event.mask = metadata->mask;
          event.pid = metadata->pid;
          /*
           * infoHeader struct is for the struct nested in metadata struct
           */
          struct fanotify_event_info_header* infoHeader =
              (struct fanotify_event_info_header*)(metadata + 1);
          char* event_end = (char*)metadata + metadata->event_len;
          char dir_path[PATH_MAX] = "unknown";
          const char* filename = NULL;

          while ((char*)infoHeader < event_end) {
            if (((char*)infoHeader) +
                    sizeof(struct fanotify_event_info_header) >
                event_end)
              break;
            if (infoHeader->info_type == FAN_EVENT_INFO_TYPE_DFID_NAME) {
              if ((char*)infoHeader + infoHeader->len > event_end) break;
              /*
               * and info_fid struct is nested in infoHeaderso we just create a
               * fid struct and take file handle from it to get affected files
               * path.
               */
              struct fanotify_event_info_fid* fid =
                  (struct fanotify_event_info_fid*)infoHeader;
              struct file_handle* fh = (struct file_handle*)fid->handle;
              // here setEventData takes fanotify_init file descriptor, roots
              // file descriptor, info_fid struct's file_handle and already
              // setted event structure.
              setEventData(fd, root_fd, fh, event);
              filename = (const char*)fh->f_handle + fh->handle_bytes;
              event.filename = std::string(filename);
            }
            if (infoHeader->len == 0) break;
            infoHeader =
                (struct fanotify_event_info_header*)((char*)infoHeader +
                                                     infoHeader->len);
          }
          std::cout << "filename: " << event.filename << std::endl;
          std::cout << "path: " << event.path << std::endl;
          std::cout << "fd: " << event.fd << std::endl;
          std::cout << "mask: " << event.mask << std::endl;
          std::cout << "pid: " << event.pid << std::endl;
          metadata = FAN_EVENT_NEXT(metadata, remainingBufLen);
        }
      }
    }
  }
}

bool eventWatcher::setEventData(int fd, int mount_fd, struct file_handle* fh,
                                eventData& event) {
  // this fd is for reopening another file descriptor to affected file and the
  // reading its path using readlink and fd file in /proc/self/fd. This only
  // works on linux if i ever think of using this kind of mechanism in another
  // OS I will need to reimplement startWatching and setEventData completely. (i
  // guess that means eventWatcher class completely.)
  int refd = open_by_handle_at(mount_fd, fh, O_RDONLY | O_PATH | O_NONBLOCK);
  // i have written and fdguard which basically closes fd when it isn't used or
  // out of block
  fdguard reFDguard(refd);
  if (refd < 0) {
    perror("OPEN_BY_HANDLE_AT");
    return false;
  }

  std::string fdpath = "/proc/self/fd/" + std::to_string(refd);
  char buf[4096];
  ssize_t retLen = readlink(fdpath.c_str(), buf, sizeof(buf));
  if (retLen < 0) {
    perror("READLINK FAIL");
    throw std::runtime_error("READLINK FAIL");
    return false;
  }
  buf[retLen] = '\0';
  event.path = buf;
  return true;
};
