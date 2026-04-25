#pragma once
#include <linux/fanotify.h>
#include <sys/fanotify.h>
#include <sys/types.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

/*
 *  watcherMask struct is a mask struct that should be read from config file and then passed to
 * event watcher. It server no purpose other than what kind of notification are wanted by user. It
 * has 2 builtin functions readmask WriteMask which basically serve as the name suggests.
 */

// FAN_MODIFY | FAN_CLOSE_WRITE | FAN_ACCESS | FAN_OPEN | FAN_ATTRIB |
// FAN_CREATE | FAN_DELETE | FAN_DELETE_SELF | FAN_MOVED_FROM | FAN_MOVED_TO |
// FAN_MOVE_SELF | FAN_EVENT_ON_CHILD,
using json = nlohmann::json;
struct watcherMask
{
    std::vector<std::pair<std::string, uint64_t>> flagMap = {
        {"fan_mod", FAN_MODIFY},         {"fan_cl_wr", FAN_CLOSE_WRITE},
        {"fan_acc", FAN_ACCESS},         {"fan_op", FAN_OPEN},
        {"fan_attr", FAN_ATTRIB},        {"fan_crt", FAN_CREATE},
        {"fan_del", FAN_DELETE},         {"fan_del_self", FAN_DELETE_SELF},
        {"fan_mv_from", FAN_MOVED_FROM}, {"fan_mv_to", FAN_MOVED_TO},
        {"fan_mv_self", FAN_MOVE_SELF},  {"fan_ev_on_child", FAN_EVENT_ON_CHILD}};
    uint64_t finalMask;
    /*
     * readMask gets each key from json file and sets them on struct as separately and as final mask
     * format basically it saves them
     */
    void readMask(const json& j)
    {
        finalMask = 0;
        for (const auto [key, flag] : flagMap)
        {
            if (j.value(key, false) == true)
            {
                finalMask |= flag;
            }
        }
    }
    /*
     * So this function just creates a json object to write to file. I don't know how to handle it,
     * Should it directly write to file or just create a json object I guess I can later decide
     */
    json writeMask()
    {
        json j;
        for (const auto [key, flag] : flagMap)
        {
            // if (j.contains(key) && j[key].is_boolean()) {
            j[key] = (finalMask & flag) ? true : false;
            //}
        }
        return j;
    }
};

/*
 * This class will be responsible for config related operations such as reading from config and
 * writing back to config.
 */
class ConfigHandler
{
   public:
    uint64_t getMask();
    void setMask(struct watcherMask& maskStruct);
    void getConfigPath();

   private:
    // uint64_t defaultmask = fan_mod | fan_cl_wr | fan_acc | fan_op | fan_attr |
    // fan_crt | fan_del
    // |
    // fan_del_self | fan_mv_from | fan_mv_to | fan_mv_self | fan_ev_on_child;
    std::filesystem::path configPath_;
    watcherMask strMask_;
};
