#include "Config.h"
#include "Loger.h"
#include "Processor.h"
#include "ServerApi.h"

PluginInfo ExtPluginInfo = {"Order Monitor", 1, "DH Copyright.", {0}};

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            char tmp[256], *cp;
            //--- create configuration filename
            GetModuleFileName((HMODULE)hModule, tmp, sizeof(tmp) - 5);
            if ((cp = strrchr(tmp, '.')) != NULL) {
                *cp = 0;
                strcat(tmp, ".ini");
            }
            Config::Instance().Load(tmp);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }
    return (TRUE);
}

void APIENTRY MtSrvAbout(PluginInfo* info) {
    if (info != NULL) {
        memcpy(info, &ExtPluginInfo, sizeof(PluginInfo));
    }
}

int APIENTRY MtSrvStartup(CServerInterface* server) {
    if (server == NULL) {
        return (FALSE);
    }
    //--- check version
    if (server->Version() != ServerApiVersion) {
        return (FALSE);
    }
    //--- save server interface link
    ServerApi::Initialize(server);

    //--- initialize dealer helper
    Processor::Instance().Initialize();

    return (TRUE);
}

void APIENTRY MtSrvCleanup() {
    Processor::Instance().Shutdown();
}

int APIENTRY MtSrvPluginCfgSet(const PluginCfg* values, const int total) {
    LOG("MtSrvPluginCfgSet total = %d.", total);
    int res = Config::Instance().Set(values, total);
    Processor::Instance().Reinitialize();
    return (res);
}

int APIENTRY MtSrvPluginCfgNext(const int index, PluginCfg* cfg) {
    LOG("MtSrvPluginCfgNext index=%d, name=%s, value=%s.", index, cfg->name, cfg->value);
    return Config::Instance().Next(index, cfg);
}

int APIENTRY MtSrvPluginCfgTotal() {
    LOG("MtSrvPluginCfgTotal.");
    return Config::Instance().Total();
}

void APIENTRY MtSrvTradesUpdate(TradeRecord* trade, UserInfo* user, const int mode) {
    LOG("MtSrvTradesUpdate.");
    Processor::Instance().OrderUpdated(trade, user, mode);
}

void APIENTRY MtSrvTradesAddExt(TradeRecord* trade, const UserInfo* user, const ConSymbol* symbol, const int mode) {
    LOG("MtSrvTradesAddExt.");
    Processor::Instance().OrderAdded(trade, user, symbol, mode);
}

void APIENTRY MtSrvTradesCloseBy(TradeRecord* ftrade, TradeRecord* strade, TradeRecord* remaind, ConSymbol* sec,
                                 UserInfo* user) {
    LOG("MtSrvTradesCloseBy.");
    Processor::Instance().OrderClosedBy(ftrade, strade, remaind, sec, user);
}