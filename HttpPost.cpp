#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <regex>
#include <stdlib.h>
#include <string>
#include "HttpPost.h"
#include "JsonWrapper.h"
#include "Loger.h"

boost::asio::io_context HttpPost::s_io_context;
boost::property_tree::ptree HttpPost::s_notices;
Synchronizer HttpPost::s_synchronizer;
ServerInfo HttpPost::s_server_info;
bool HttpPost::s_stop_post = false;

void HttpPost::StartPost() {
    try {
        boost::shared_ptr<boost::thread> thread(new boost::thread(HttpPost::RunPostLoop));
    } catch (std::exception& e) {
    }
}

void HttpPost::AddNotice(boost::property_tree::ptree& notice) {
    s_synchronizer.Lock();
    s_notices.push_back(std::make_pair("", notice));
    s_synchronizer.Unlock();
}

void HttpPost::SetUrl(std::string& url) {
    FUNC_WARDER;

    s_synchronizer.Lock();
    s_server_info.m_url = url;
    LOG(url);
    std::regex ex("(((http)://)?)([^/ :]+):?([^/ ]*)(/?.*)");

    std::smatch what;
    if (!std::regex_match(url, what, ex)) {
        return;
    }

    s_server_info.m_scheme = what[3];
    s_server_info.m_host = what[4];
    s_server_info.m_port = what[5];
    s_server_info.m_path = what[6];

    if (s_server_info.m_scheme.empty()) {
        s_server_info.m_scheme = "http";
    }

    if (s_server_info.m_port.empty()) {
        s_server_info.m_port = "80";
    }

    if (s_server_info.m_path.empty()) {
        s_server_info.m_path = "/";
    }
    s_synchronizer.Unlock();
}

inline void HttpPost::stop() {
    FUNC_WARDER;
    s_stop_post = true;
}

void HttpPost::RunPostLoop() {
    FUNC_WARDER;

    while (true) {
        try {
            if (s_stop_post) {
                return;
            }
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
            if (s_stop_post) {
                return;
            }

            s_synchronizer.Lock();
            boost::property_tree::ptree tree;
            tree.add_child("notices", s_notices);
            tree.put("count", s_notices.size());
            if (s_notices.size() == 0) {
                s_synchronizer.Unlock();
                continue;
            }
            std::string content = JsonWrapper::ToJsonStr(tree);
            s_notices.clear();
            s_synchronizer.Unlock();
            Post(content);
        } catch (std::exception& e) {
            LOG_INFO(e.what());
        }
    }
}

void HttpPost::Post(std::string& content) {
    boost::asio::ip::tcp::resolver resolver(s_io_context);
    boost::asio::ip::tcp::resolver::query query(s_server_info.m_host, s_server_info.m_port);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(s_io_context);
    boost::asio::connect(socket, endpoints);

    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << s_server_info.m_path << " HTTP/1.1\r\n";
    request_stream << "Host: " << s_server_info.m_host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n";
    request_stream << "Content-Length: " << content.length() << "\r\n";
    request_stream << "Content-Type: application/json\r\n\r\n";

    request_stream << content;
    boost::asio::write(socket, request);

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        LOG("Invalid response");
        // continue;
    }
    if (status_code != 200) {
        LOG("Response returned with status code %d", status_code);
        // continue;
    }

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
    }

    LOG("Post time = %d", time(NULL));
}
