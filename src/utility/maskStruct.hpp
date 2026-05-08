#include <sys/fanotify.h>

#include <cstdint>
#include <map>
#include <string>
struct maskStruct
{
    std::string something;
    // this will be used for getting values af all masks with bitwise or
    std::map<std::string, uint64_t> maskValues = {
        {"fan_mod", FAN_MODIFY},         {"fan_cl_wr", FAN_CLOSE_WRITE},
        {"fan_acc", FAN_ACCESS},         {"fan_op", FAN_OPEN},
        {"fan_attr", FAN_ATTRIB},        {"fan_crt", FAN_CREATE},
        {"fan_del", FAN_DELETE},         {"fan_del_self", FAN_DELETE_SELF},
        {"fan_mv_from", FAN_MOVED_FROM}, {"fan_mv_to", FAN_MOVED_TO},
        {"fan_mv_self", FAN_MOVE_SELF},  {"fan_ev_on_child", FAN_EVENT_ON_CHILD}};
    // this one will be used for saving data of masks in another struct
    std::map<std::string, bool> stateOfMasks = {
        {"fan_mod", false},   {"fan_cl_wr", false},    {"fan_acc", false},
        {"fan_op", false},    {"fan_attr", false},     {"fan_crt", false},
        {"fan_del", false},   {"fan_del_self", false}, {"fan_mv_from", false},
        {"fan_mv_to", false}, {"fan_mv_self", false},  {"fan_ev_on_child", false}};
};
