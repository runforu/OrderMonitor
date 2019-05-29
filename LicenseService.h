#ifndef _LICENSESERVICE_H_
#define _LICENSESERVICE_H_

#ifdef _LICENSE_VERIFICATION_

#include "LicenseClient.h"

class LicenseService {
public:
    static LicenseService& Instance();

    inline bool IsLicenseValid() {
        return m_is_license_valid;
    }

    void ResetLicense();

    void Stop() {
        m_stop = true;
        WaitForSingleObject(m_thread, 1000);
    }

private:
    LicenseService();

    ~LicenseService() = default;

    LicenseService(LicenseService const&) = delete;

    void operator=(LicenseService const&) = delete;

    static void Verify(void* para);

    void DoVerify();

private:
    static const int s_interval_second = 24 * 60 * 60;
    char m_key[LicenseClient::kKeyLength];
    bool m_is_license_valid;
    bool m_stop;
    LicenseClient m_license_client;
    time_t m_last_verification;
    HANDLE m_thread;
};

#endif  // _LICENSE_VERIFICATION_

#endif  // !_LICENSE_VERIFICATION_
