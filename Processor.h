#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "Config.h"
#include "common.h"
#include "LicenseService.h"

class Processor {
private:
    //--- configurations
    int m_disable_plugin;
    char m_notice_server[256];
    LONG m_reinitialize_flag;

    // Synchronizer m_sync;

public:
    static Processor& Instance();

    inline void Reinitialize() {
        InterlockedExchange(&m_reinitialize_flag, 1);
    }

    void ShowStatus();

    void OrderUpdated(TradeRecord* trade, UserInfo* user, const int mode);
    void OrderAdded(TradeRecord* trade, const UserInfo* user, const ConSymbol* symbol, const int mode);
    void OrderClosedBy(TradeRecord* ftrade, TradeRecord* strade, TradeRecord* remaind, ConSymbol* sec, UserInfo* user);

    void Initialize();

    void Shutdown(void);

private:
    Processor();
    Processor(const Processor&) {}
    void operator=(const Processor&) {}
};

//+------------------------------------------------------------------+
#endif  // !_PROCESSOR_H_
