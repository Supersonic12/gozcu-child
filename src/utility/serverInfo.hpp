#pragma once
#include <nlohmann/json.hpp>
#include <string>
struct serverInfo
{
    std::string ipAddress = "";
    int portAddress = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(serverInfo, ipAddress, portAddress);
