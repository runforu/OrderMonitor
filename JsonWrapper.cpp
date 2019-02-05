#include "JsonWrapper.h"
#include "Loger.h"

boost::property_tree::ptree JsonWrapper::ParseJson(std::string json_str) {
    boost::property_tree::ptree tree;
    std::stringstream ss(json_str);
    try {
        read_json(ss, tree);
    } catch (boost::property_tree::ptree_error& e) {
        LOG("read_json -- exception: %s", e.what());
    }
    return tree;
}

std::string JsonWrapper::ToJsonStr(boost::property_tree::ptree& pt, bool pretty) {
    std::stringstream ss;
    write_json(ss, pt, pretty);
    return ss.str();
}
