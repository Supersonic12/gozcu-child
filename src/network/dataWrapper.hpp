#pragma once
#include <nlohmann/json.hpp>

#include "../eventData.hpp"
using json = nlohmann::json;
// this is for converting eventData struct to json format and then it shall return it to
// connectionHandler class
// I guess i don't need a class here so i will change this into helper function;
inline json wrapData(const eventData& data)
{
    json j;
    j["pid"] = data.pid;
    j["timestamp"] = std::chrono::system_clock::to_time_t(data.dateTime);
    j["pathTo"] = data.path;
    j["fileName"] = data.filename;
    j["mask"] = data.mask;

    return j;
};
