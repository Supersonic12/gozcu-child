#include "configHandler.hpp"

#include <linux/fanotify.h>
#include <sys/types.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void ConfigHandler::getConfigPath()
{
    std::filesystem::path configPath;
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    const char* home = std::getenv("HOME");
    if (xdgConfig)
    {
        configPath = std::filesystem::path(xdgConfig) / "gozcu";
    }
    else
    {
        configPath = std::filesystem::path(home) / ".config" / "gozcu";
    }

    configPath_ = configPath;
}

void ConfigHandler::setMask(struct watcherMask& maskStruct)
{
    std::ofstream(tempPath_);
    // maskStruct.
}
uint64_t ConfigHandler::getMask() { std::ifstream(configPath_); }
