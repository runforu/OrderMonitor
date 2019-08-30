#include <process.h>
#include <stdio.h>
#include "HttpPost.h"
#include "Loger.h"
#include "Processor.h"
#include "ServerApi.h"

void Processor::Shutdown(void) {
    ShowStatus();

#ifdef _LICENSE_VERIFICATION_
    LicenseService::Instance().Stop();
#endif  // !_LICENSE_VERIFICATION_
}

Processor::Processor() : m_reinitialize_flag(0), m_disable_plugin(0) {}

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
    Config::Instance().GetString("Server", m_notice_server, sizeof(m_notice_server) - 1, "http://localhost");
    HttpPost::Instance().SetUrl(m_notice_server);

#ifdef _LICENSE_VERIFICATION_
    LicenseService::Instance().ResetLicense();
#endif  // !_LICENSE_VERIFICATION_
}

void Processor::OrderUpdated(TradeRecord* trade, UserInfo* user, const int mode) {
    if (!CommonCheck()) {
        return;
    }

    if (trade->cmd >= OP_BALANCE) {
        return;
    }

    boost::property_tree::ptree notice;
    notice.put("order", trade->order);
    notice.put("user", user->login);
    switch (mode) {
        case UPDATE_NORMAL:
            if (trade->cmd == OP_BUY || trade->cmd == OP_SELL) {
                notice.put("mode", "update");
            } else {
                notice.put("mode", "p_update");
            }
            break;
        case UPDATE_ACTIVATE:
            notice.put("mode", "active");
            break;
        case UPDATE_CLOSE:
            notice.put("mode", "close");
            break;
        case UPDATE_DELETE:
            notice.put("mode", "p_delete");
            break;
        default:
            notice.put("error", "Unknown");
            break;
    }
    HttpPost::Instance().AddNotice(notice);
}

void Processor::OrderAdded(TradeRecord* trade, const UserInfo* user, const ConSymbol* symbol, const int mode) {
    if (!CommonCheck()) {
        return;
    }

    if (trade->cmd >= OP_BALANCE) {
        return;
    }

    // OP_BUY_LIMIT, OP_SELL_LIMIT, OP_BUY_STOP, OP_SELL_STOP

    boost::property_tree::ptree notice;
    notice.put("order", trade->order);
    notice.put("user", user->login);
    switch (mode) {
        case OPEN_NEW:
            if (trade->cmd == OP_BUY || trade->cmd == OP_SELL) {
                notice.put("mode", "open");
            } else {
                notice.put("mode", "p_open");
            }
            break;
        case OPEN_CLOSE:
            notice.put("mode", "OPEN_CLOSE");
            break;
        case OPEN_RESTORE:
            notice.put("mode", "OPEN_RESTORE");
            break;
        case OPEN_API:
            notice.put("mode", "OPEN_API");
            break;
        case OPEN_ROLLOVER:
            notice.put("mode", "OPEN_ROLLOVER");
        default:
            notice.put("error", "Unknown");
            break;
    }
    HttpPost::Instance().AddNotice(notice);
}

void Processor::OrderClosedBy(TradeRecord* ftrade, TradeRecord* strade, TradeRecord* remaind, ConSymbol* sec, UserInfo* user) {
    // Do nothing
}

 void Processor::OnStopoutsApply(const UserInfo * user, const ConGroup * group, const ConSymbol * symbol, TradeRecord * stopout) {
    FUNC_WARDER;
    if (!CommonCheck()) {
        return;
    }
    boost::property_tree::ptree notice;
    notice.put("order", stopout->order);
    notice.put("user", user->login);
    notice.put("mode", "STOPOUT");
    HttpPost::Instance().AddNotice(notice);
}

  void Processor::OnStopsApply(const UserInfo * user, const ConGroup * group, const ConSymbol * symbol, TradeRecord * trade, const int isTP) {
     FUNC_WARDER;
     if (!CommonCheck()) {
         return;
     }
     boost::property_tree::ptree notice;
     notice.put("order", trade->order);
     notice.put("user", user->login);
     notice.put("mode", isTP ? "TP" : "SL");
     HttpPost::Instance().AddNotice(notice);
 }

   void Processor::OnPendingsApply(const UserInfo * user, const ConGroup * group, const ConSymbol * symbol, const TradeRecord * pending, TradeRecord * trade) {
      FUNC_WARDER;
      if (!CommonCheck()) {
          return;
      }
      boost::property_tree::ptree notice;
      notice.put("order", pending->order);
      notice.put("user", user->login);
      notice.put("mode", "ACTIVATION");
      HttpPost::Instance().AddNotice(notice);
  }
