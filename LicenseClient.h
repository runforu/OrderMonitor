#ifndef _LICENSECLIENT_H_
#define _LICENSECLIENT_H_

#ifdef _LICENSE_VERIFICATION_

#include <openssl/ssl.h>

class LicenseClient {
public:
    static const int kKeyLength = 16;

private:
    static const unsigned char m_server_certificate[1395];
    int serial_number;

public:
    LicenseClient();

    ~LicenseClient();

    bool Verify(const char *server, const char *port, char (&key)[kKeyLength]);

    void ZeroSerialNumber();

private:
    LicenseClient(const LicenseClient &client) = delete;

    LicenseClient &operator=(const LicenseClient &client) = delete;

    void PBKDF2_HMAC_SHA1(const char (&input)[kKeyLength], int iterations, unsigned char (&result)[kKeyLength]);

    void HexPrintf(unsigned char (&in)[kKeyLength]);

    bool CheckCertificate();

    int ReadLength(SSL *ssl);

    bool Read(SSL *ssl, char *buf, int size);

    int WriteLength(SSL *ssl, int len);

    bool Write(SSL *ssl, const void *buf, int len);

    bool Lookup(const char *server, SOCKADDR_IN *socket_addr);
};

#endif // !_LICENSE_VERIFICATION_

#endif  // !_LICENSECLIENT_H_