#ifndef _SERVERAPI_H_
#define _SERVERAPI_H_

#include <windows.h>
#include "../include/MT4ServerAPI.h"

struct CServerInterface;

class ServerApi {
public:
    static void Initialize(CServerInterface* server) {
        s_interface = server;
    }

    inline static CServerInterface* Api() {
        return s_interface;
    }

private:
    ServerApi(){};
    ~ServerApi(){};
    ServerApi(ServerApi const&) {}
    void operator=(ServerApi const&) {}

private:
    static CServerInterface* s_interface;
};

#endif  // !_SERVERAPI_H_
