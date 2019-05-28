#ifdef _LICENSE_VERIFICATION_
#include <WinSock2.h>
#include <process.h>
#include <windows.h>
#include "Config.h"
#include "LicenseService.h"
#include "Loger.h"

LicenseService& LicenseService::Instance() {
    static LicenseService _instance;
    return _instance;
}

void LicenseService::ResetLicense() {
    FUNC_WARDER;
    m_is_license_valid = true;
    m_license_client.ZeroSerialNumber();
    char key[LicenseClient::kKeyLength + 1];
    Config::Instance().GetString("license", key, sizeof(key) - 1, "0000000000000000");
    memcpy(m_key, key, LicenseClient::kKeyLength);
}

LicenseService::LicenseService() : m_is_license_valid(true) {
    m_thread = (HANDLE)_beginthread(Verify, 0, this);
}

void LicenseService::Verify(void* para) {
    FUNC_WARDER;
    ((LicenseService*)para)->DoVerify();
}

void LicenseService::DoVerify() {
    FUNC_WARDER;
    while (!m_stop) {
        for (int i = 0; i < s_interval_second * 10; ++i) {
            Sleep(100);
            if (m_stop) {
                return;
            }
        }
        if (time(NULL) - m_last_verification > s_interval_second) {
            m_is_license_valid = m_license_client.Verify("mt4.duhui.info", "523", m_key);
            m_last_verification = time(NULL);
        }
    }
}

#endif  // _LICENSE_VERIFICATION_