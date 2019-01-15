#include <stdio.h>
#include "Config.h"
#include "FileUtil.h"
#include "Loger.h"

// Config ExtConfig;
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
Config::Config() : m_cfg(NULL), m_cfg_total(0), m_cfg_max(0) {
    m_filename[0] = 0;
}
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
Config::~Config() {
    m_sync.Lock();

    if (m_cfg != NULL) {
        delete[] m_cfg;
        m_cfg = NULL;
    }
    m_cfg_total = m_cfg_max = 0;

    m_sync.Unlock();
}
Config& Config::Instance() {
    static Config _instance;
    return _instance;
}
//+------------------------------------------------------------------+
//| Load config from file                                            |
//+------------------------------------------------------------------+
void Config::Load(LPCSTR filename) {
    char tmp[MAX_PATH], *cp, *start;
    FileUtil in;
    PluginCfg cfg, *buf;

    if (filename == NULL) return;
    //--- copy file name
    m_sync.Lock();
    COPY_STR(m_filename, filename);
    //--- open file
    if (in.Open(m_filename, GENERIC_READ, OPEN_EXISTING)) {
        while (in.GetNextLine(tmp, sizeof(tmp) - 1) > 0) {
            if (tmp[0] == ';') continue;
            //--- omit white space
            start = tmp;
            while (*start == ' ') start++;
            if ((cp = strchr(start, '=')) == NULL) continue;
            *cp = 0;
            //--- copy config name
            ZeroMemory(&cfg, sizeof(cfg));
            COPY_STR(cfg.name, start);
            //--- skip space character
            cp++;
            while (*cp == ' ') cp++;
            COPY_STR(cfg.value, cp);

            if (cfg.name[0] == 0 || cfg.value[0] == 0) continue;
            //--- add
            if (m_cfg == NULL || m_cfg_total >= m_cfg_max)  // place here
            {
                //--- reallocate memory
                if ((buf = new PluginCfg[m_cfg_total + 64]) == NULL) break;
                //--- copy configs to new memory
                if (m_cfg != NULL) {
                    if (m_cfg_total > 0) memcpy(buf, m_cfg, sizeof(PluginCfg) * m_cfg_total);
                    delete[] m_cfg;
                }
                //--- replace old buffer
                m_cfg = buf;
                m_cfg_max = m_cfg_total + 64;
            }
            //--- add new config
            memcpy(&m_cfg[m_cfg_total++], &cfg, sizeof(PluginCfg));
        }
        //--- close
        in.Close();
    }
    //--- sort config by name
    if (m_cfg != NULL && m_cfg_total > 0) qsort(m_cfg, m_cfg_total, sizeof(PluginCfg), SortByName);
    m_sync.Unlock();
    LOG("------------Load----------Load------Load------");
}
//+------------------------------------------------------------------+
//| save configs to file                                             |
//+------------------------------------------------------------------+
void Config::Save(void) {
    FileUtil out;
    char tmp[512];
    //--- write
    m_sync.Lock();
    if (m_filename[0] != 0)
        if (out.Open(m_filename, GENERIC_WRITE, CREATE_ALWAYS)) {
            if (m_cfg != NULL)
                for (int i = 0; i < m_cfg_total; i++) {
                    _snprintf(tmp, sizeof(tmp) - 1, "%s=%s\n", m_cfg[i].name, m_cfg[i].value);
                    if (out.Write(tmp, strlen(tmp)) < 1) break;
                }
            //--- close file
            out.Close();
        }
    m_sync.Unlock();
}
//+------------------------------------------------------------------+
//| Search config by name                                            |
//+------------------------------------------------------------------+
PluginCfg* Config::Search(LPCSTR name) {
    PluginCfg* config = NULL;

    if (m_cfg != NULL && m_cfg_total > 0)
        config = (PluginCfg*)bsearch(name, m_cfg, m_cfg_total, sizeof(PluginCfg), SearchByName);

    return (config);
}

int Config::Add(const char* name, const char* value, bool save) {
    PluginCfg *config, *buf;

    if (name == NULL || name[0] == 0 || value == NULL) {
        return (FALSE);
    }

    m_sync.Lock();
    if ((config = Search(name)) != NULL) {
        memcpy(config->name, name, sizeof(PluginCfg::name));
        memcpy(config->value, value, sizeof(PluginCfg::value));
    } else {
        //--- reach max size or empty
        if (m_cfg == NULL || m_cfg_total >= m_cfg_max) {
            //--- reallocate memory
            if ((buf = new PluginCfg[m_cfg_total + 64]) == NULL) {
                m_sync.Unlock();
                return (FALSE);
            }
            //--- copy old configs
            if (m_cfg != NULL) {
                if (m_cfg_total > 0) {
                    memcpy(buf, m_cfg, sizeof(PluginCfg) * m_cfg_total);
                }
                delete[] m_cfg;
            }
            //--- replace old buf
            m_cfg = buf;
            m_cfg_max = m_cfg_total + 64;
        }
        //--- add new config
        memcpy(config->name, name, sizeof(PluginCfg::name));
        memcpy(config->value, value, sizeof(PluginCfg::value));
        //--- sort configs by name
        qsort(m_cfg, m_cfg_total, sizeof(PluginCfg), SortByName);
    }
    m_sync.Unlock();

    if (save) {
        Save();
    }

    return (TRUE);
}

int Config::Add(const PluginCfg* cfg) {
    PluginCfg *config, *buf;

    if (cfg == NULL || cfg->name[0] == 0) return (FALSE);

    m_sync.Lock();
    if ((config = Search(cfg->name)) != NULL)
        memcpy(config, cfg, sizeof(PluginCfg));
    else {
        //--- reach max size or empty
        if (m_cfg == NULL || m_cfg_total >= m_cfg_max) {
            //--- reallocate memory
            if ((buf = new PluginCfg[m_cfg_total + 64]) == NULL) {
                m_sync.Unlock();
                return (FALSE);
            }
            //--- copy old configs
            if (m_cfg != NULL) {
                if (m_cfg_total > 0) memcpy(buf, m_cfg, sizeof(PluginCfg) * m_cfg_total);
                delete[] m_cfg;
            }
            //--- replace old buf
            m_cfg = buf;
            m_cfg_max = m_cfg_total + 64;
        }
        //--- add new config
        memcpy(&m_cfg[m_cfg_total++], cfg, sizeof(PluginCfg));
        //--- sort configs by name
        qsort(m_cfg, m_cfg_total, sizeof(PluginCfg), SortByName);
    }
    m_sync.Unlock();
    //--- save configs to file
    Save();
    //--- return
    return (TRUE);
}

//+------------------------------------------------------------------+
//| Batch set configs                                       |
//+------------------------------------------------------------------+
int Config::Set(const PluginCfg* values, const int total) {
    if (total < 0) return (FALSE);

    m_sync.Lock();
    if (values != NULL && total > 0) {
        //--- overflow or empty
        if (m_cfg == NULL || total >= m_cfg_max) {
            //--- replace old buffer with new buffer
            if (m_cfg != NULL) delete[] m_cfg;
            if ((m_cfg = new PluginCfg[total + 64]) == NULL) {
                m_cfg_max = m_cfg_total = 0;
                m_sync.Unlock();
                return (FALSE);
            }
            //--- set max config capacity
            m_cfg_max = total + 64;
        }
        //--- copy configs
        memcpy(m_cfg, values, sizeof(PluginCfg) * total);
    }
    //--- expose new configs
    m_cfg_total = total;
    if (m_cfg != NULL && m_cfg_total > 0) qsort(m_cfg, m_cfg_total, sizeof(PluginCfg), SortByName);
    m_sync.Unlock();
    //--- save to file
    Save();
    return (TRUE);
}
//+------------------------------------------------------------------+
//| Look for config by name                                          |
//+------------------------------------------------------------------+
int Config::Get(LPCSTR name, PluginCfg* cfg) {
    PluginCfg* config = NULL;
    if (name != NULL && cfg != NULL) {
        m_sync.Lock();
        if ((config = Search(name)) != NULL) memcpy(cfg, config, sizeof(PluginCfg));
        m_sync.Unlock();
    }
    return (config != NULL);
}
//+------------------------------------------------------------------+
//| Get config at index                                              |
//+------------------------------------------------------------------+
int Config::Next(const int index, PluginCfg* cfg) {
    if (cfg != NULL && index >= 0) {
        m_sync.Lock();
        if (m_cfg != NULL && index < m_cfg_total) {
            memcpy(cfg, &m_cfg[index], sizeof(PluginCfg));
            m_sync.Unlock();
            return (TRUE);
        }
        m_sync.Unlock();
    }
    //--- not found
    return (FALSE);
}
//+------------------------------------------------------------------+
//| Remove config by name                                            |
//+------------------------------------------------------------------+
int Config::Delete(LPCSTR name) {
    PluginCfg* config = NULL;
    if (name != NULL) {
        m_sync.Lock();
        if ((config = Search(name)) != NULL) {
            int index = config - m_cfg;
            if ((index + 1) < m_cfg_total) memmove(config, config + 1, sizeof(PluginCfg) * (m_cfg_total - index - 1));
            m_cfg_total--;
        }
        //--- sort
        if (m_cfg != NULL && m_cfg_total > 0) qsort(m_cfg, m_cfg_total, sizeof(PluginCfg), SortByName);
        m_sync.Unlock();
    }
    return (config != NULL);
}

int Config::SortByName(const void* left, const void* right) {
    return strcmp(((PluginCfg*)left)->name, ((PluginCfg*)right)->name);
}

int Config::SearchByName(const void* left, const void* right) {
    return strcmp((char*)left, ((PluginCfg*)right)->name);
}

int Config::GetInteger(LPCSTR name, int* value, LPCSTR defvalue) {
    PluginCfg* config = NULL;
    if (name != NULL && value != NULL) {
        m_sync.Lock();
        if ((config = Search(name)) != NULL)
            *value = atoi(config->value);
        else if (defvalue != NULL) {
            m_sync.Unlock();
            //--- new config
            PluginCfg cfg = {0};
            COPY_STR(cfg.name, name);
            COPY_STR(cfg.value, defvalue);
            Add(&cfg);
            //--- set default value
            *value = atoi(cfg.value);
            return (TRUE);
        }
        m_sync.Unlock();
    }
    //--- return
    return (config != NULL);
}

int Config::GetLong(LPCSTR name, long* value, LPCSTR defvalue) {
    PluginCfg* config = NULL;
    if (name != NULL && value != NULL) {
        m_sync.Lock();
        if ((config = Search(name)) != NULL) {
            *value = atol(config->value);
        } else if (defvalue != NULL) {
            m_sync.Unlock();
            PluginCfg cfg = {0};
            COPY_STR(cfg.name, name);
            COPY_STR(cfg.value, defvalue);
            Add(&cfg);
            *value = atol(cfg.value);
            return (TRUE);
        }
        m_sync.Unlock();
    }
    return (config != NULL);
}

int Config::GetString(LPCSTR name, LPTSTR value, const int maxlen, LPCSTR defvalue) {
    PluginCfg* config = NULL;
    if (name != NULL && value != NULL) {
        m_sync.Lock();
        if ((config = Search(name)) != NULL) {
            strncpy(value, config->value, maxlen);
            value[maxlen] = 0;
        } else if (defvalue != NULL) {
            m_sync.Unlock();
            //--- new config
            PluginCfg cfg = {0};
            COPY_STR(cfg.name, name);
            COPY_STR(cfg.value, defvalue);
            Add(&cfg);
            //--- copy value
            strncpy(value, cfg.value, maxlen);
            value[maxlen] = 0;
            return (TRUE);
        }
        m_sync.Unlock();
    }
    //--- return
    return (config != NULL);
}

bool Config::HasKey(LPCSTR name) {
    if (name != NULL) {
        m_sync.Lock();
        PluginCfg* config = Search(name);
        m_sync.Unlock();
        return config != NULL;
    }
    return FALSE;
}
//--- end
