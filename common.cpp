#include "../include/MT4ServerAPI.h"
#include "common.h"

static const double ExtDecimalArray[9] = {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0};

double __fastcall NormalizeDouble(const double val, int digits) {
    if (digits < 0) digits = 0;
    if (digits > 8) digits = 8;

    const double p = ExtDecimalArray[digits];
    return ((val >= 0.0) ? (double(__int64(val * p + 0.5000001)) / p) : (double(__int64(val * p - 0.5000001)) / p));
}

int CheckTemplate(char* expr, char* tok_end, const char* group, char* prev, int* deep) {
    char tmp = 0;
    char *lastwc, *prev_tok;
    const char* cp;
    //---check depth
    if ((*deep)++ >= 10) return (FALSE);
    //--- skip repetition
    while (*expr == '*' && expr != tok_end) expr++;
    if (expr == tok_end) return (TRUE);
    //--- look for next "*"
    lastwc = expr;
    while (*lastwc != '*' && *lastwc != 0) lastwc++;
    //--- temporarily restrict the line
    if ((tmp = *(lastwc)) != 0)  // current not the last line
    {
        tmp = *(lastwc);
        *(lastwc) = 0;
        if ((prev_tok = (char*)strstr(group, expr)) == NULL) {
            if (tmp != 0) *(lastwc) = tmp;
            return (FALSE);
        }
        *(lastwc) = tmp;
    } else {  // last line

        cp = group + strlen(group);
        for (; cp >= group; cp--)
            if (*cp == expr[0] && strcmp(cp, expr) == 0) return (TRUE);
        return (FALSE);
    }
    //--- broken up
    if (prev != NULL && prev_tok <= prev) return (FALSE);
    prev = prev_tok;

    group = prev_tok + (lastwc - expr - 1);
    //--- end
    if (lastwc != tok_end) return CheckTemplate(lastwc, tok_end, group, prev, deep);
    return (TRUE);
}

int CheckGroup(char* grouplist, const char* group) {
    if (grouplist == NULL || group == NULL) return (FALSE);
    //--- go through all the groups
    char *tok_start = grouplist, end;
    int res = TRUE, deep = 0, normal_mode;
    while (*tok_start != 0) {
        //--- skip ','
        while (*tok_start == ',') tok_start++;

        if (*tok_start == '!') {
            tok_start++;
            normal_mode = FALSE;
        } else
            normal_mode = TRUE;
        //--- find the boundaries of the token
        char* tok_end = tok_start;
        while (*tok_end != ',' && *tok_end != 0) tok_end++;
        end = *tok_end;
        *tok_end = NULL;

        char* tp = tok_start;
        const char* gp = group;
        char* prev = NULL;
        //--- go through the token
        res = TRUE;
        while (tp != tok_end && *gp != NULL) {
            //--- find "*"
            if (*tp == '*') {
                deep = 0;
                if ((res = CheckTemplate(tp, tok_end, gp, prev, &deep)) == TRUE) {
                    *tok_end = end;
                    return (normal_mode);
                }
                break;
            }
            //--- just check
            if (*tp != *gp) {
                *tok_end = end;
                res = FALSE;
                break;
            }
            tp++;
            gp++;
        }
        //--- restore
        *tok_end = end;
        //--- we found all tokens
        if (*gp == NULL && (tp == tok_end || *tp == '*') && res == TRUE) return (normal_mode);
        //--- next token
        if (*tok_end == 0) break;
        tok_start = tok_end + 1;
    }

    return (FALSE);
}

double DecPow(const int digits) {
    static double decarray[9] = {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0};

    if (digits < 0 || digits > 8) return (0);

    return decarray[digits];
}

int CStrToInt(char* string) {
    if (string != NULL) {
        return atoi(string);
    }
    return 0;
}

char* RemoveWhiteChar(char* str) {
    char *pend = str, *pch = str;
    while (*pch != 0) {
        if (isspace(*pch)) {
            pch++;
            continue;
        }
        if (pend != pch) {
            *pend = *pch;
        }
        pend++;
        pch++;
    }
    *pend = 0;
    return str;
}

char* StrRange(char* str, const char begin, const char end, char** buf) {
    char* cp = str == NULL ? *buf : str;
    if (cp == NULL) {
        return NULL;
    }
    if (*cp == 0) {
        return NULL;
    }
    *buf = NULL;

    if ((cp = strchr(cp, begin)) == NULL) {
        return NULL;
    }
    cp++;
    char* result = cp;

    if ((cp = strchr(cp, end)) == NULL) {
        return NULL;
    }
    *cp = 0;
    cp++;
    if (strchr(result, begin) == NULL) {
        *buf = cp;
        return result;
    }
    return NULL;
}

bool IsDigitalStr(char* string) {
    if (string == NULL) {
        return false;
    }

    while (*string != 0) {
        if (*string < '0' || *string > '9') {
            return false;
        }
        string++;
    }
    return true;
}

const char* TradeTypeStr(int trade_type) {
    static char* trans_type_str[] = {
        "TT_ORDER_IE_OPEN",      "TT_ORDER_REQ_OPEN",  "TT_ORDER_MK_OPEN",     "TT_ORDER_PENDING_OPEN", "TT_ORDER_IE_CLOSE",
        "TT_ORDER_REQ_CLOSE",    "TT_ORDER_MK_CLOSE",  "TT_ORDER_MODIFY",      "TT_ORDER_DELETE",       "TT_ORDER_CLOSE_BY",
        "TT_ORDER_CLOSE_ALL",    "TT_BR_ORDER_OPEN",   "TT_BR_ORDER_CLOSE",    "TT_BR_ORDER_DELETE",    "TT_BR_ORDER_CLOSE_BY",
        "TT_BR_ORDER_CLOSE_ALL", "TT_BR_ORDER_MODIFY", "TT_BR_ORDER_ACTIVATE", "TT_BR_ORDER_COMMENT",   "TT_BR_BALANCE",
    };
    return trans_type_str[trade_type - 64];
}

const char* TradeCmdStr(int trade_cmd) {
    static char* trans_cmd_str[] = {"OP_BUY",      "OP_SELL",      "OP_BUY_LIMIT", "OP_SELL_LIMIT",
                                    "OP_BUY_STOP", "OP_SELL_STOP", "OP_BALANCE",   "OP_CREDIT"};
    return trans_cmd_str[trade_cmd];
}

const char* OrderTypeStr(int order_type) {
    static char* price_option_str[] = {"OT_NONE",
                                       "OT_OPEN",
                                       "OT_CLOSE",
                                       "OT_CLOSE|OT_OPEN"
                                       "OT_TP",
                                       "OT_TP|OT_OPEN",
                                       "OT_TP|OT_CLOSE",
                                       "OT_TP|OT_CLOSE|OT_OPEN",
                                       "OT_SL",
                                       "OT_SL|OT_OPEN",
                                       "OT_SL|OT_CLOSE",
                                       "OT_SL|OT_CLOSE|OT_OPEN",
                                       "OT_SL|OT_TP",
                                       "OT_SL|OT_TP|OT_OPEN",
                                       "OT_SL|OT_TP|OT_CLOSE",
                                       "OT_SL|OT_TP|OT_CLOSE|OT_OPEN",
                                       "OT_PENDING",
                                       "OT_PENDING|OT_OPEN",
                                       "OT_PENDING|OT_CLOSE",
                                       "OT_PENDING|OT_CLOSE|OT_OPEN"
                                       "OT_PENDING|OT_TP",
                                       "OT_PENDING|OT_TP|OT_OPEN",
                                       "OT_PENDING|OT_TP|OT_CLOSE",
                                       "OT_PENDING|OT_TP|OT_CLOSE|OT_OPEN",
                                       "OT_PENDING|OT_SL",
                                       "OT_PENDING|OT_SL|OT_OPEN",
                                       "OT_PENDING|OT_SL|OT_CLOSE",
                                       "OT_PENDING|OT_SL|OT_CLOSE|OT_OPEN",
                                       "OT_PENDING|OT_SL|OT_TP",
                                       "OT_PENDING|OT_SL|OT_TP|OT_OPEN",
                                       "OT_PENDING|OT_SL|OT_TP|OT_CLOSE",
                                       "OT_ALL"};
    return price_option_str[order_type];
}

int ToCmd(const char* cmd_str, int default_value) {
    if (cmd_str == NULL) {
        return default_value;
    }
    if (strcmp("OP_BUY", cmd_str) == 0) {
        return OP_BUY;
    } else if (strcmp("OP_BUY", cmd_str) == 0) {
        return OP_BUY;
    } else if (strcmp("OP_SELL", cmd_str) == 0) {
        return OP_SELL;
    } else if (strcmp("OP_BUY_LIMIT", cmd_str) == 0) {
        return OP_BUY_LIMIT;
    } else if (strcmp("OP_SELL_LIMIT", cmd_str) == 0) {
        return OP_SELL_LIMIT;
    } else if (strcmp("OP_BUY_STOP", cmd_str) == 0) {
        return OP_BUY_STOP;
    } else if (strcmp("OP_SELL_STOP", cmd_str) == 0) {
        return OP_SELL_STOP;
    }

    return default_value;
}

const char* ToTradeRecordStateStr(int state) {
    if (state < TS_OPEN_NORMAL || state > TS_DELETED) {
        return NULL;
    }
    static const char* string[] = {"TS_OPEN_NORMAL", "TS_OPEN_REMAND", "TS_OPEN_RESTORED", "TS_CLOSED_NORMAL",
                                   "TS_CLOSED_PART", "TS_CLOSED_BY",   "TS_DELETED"};
    return string[state];
}
