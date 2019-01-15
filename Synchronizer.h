#ifndef _SYNCHRONIZER_H_
#define _SYNCHRONIZER_H_

#include <windows.h>

class Synchronizer {
private:
    CRITICAL_SECTION m_cs;

public:
    Synchronizer() {
        ZeroMemory(&m_cs, sizeof(m_cs));
        InitializeCriticalSection(&m_cs);
    }
    ~Synchronizer() {
        DeleteCriticalSection(&m_cs);
        ZeroMemory(&m_cs, sizeof(m_cs));
    }
    inline void Lock() { EnterCriticalSection(&m_cs); }
    inline void Unlock() { LeaveCriticalSection(&m_cs); }
};

#endif