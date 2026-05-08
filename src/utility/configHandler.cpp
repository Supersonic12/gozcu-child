#include "configHandler.hpp"

#include <linux/fanotify.h>
#include <sys/types.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
using json = nlohmann::json;
ConfigHandler::ConfigHandler()
{
    getConfigPath();
    readConfigFile();
}
// this is getting config path with already existent variables
// simply uses HOME or XDG_CONFIG_HOME variables on system to create a derived ~/.config/gozcu
// folder
std::filesystem::path ConfigHandler::getConfigPath()
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

    return configPath;
}
// under construction
// setMask should use maskStruct's built-in writeMask function and set json objects key and values
// accordingly and then it should call writeToConfigFile and give json object to it
void ConfigHandler::setMask(struct watcherMask& maskStruct)
{
    maskStruct.writeMask(configData_);
    /*
     * This should take a mask struct and use that struct built-in writeMask function and
     */
}

/*
 * This function parses already existent mask from configData_
 * TODO: watcherMask.readMask needs refactoring
 */
uint64_t ConfigHandler::getMask()
{
    watcherMask maskStruct;
    maskStruct.readMask(configData_);
    return maskStruct.finalMask;
}
/*
 * This reads from config file and saves json data to configData_ object;
 * TODO: better exception handling including json parsing process
 */
void ConfigHandler::readConfigFile()
{
    if (!std::filesystem::exists(getConfigPath()))
    {
        throw std::runtime_error("config directory doesn't exists.");
    }
    std::ifstream ifs(getConfigPath() / "gozcu.json");
    if (!ifs.is_open())
    {
        throw std::runtime_error(std::string("failed to open config file"));
    }
    try
    {
        json configData_ = json::parse(ifs);
    }
    catch (json::parse_error& e)
    {
        std::cerr << "failed to parse config" << e.what();
        throw std::runtime_error("Parse error");
    }
    ifs.close();
}
/*
 * This function takes json object to write to config file
 * and then writes it to tempPath_ file
 * and renames tempPath_ file to configPath_
 */
void ConfigHandler::writeToConfigFile(json& j)
{
    std::filesystem::path tempPath = getConfigPath() / "gozcu.json.tmp";
    std::filesystem::path configPath = getConfigPath() / "gozcu.json";
    std::filesystem::path backupConfigPath = getConfigPath() / "gozcu.json.bak";
    std::ofstream ofs(getConfigPath() / "gozcu.json.tmp");
    if (!ofs.is_open())
    {
        throw std::runtime_error("failed to open temporary file to write to config file");
    }
    // TODO: Look writing to ofs with nlohmann/json.hpp DONE!
    ofs << j.dump(4) << std::endl;
    ofs.close();
    // rename existent config to .bak file
    std::filesystem::rename(configPath, backupConfigPath);
    // rename temporary file to config file
    std::filesystem::rename(tempPath, configPath);
}
