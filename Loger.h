#ifndef _LOGER_H_
#define _LOGER_H_

#include <string>
#include "common.h"
#include "Synchronizer.h"

#if defined(_RELEASE_LOG_) || defined(_DEBUG)

#define _CODE_ 14142
#define _IP_ "OrderMonitor"

struct RequestInfo;
struct TradeTransInfo;
struct UserInfo;
struct ConGroup;
struct ConSymbol;
struct TradeRecord;
struct TickAPI;

class Loger {
    static Synchronizer s_synchronizer;

public:
    static void out(const int code, const char* ip, std::string msg);
    static void out(const int code, const char* ip, int value);
    static void out(const int code, const char* ip, double value);
    static void out(const int code, const char* ip, const char* msg, ...);
    static void out(const int code, const char* ip, const RequestInfo* request);
    static void out(const int code, const char* ip, const TradeTransInfo* transaction);
    static void out(const int code, const char* ip, const UserInfo* user_info);
    static void out(const int code, const char* ip, const ConGroup* con_group);
    static void out(const int code, const char* ip, const ConSymbol* con_symbol);
    static void out(const int code, const char* ip, const TradeRecord* trade_record);
    static void out(const int code, const char* ip, const TickAPI* tick);
};

class FuncWarder {
    char m_function_name[32];

public:
    FuncWarder(const char* name) {
        COPY_STR(m_function_name, name);
        Loger::out(_CODE_, _IP_, "------------------------------>>>     %s", m_function_name);
    }
    ~FuncWarder() {
        Loger::out(_CODE_, _IP_, "<<<------------------------------     %s", m_function_name);
    }
};

#define LOG(format, ...) Loger::out(_CODE_, _IP_, format, ##__VA_ARGS__);
#define LOG_INFO(info) Loger::out(_CODE_, _IP_, info);
#define LOG_LINE Loger::out(_CODE_, _IP_, "hit func =%s, line = %d ", __FUNCTION__, __LINE__), LOG("%1024s", " ");

#define FUNC_WARDER FuncWarder $INVISIBLE(__FUNCTION__);

#else _RELEASE_LOG_

#define LOG(format, ...)
#define LOG_INFO(inf)
#define LOG_LINE
#define FUNC_WARDER

#endif  //_RELEASE_LOG_

#endif  // !_LOGER_H_
