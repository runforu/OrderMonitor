#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <regex>
#include <string>
#include "Synchronizer.h"

class ServerInfo {
public:
    std::string m_url;
    std::string m_scheme;
    std::string m_host;
    std::string m_port;
    std::string m_path;
};

class HttpPost {
public:
    static void StartPost();

    static void AddNotice(boost::property_tree::ptree& notice);

    static void SetUrl(std::string& url);

    inline static void stop();

private:
    static void RunPostLoop();

    static void Post(std::string& content);

private:
    static boost::asio::io_context s_io_context;
    static boost::property_tree::ptree s_notices;
    static Synchronizer s_synchronizer;
    static ServerInfo s_server_info;
    static bool s_stop_post;
};

#endif  // !_HTTPCLIENT_H_
