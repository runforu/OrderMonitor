#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <time.h>
#include <windows.h>
#include "Synchronizer.h"
#include "common.h"
#include "../include/MT4ServerAPI.h"

//+------------------------------------------------------------------+
//| Simple configuration                                             |
//+------------------------------------------------------------------+
class Config {
private:
    Synchronizer m_sync;        // synchronizer
    char m_filename[MAX_PATH];  // name of the configuration file
    PluginCfg* m_cfg;           // configs
    int m_cfg_total;            // total number of records
    int m_cfg_max;              // max number of records

public:
    static Config& Instance();

    //--- Initializing the database (reading the config file)
    void Load(LPCSTR filename);
    void Save(void);

    //--- access
    int Add(const char* name, const char* value, bool save = false);
    int Add(const PluginCfg* cfg);
    int Set(const PluginCfg* values, const int total);
    int Get(LPCSTR name, PluginCfg* cfg);
    int Next(const int index, PluginCfg* cfg);
    int Delete(LPCSTR name);
    inline int Total(void) {
        m_sync.Lock();
        int total = m_cfg_total;
        m_sync.Unlock();
        return (total);
    }

    int GetInteger(LPCSTR name, int* value, LPCSTR defvalue = NULL);
    int GetLong(LPCSTR name, long* value, LPCSTR defvalue = NULL);
    int GetString(LPCSTR name, LPTSTR value, const int maxlen, LPCSTR defvalue = NULL);
    bool HasKey(LPCSTR name);

private:
    Config();
    ~Config();
    Config(Config const&) {}
    void operator=(Config const&) {}
    PluginCfg* Search(LPCSTR name);
    static int SortByName(const void* left, const void* right);
    static int SearchByName(const void* left, const void* right);
};

// extern Config ExtConfig;
//+------------------------------------------------------------------+
#endif  // !_CONFIGURATION_H_
