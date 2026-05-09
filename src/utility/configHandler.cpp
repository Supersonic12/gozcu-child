#include "configHandler.hpp"

#include <linux/fanotify.h>
#include <sys/types.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
using json = nlohmann::json;
ConfigHandler::ConfigHandler() {
  getConfigPath();
  loadFile();
}
// this is getting config path with already existent variables
// simply uses HOME or XDG_CONFIG_HOME variables on system to create a derived
// ~/.config/gozcu folder
std::filesystem::path ConfigHandler::getConfigPath() {
  // std::filesystem::path configPath;

  // well apparently I can't use these 2 because my application is used with
  // fucking root. which changes output of $HOME to /root and $XDG_CONFIG_HOME
  // is not reliable always. I guess I'm just gonna use /etc/gozcu/gozcu.json as
  // test subject
  // const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
  // const char* home = std::getenv("HOME");
  std::filesystem::path genericConfig = "/etc/gozcu";
  // if (xdgConfig!=nullptr)
  // {
  //     configPath = std::filesystem::path(xdgConfig) / "gozcu";
  // }
  // else
  // {
  // configPath = std::filesystem::path(home) / ".config" / "gozcu";
  // }

  return genericConfig;
}

void ConfigHandler::loadFile() {
  if (!std::filesystem::exists(getConfigPath())) {
    std::cerr << "config directory doesn't exists." << getConfigPath()
              << std::endl;
  }
  std::ifstream ifs(getConfigPath() / "gozcu.json");
  if (!ifs.is_open()) {
    throw std::runtime_error(std::string("failed to open config file"));
  }
  try {
    configData_ = json::parse(ifs);
    // std::cout << configData_.dump(4) << std::endl;
  } catch (json::parse_error& e) {
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
void ConfigHandler::writeToFile() {
  std::filesystem::path tempPath = getConfigPath() / "gozcu.json.tmp";
  std::filesystem::path configPath = getConfigPath() / "gozcu.json";
  std::filesystem::path backupConfigPath = getConfigPath() / "gozcu.json.bak";
  std::ofstream ofs(getConfigPath() / "gozcu.json.tmp");
  if (!ofs.is_open()) {
    throw std::runtime_error(
        "failed to open temporary file to write to config file");
  }
  // TODO: Look writing to ofs with nlohmann/json.hpp DONE!
  ofs << configData_.dump(4) << std::endl;
  ofs.close();
  // rename existent config to .bak file
  std::filesystem::rename(configPath, backupConfigPath);
  // rename temporary file to config file
  std::filesystem::rename(tempPath, configPath);
}
