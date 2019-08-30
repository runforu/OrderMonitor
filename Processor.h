#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "Config.h"
#include "LicenseService.h"
#include "common.h"

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

    void OnStopoutsApply(const UserInfo* user, const ConGroup* group, const ConSymbol* symbol, TradeRecord* stopout);

    void OnStopsApply(const UserInfo* user, const ConGroup* group, const ConSymbol* symbol, TradeRecord* trade, const int isTP);

    void OnPendingsApply(const UserInfo* user, const ConGroup* group, const ConSymbol* symbol, const TradeRecord* pending,
                         TradeRecord* trade);

    void Initialize();

    void Shutdown(void);

private:
    Processor();
    Processor(const Processor&) {}
    void operator=(const Processor&) {}

    bool CommonCheck() {
        //--- reinitialize if configuration changed
        if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
            Initialize();
        }

        if (m_disable_plugin) {
            return false;
        }

#ifdef _LICENSE_VERIFICATION_
        if (!LicenseService::Instance().IsLicenseValid()) {
            LOG("OrderMonitor: invalid license.");
            return false;
        }
#endif  // !_LICENSE_VERIFICATION_

        return true;
    }
};

//+------------------------------------------------------------------+
#endif  // !_PROCESSOR_H_
