#include <process.h>
#include <stdio.h>
#include "Loger.h"
#include "Processor.h"
#include "ServerApi.h"

void Processor::Shutdown(void) {
    ShowStatus();
}

Processor::Processor() : m_reinitialize_flag(0), m_disable_plugin(0) {
    ZeroMemory(&m_manager, sizeof(m_manager));
    m_manager.login = 14142;
    COPY_STR(m_manager.name, "Order Monitor");
    COPY_STR(m_manager.ip, "OrderMonitor");
}

Processor& Processor::Instance() {
    static Processor _instance;
    return _instance;
}

void Processor::ShowStatus() {
    LOG("OrderMonitor is going to shutdown.");
}

void Processor::Initialize() {
    FUNC_WARDER;

    Config::Instance().GetInteger("Disable Plugin", &m_disable_plugin, "0");
    Config::Instance().GetString("Server", m_notice_server, sizeof(m_notice_server), "");
}

void Processor::OrderUpdated(TradeRecord* trade, UserInfo* user, const int mode) {
    FUNC_WARDER;

    LOG_INFO(trade);
    LOG_INFO(mode);

    //--- reinitialize if configuration changed
    if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
        Initialize();
    }

    if (m_disable_plugin) {
        return ;
    }

}

void Processor::OrderAdded(TradeRecord* trade, const UserInfo* user, const ConSymbol* symbol, const int mode) {
    FUNC_WARDER;

    LOG_INFO(trade);
    LOG_INFO(mode);

    //--- reinitialize if configuration changed
    if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
        Initialize();
    }

    if (m_disable_plugin) {
        return;
    }
}

void Processor::OrderClosedBy(TradeRecord* ftrade, TradeRecord* strade, TradeRecord* remaind, ConSymbol* sec, UserInfo* user) {
    FUNC_WARDER;

    LOG_INFO(ftrade);
    LOG_INFO(strade);

    //--- reinitialize if configuration changed
    if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
        Initialize();
    }

    if (m_disable_plugin) {
        return;
    }
}
