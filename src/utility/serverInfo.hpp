#pragma once
#include <nlohmann/json.hpp>
#include <string>
struct ServerInfo
{
    std::string ipAddress = "";
    std::string portAddress = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerInfo, ipAddress, portAddress);
