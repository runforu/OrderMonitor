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
    static HttpPost& Instance();

    void StartPost();

    void AddNotice(boost::property_tree::ptree& notice);

    void SetUrl(const std::string& url);

    void stop();

private:
    HttpPost() : m_stop_post(false), m_running_thread(0){};

    HttpPost(const HttpPost& post){};

    void operator=(const HttpPost& post){};

    ~HttpPost(){};

    void RunPostLoop();

    void Post(std::string& content);

private:
    boost::asio::io_context m_io_context;
    boost::property_tree::ptree m_notices;
    Synchronizer m_synchronizer;
    ServerInfo m_server_info;
    bool m_stop_post;
    long m_running_thread;
};

#endif  // !_HTTPCLIENT_H_
