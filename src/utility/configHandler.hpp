#pragma once

#include <linux/fanotify.h>
#include <sys/fanotify.h>

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;
class ConfigHandler {
 private:
  // this will be loaded from config file
  json configData_;
  // defaulting to config path
  std::filesystem::path getConfigPath();

 public:
  ConfigHandler();
  // This will read json object from file and save it to runtime json object
  void loadFile();
  // this will take private json object and write it to config file
  void writeToFile();

  template <typename T>
  T get(const std::string& key) {
    if (configData_.contains(key)) {
      return configData_.at(key).get<T>();
    } else {
      throw std::runtime_error(std::string("Non-existent key"));
    }
  };
  template <typename T>
  void set(const std::string& key, T& value) {
    configData_[key] = value;
    writeToFile();
  }
};
// This struct should get a handler object inside and according to that handlers
// json data it should get its own value key pair and behave accordingly. This
// can be implemented with either virtual interface and derived classes or
// struct and template, claude said it would be better to implement it with
// struct and this way I will need to create each of their own template type and
// will add new struct if new kind of config is added to file
struct maskStruct {
  // Mask struct creates a handler object. fromHandler is for fetching specific
  // data type from config file. In our case it is for fetching map of mask bit
  // values. toHandler basically is just setting new value to specific key. In
  // our case a map object to markMask key.
  // TODO: build function for creating final mask from mask components.
  ConfigHandler handler;
  std::map<std::string, bool> maskStateMap;
  void fromHandler() {
    const std::string maskKey = "markMask";
    maskStateMap = handler.get<std::map<std::string, bool>>(maskKey);
    // Here it says me expected expression, I don't have any idea what should I
    // write there Fixed!
  }
  void toHandler(const std::string& key, std::map<std::string, bool>& maskMap) {
    handler.set<std::map<std::string, bool>>(key, maskMap);
  }
  uint64_t buildMask() {
    uint64_t builtMask = 0;
    std::map<std::string, uint64_t> allMasks = {
        {"use_fan_del", FAN_DELETE},
        {"use_fan_mod", FAN_MODIFY},
        {"use_fan_cl_wr", FAN_CLOSE_WRITE},
        {"use_fan_acc", FAN_ACCESS},
        {"use_fan_attr", FAN_ATTRIB},
        {"use_fan_del_self", FAN_DELETE_SELF},
        {"use_fan_mv_from", FAN_MOVED_FROM},
        {"use_fan_mv_to", FAN_MOVED_TO},
        {"use_fan_mv_self", FAN_MOVE_SELF},
        {"use_fan_op", FAN_OPEN},
        {"use_fan_ev_on_child", FAN_EVENT_ON_CHILD},
        {"use_fan_crt", FAN_CREATE},
    };
    // uint64_t test_all_mask_enabled = 0;
    // for (const auto [key, value] : allMasks) {
    //   test_all_mask_enabled |= value;
    // }
    // std::cout << test_all_mask_enabled << std::endl;
    for (const auto [key, enabled] : maskStateMap)

    {
      if (enabled) {
        auto it = allMasks.find(key);
        if (it != allMasks.end()) {
          builtMask |= it->second;
        }
      }
    }
    return builtMask;
  }
};
// NOTE: I don't have any idea how should I implement a write to config feature
// with struct way. I mean struct can use template and derived function again
// but seriously how will it know which file it should write to. Oh I have an
// idea maybe this would be correct way to do it
