#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "../eventData.hpp"
#include "../network/queueHandler.hpp"

/* this class is for watching real time kernel events while they are happening
 * it is based on fanotify calls so we can also stop or kill unwanted processes
 */
class eventWatcher
{
   public:
    eventWatcher(uint64_t watcherMask, std::vector<std::filesystem::path> paths,
                 std::shared_ptr<queueHandler>& outgoingQueue)
        : watchedPaths_(paths), watchMask_(watcherMask), outgoingQueue_(outgoingQueue)
    {
        if (watchedPaths_.empty())
        {
            std::cerr << "ERROR: either no path is specified or watched paths failed "
                         "to pass to eventWatcher"
                      << std::endl;
        }
    }
    // start watching is basically main function of this class
    // normally I should change its name to init something
    void startWatching();
    // setEventData is internal function which basically sets real path of
    // affected file. It just uses readlink to get file path and then sets it on
    // the given eventData struct. Everything else is set on startWatching. I need
    // to refactor this class, It isn't optimal.
    bool setEventData(int fd, int mount_fd, struct file_handle* fh, eventData& event);

   private:
    std::vector<std::filesystem::path> watchedPaths_;
    uint64_t watchMask_;
    std::shared_ptr<queueHandler> outgoingQueue_;
};
