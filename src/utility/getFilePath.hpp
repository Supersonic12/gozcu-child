#pragma once
#include <filesystem>

#include "../eventData.hpp"
inline std::filesystem::path getFullPath(eventData& data)
{
    return std::filesystem::path(data.path) / std::filesystem::path(data.filename);
};
