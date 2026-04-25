#pragma once
#include <sys/fanotify.h>

#include <cstdint>
#include <ctime>
#include <filesystem>
#include <string>
/*
 * This struct is okay but later It will need some additional variables.
 */
struct eventData
{
    int pid;
    int fd;
    uint64_t mask;
    std::filesystem::path path;
    std::string filename;
};
/*
 * messageData isn't currently used it needs better handling
 */
struct messageData
{
    int pid;
    time_t timeStamp;
    std::filesystem::path filePath;
    std::string processName;
};
