
#include "precompiled/libnet.h"
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#ifdef _WIN32
#include "winsock2.h"
#endif

#ifdef __WX__
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/msgdlg.h"
#include "wx/mstream.h"
#include "wx/socket.h"
#include "wx/tokenzr.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"

#include "global.h"

#endif

#include <memory>
#include <cstdlib>
#include "net.h"
#include "guid.h"

#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

unsigned char UniversalUniqueID::emptyUUID[UUID_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

UniversalUniqueID nullUUID;

UniversalUniqueID thisServerID;
UniversalUniqueID thisGroupID;

UniversalUniqueID& UniversalUniqueID::Set(const unsigned char* origSrc, size_t len) {
    std::lock_guard lock(guard);
    memset(bytes, 0, sizeof(bytes));
    unsigned char* p = bytes;
    const unsigned char* b = origSrc;
    size_t b_len = 0;
    for (int i = 0; i < UUID_SIZE; i++, p++) {  // allow repeating values. For example UniversalUniqueID("\xFF\xEE\xEE\xDD", 1) -> will fill it with FFEEEEDD-FFEE-EEDD-FFEE-EEDDFFEEEEDD,
        *p = *b++;
        if (++b_len >= len) {
            b_len = 0;
            b = origSrc;
        }
    }
    return *this;
}

// converts a single hex char to a number (0 - 15)
static unsigned char hexDigitToChar(char ch) {
    if (ch > 47 && ch < 58)
        return ch - 48;

    if (ch > 96 && ch < 103)
        return ch - 87;

    if (ch > 64 && ch < 71)
        return ch - 55;

    return 0;
}

// converts the two hexadecimal characters to an unsigned char (a byte)
static unsigned char hexPairToChar(char a, char b) {
    return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

// create a guid from string
UniversalUniqueID& UniversalUniqueID::Import(const std::string &fromString) {
    std::lock_guard lock(guard);
    memset(bytes, 0, sizeof(bytes));

    char char1 = ' ', char2 = ' ';
    bool isFirst = true;

    unsigned char* p = bytes;
    for (auto ch : fromString) {
        if (ch == '-')
            continue;
        if (isFirst) {
            char1 = ch;
            isFirst = false;
        } else {
            char2 = ch;
            auto byte = hexPairToChar(char1, char2);
            *p++ = byte;
            isFirst = true;
        }
    }
    return *this;
}

int UniversalUniqueID::Import(const char* s) {  // hyphenated string uuid
    std::lock_guard lock(guard);
    memset(bytes, 0, sizeof(bytes));

    char char1 = ' ', char2 = ' ';
    bool isFirst = true;

    unsigned char* p = bytes;
    int len = 0;
    for (const char* ps = s; *ps != ' '; ps++, len++) {
        const char& ch = *ps;
        if (ch == '-')
            continue;
        if (isFirst) {
            char1 = ch;
            isFirst = false;
        } else {
            char2 = ch;
            auto byte = hexPairToChar(char1, char2);
            *p++ = byte;
            isFirst = true;
        }
    }
    return len;
}

std::string UniversalUniqueID::GetString() const {
    char buf[50];
    std::string res;
    std::sprintf(buf, "%02x%02x%02x%02x-", bytes[0], bytes[1], bytes[2], bytes[3]);
    res.append(buf);
    std::sprintf(buf, "%02x%02x-%02x%02x-%02x%02x-", bytes[4], bytes[5], bytes[6], bytes[7], bytes[8], bytes[9]);
    res.append(buf);
    std::sprintf(buf, "%02x%02x%02x%02x%02x%02x", bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
    res.append(buf);
    return res;
}

std::string UniversalUniqueID::Hex() const {  // without dashes.
    char buf[50];
    std::string res;
    std::sprintf(buf, "%02x%02x%02x%02x", bytes[0], bytes[1], bytes[2], bytes[3]);
    res.append(buf);
    std::sprintf(buf, "%02x%02x%02x%02x%02x%02x", bytes[4], bytes[5], bytes[6], bytes[7], bytes[8], bytes[9]);
    res.append(buf);
    std::sprintf(buf, "%02x%02x%02x%02x%02x%02x", bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
    res.append(buf);
    return res;
}

#ifdef _WIN32
void UniversalUniqueID::Create(unsigned char* res) {
    GUID newId;
    CoCreateGuid(&newId);
    res[0] = (newId.Data1 >> 24) & 0xFF;
    res[1] = (newId.Data1 >> 16) & 0xFF;
    res[2] = (newId.Data1 >> 8) & 0xFF;
    res[3] = (newId.Data1) & 0xff;
    res[4] = (newId.Data2 >> 8) & 0xFF;
    res[5] = (newId.Data2) & 0xff;
    res[6] = (newId.Data3 >> 8) & 0xFF;
    res[7] = (newId.Data3) & 0xFF;
    res[8] = newId.Data4[0];
    res[9] = newId.Data4[1];
    res[10] = newId.Data4[2];
    res[11] = newId.Data4[3];
    res[12] = newId.Data4[4];
    res[13] = newId.Data4[5];
    res[14] = newId.Data4[6];
    res[15] = newId.Data4[7];
}

UniversalUniqueID& UniversalUniqueID::Build() {
    GUID newId;
    CoCreateGuid(&newId);
    bytes[0] = (newId.Data1 >> 24) & 0xFF;
    bytes[1] = (newId.Data1 >> 16) & 0xFF;
    bytes[2] = (newId.Data1 >> 8) & 0xFF;
    bytes[3] = (newId.Data1) & 0xff;
    bytes[4] = (newId.Data2 >> 8) & 0xFF;
    bytes[5] = (newId.Data2) & 0xff;
    bytes[6] = (newId.Data3 >> 8) & 0xFF;
    bytes[7] = (newId.Data3) & 0xFF;
    bytes[8] = newId.Data4[0];
    bytes[9] = newId.Data4[1];
    bytes[10] = newId.Data4[2];
    bytes[11] = newId.Data4[3];
    bytes[12] = newId.Data4[4];
    bytes[13] = newId.Data4[5];
    bytes[14] = newId.Data4[6];
    bytes[15] = newId.Data4[7];
    return *this;
}
#else

// sudo apt-get install uuid-dev
UniversalUniqueID& UniversalUniqueID::Build() {
    uuid_generate(bytes);
    return *this;
}

void UniversalUniqueID::Create(unsigned char* res) {
    // uuid_t is just a typedef to unsigned char [16];
    // uuid_t id; --
    uuid_generate(res);
}
#endif
