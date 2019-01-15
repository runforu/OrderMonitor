#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>

#define TERMINATE_STR(str) str[sizeof(str) - 1] = 0;
#define COPY_STR(dst, src)                      \
    {                                           \
        if (dst != NULL && src != NULL) {       \
            strncpy(dst, src, sizeof(dst) - 1); \
            dst[sizeof(dst) - 1] = 0;           \
        }                                       \
    }

double __fastcall NormalizeDouble(const double val, int digits);

double DecPow(const int digits);

char* RemoveWhiteChar(char* str);

char* StrRange(char* str, const char begin, const char end, char** buf);

int CStrToInt(char* string);

bool IsDigitalStr(char* string);

const char* TradeTypeStr(int trade_type);

const char* TradeCmdStr(int trade_cmd);

const char* OrderTypeStr(int order_type);

//--- "OP_BUY", "OP_SELL", "OP_BUY_LIMIT", "OP_SELL_LIMIT", "OP_BUY_STOP", "OP_SELL_STOP"
int ToCmd(const char* cmd_str, int default_value = -1);

//--- TS_OPEN_NORMAL, TS_OPEN_REMAND, TS_OPEN_RESTORED, TS_CLOSED_NORMAL, TS_CLOSED_PART, TS_CLOSED_BY, TS_DELETED
const char* ToTradeRecordStateStr(int state);

#endif  // !_COMMON_H_