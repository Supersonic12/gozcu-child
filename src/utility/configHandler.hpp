#pragma once

#include <linux/fanotify.h>
#include <sys/fanotify.h>

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "serverInfo.hpp"
using json = nlohmann::json;
class ConfigHandler
{
   private:
    // this will be loaded from config file
    json configData_;
    // defaulting to config path
    std::filesystem::path getConfigPath();
    template <typename T>
    T get(const std::string& key)
    {
        if (configData_.contains(key))
        {
            return configData_.at(key).get<T>();
        }
        else
        {
            throw std::runtime_error(std::string("Non-existent key"));
        }
    };
    template <typename T>
    void set(const std::string& key, T& value)
    {
        configData_[key] = value;
        writeToFile();
    }

   public:
    ConfigHandler();
    // This will read json object from file and save it to runtime json object
    void loadFile();
    // this will take private json object and write it to config file
    void writeToFile();

    std::map<std::string, bool> getMaskMap();
    ServerInfo getServerInfo();

    void saveMaskSettings(std::map<std::string, bool>& maskMap);
    void saveServerInfoSettings(ServerInfo& serverInfo);
};
