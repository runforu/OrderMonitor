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

HttpPost& HttpPost::Instance() {
    static HttpPost _instance;
    return _instance;
}

void HttpPost::StartPost() {
    try {
        // only allow one thread to post.
        if (InterlockedExchange(&m_running_thread, 1L) != 0) {
            return;
        }
        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&HttpPost::RunPostLoop, &HttpPost::Instance())));
    } catch (std::exception&) {
    }
}

void HttpPost::AddNotice(boost::property_tree::ptree& notice) {
    m_synchronizer.Lock();
    m_notices.push_back(std::make_pair("", notice));
    m_synchronizer.Unlock();
}

void HttpPost::SetUrl(const std::string& url) {
    FUNC_WARDER;

    m_synchronizer.Lock();
    m_server_info.m_url = url;
    LOG(url);
    std::regex ex("(((http)://)?)([^/ :]+):?([^/ ]*)(/?.*)");

    std::smatch what;
    if (!std::regex_match(url, what, ex)) {
        return;
    }

    m_server_info.m_scheme = what[3];
    m_server_info.m_host = what[4];
    m_server_info.m_port = what[5];
    m_server_info.m_path = what[6];

    if (m_server_info.m_scheme.empty()) {
        m_server_info.m_scheme = "http";
    }

    if (m_server_info.m_port.empty()) {
        m_server_info.m_port = "80";
    }

    if (m_server_info.m_path.empty()) {
        m_server_info.m_path = "/";
    }
    m_synchronizer.Unlock();
}

void HttpPost::stop() {
    FUNC_WARDER;
    m_stop_post = true;
}

void HttpPost::RunPostLoop() {
    FUNC_WARDER;

    while (true) {
        try {
            if (m_stop_post) {
                return;
            }
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
            if (m_stop_post) {
                return;
            }

            m_synchronizer.Lock();
            if (m_notices.size() == 0) {
                m_synchronizer.Unlock();
                continue;
            }
            boost::property_tree::ptree tree;
            tree.add_child("notices", m_notices);
            tree.put("count", m_notices.size());
            std::string content = JsonWrapper::ToJsonStr(tree);
            m_notices.clear();
            m_synchronizer.Unlock();
            Post(content);
        } catch (std::exception& e) {
            LOG_INFO(e.what());
        }
    }
}

void HttpPost::Post(std::string& content) {
    try {
        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::asio::ip::tcp::resolver::query query(m_server_info.m_host, m_server_info.m_port);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(query, ec);
        if (ec) {
            return;
        }

        // Try each endpoint until we successfully establish a connection.
        boost::asio::ip::tcp::socket socket(m_io_context);
        boost::asio::connect(socket, endpoints, ec);
        if (ec) {
            return;
        }

        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST " << m_server_info.m_path << " HTTP/1.1\r\n";
        request_stream << "Host: " << m_server_info.m_host << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << "Content-Length: " << content.length() << "\r\n";
        request_stream << "Content-Type: application/json\r\n\r\n";
        request_stream << content;
        boost::asio::write(socket, request, ec);
        if (ec) {
            socket.close();
            return;
        }

        boost::asio::streambuf response;
        static const std::string delimiter("\r\n");
        size_t n = boost::asio::read_until(socket, response, delimiter, ec);
        if (ec) {
            socket.close();
            return;
        }

        LOG("Notification response: %.*s", n - delimiter.length(), response.data());
        response.consume(n);
        socket.close();
    } catch (...) {
        LOG_LINE;
    }
}
