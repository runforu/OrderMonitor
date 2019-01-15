#include "FileUtil.h"


//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
FileUtil::FileUtil(const int nBufSize)
    : m_file(INVALID_HANDLE_VALUE)
    , m_file_size(0)
    , m_buffer(new BYTE[nBufSize])
    , m_buffer_size(nBufSize)
    , m_buffer_index(0)
    , m_buffer_readed(0)
    , m_buffer_line(0) {
}
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
FileUtil::~FileUtil() {
    //--- close the connection
    Close();
    //--- release the buffer
    if (m_buffer != NULL) {
        delete[] m_buffer;
        m_buffer = NULL;
    }
}
//+------------------------------------------------------------------+
//| Open the file                                                    |
//| dwAccess       -GENERIC_READ or  GENERIC_WRITE                   |
//| dwCreationFlags-CREATE_ALWAYS, OPEN_EXISTING, OPEN_ALWAYS        |
//+------------------------------------------------------------------+
bool FileUtil::Open(LPCTSTR lpFileName, const DWORD dwAccess, const DWORD dwCreationFlags) {
    //--- close on every case previous file
    Close();
 
    if (lpFileName != NULL) {
        //--- create file
        m_file = CreateFile(lpFileName, dwAccess, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, dwCreationFlags, FILE_ATTRIBUTE_NORMAL, NULL);
        //--- define the file size (no more than 4Gb)
        if (m_file != INVALID_HANDLE_VALUE)
            m_file_size = GetFileSize(m_file, NULL);
    }
    //--- return the result
    return (m_file != INVALID_HANDLE_VALUE);
}
//+------------------------------------------------------------------+
//| read length of bytes into buffer                                 |
//+------------------------------------------------------------------+
int FileUtil::Read(void* buffer, const DWORD length) {
    DWORD readed = 0;
 
    if (m_file == INVALID_HANDLE_VALUE || buffer == NULL || length < 1)
        return (0);
    //--- read file
    if (ReadFile(m_file, buffer, length, &readed, NULL) == 0)
        readed = 0;
    //--- return the length of reading
    return (readed);
}
//+------------------------------------------------------------------+
//| Write length of buffer into file                                 |
//+------------------------------------------------------------------+
int FileUtil::Write(const void* buffer, const DWORD length) {
    DWORD written = 0;
 
    if (m_file == INVALID_HANDLE_VALUE || buffer == NULL || length < 1)
        return (0);
    //--- write data
    if (WriteFile(m_file, buffer, length, &written, NULL) == 0)
        written = 0;
    //--- return lenght of bytes
    return (written);
}
//+------------------------------------------------------------------+
//| Reset                                 |
//+------------------------------------------------------------------+
void FileUtil::Reset() {
    //--- reset counters
    m_buffer_index = 0;
    m_buffer_readed = 0;
    m_buffer_line = 0;
    //--- rewind to begin of file
    if (m_file != INVALID_HANDLE_VALUE)
        SetFilePointer(m_file, 0, NULL, FILE_BEGIN);
}
//+------------------------------------------------------------------+
//| Read a line within maxsize bytes                                  |
//+------------------------------------------------------------------+
int FileUtil::GetNextLine(char* line, const int maxsize) {
    char *currsym = line, *lastsym = line + maxsize;
    BYTE* curpos = m_buffer + m_buffer_index;
 
    if (line == NULL || m_file == INVALID_HANDLE_VALUE || m_buffer == NULL)
        return (0);
    //--- loop
    for (;;) {
        //--- firs line or entire buffer
        if (m_buffer_line == 0 || m_buffer_index == m_buffer_readed) {
            //--- reset counters
            m_buffer_index = 0;
            m_buffer_readed = 0;
            //--- read bytes to buffer
            if (::ReadFile(m_file, m_buffer, m_buffer_size, (DWORD*)&m_buffer_readed, NULL) == 0) {
                Close();
                return (0);
            }

            if (m_buffer_readed < 1) {
                *currsym = 0;
                return (currsym != line ? m_buffer_line : 0);
            }
            curpos = m_buffer;
        }
        //--- analyse buffer
        while (m_buffer_index < m_buffer_readed) {
            //--- overflow maxsize
            if (currsym >= lastsym) {
                *currsym = 0;
                return (m_buffer_line);
            }
            //--- handle new line character
            if (*curpos == '\n') {
                //--- return character
                if (currsym > line && currsym[-1] == '\r')
                    currsym--; // wipe return character
                *currsym = 0;
                //--- return length of string
                m_buffer_line++;
                m_buffer_index++;
                return (m_buffer_line);
            }
            //--- contine
            *currsym++ = *curpos++;
            m_buffer_index++;
        }
    }
    //--- noop
    return (0);
}

