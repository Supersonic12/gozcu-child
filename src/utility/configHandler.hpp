#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
class ConfigHandler
{
   private:
    // this will be loaded from config file
    json configData;
    // defaulting to config path
    std::filesystem::path getConfigPath();

   public:
    // This will read json object from file and save it to runtime json object
    void loadFile();
    // this will take private json object and write it to config file
    void writeToFile();

    template <typename T>
    T get(const std::string& key)
    {
        if (configData.contains(key))
        {
            return configData.at(key).get<T>();
        }
        else
        {
            throw std::runtime_error(std::string("Non-existent key"));
        }
    };
    template <typename T>
    void set(const std::string& key, T& value)
    {
        configData[key] = value;
        writeToFile();
    }
};
// This struct should get a handler object inside and according to that handlers json data it should
// get its own value key pair and behave accordingly. This can be implemented with either virtual
// interface and derived classes or struct and template, claude said it would be better to implement
// it with struct and this way I will need to create each of their own template type and will add
// new struct if new kind of config is added to file
struct maskStruct
{
    // Mask struct creates a handler object. fromHandler is for fetching specific data type from
    // config file. In our case it is for fetching map of mask bit values. toHandler basically is
    // just setting new value to specific key. In our case a map object to markMask key.
    // TODO: build function for creating final mask from mask components.
    ConfigHandler handler;
    void fromHandler()
    {
        const std::string maskKey = "markMask";
        std::map maskStateMap = handler.get<std::map<std::string, bool>>(maskKey);
        // Here it says me expected expression, I don't have any idea what should I write there
        // Fixed!
    }
    void toHandler(const std::string& key, std::map<std::string, bool>& maskMap)
    {
        handler.set<std::map<std::string, bool>>(key, maskMap);
    }
};
// NOTE: I don't have any idea how should I implement a write to config feature with struct way. I
// mean struct can use template and derived function again but seriously how will it know which file
// it should write to. Oh I have an idea maybe this would be correct way to do it
