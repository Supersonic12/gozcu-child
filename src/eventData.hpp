#pragma once
#include <sys/fanotify.h>

#include <chrono>
#include <cstdint>
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
    std::chrono::system_clock::time_point dateTime;
    std::string fileHash;
    bool checkHash = false;
};
