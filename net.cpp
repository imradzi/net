#include "project_common.h"
// #include "boost/system/system_error.hpp"
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef _WIN32
#include "winsock2.h"
#endif

#ifdef __clang__
#if __has_warning("-Wdeprecated-enum-enum-conversion")
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"  // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#endif
#endif

#ifdef __WX__
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/socket.h"
#include "global.h"
#include "words.h"
#include "logger/logger.h"
#endif
#include <fstream>
#include <array>
#include <unordered_set>
#include <cstdlib>

#include "md5.h"
#include "net.h"
#include "sha01.h"
#include "sha256.h"
#include "guid.h"
#include <boost/asio.hpp>
#include <fmt/format.h>

const char *DB::softwareExtensionKey[] = {
    "enable_AccountingAddOn", 
    "enable_TicketingAddOn", 
    "enable_CateringAddOn", 
    "enable_OnlineAddOn", 
    "enable_OnlineMarketPlace", 
    "enable_RentalAddOn", 
    "enable_PrintReceiveLabel", 
    "enable_MyKad", 
    "enable_Backup", 
    "enable_Dispensing", 
    "enable_PatientRecord", 
    "enable_HumanResources", 
    "enable_eInvoice"
};

std::string GetHostIP(const std::string &hostName) {
    using namespace boost;
    asio::io_context io_context;
    system::error_code error;

    asio::ip::tcp::resolver resolver(io_context);

    asio::ip::tcp::resolver::results_type results = resolver.resolve(hostName, "80", error);
    
    for (asio::ip::tcp::endpoint const endpoint : results) {
        if (endpoint.address().is_v4()) {
            return endpoint.address().to_string();
        }
    }

    return std::string(hostName);
}


std::string GetMD5name(const std::string &sql) {
    ShowLog("GetMD5name: " + sql);
    MD5 md5;
    md5.update((unsigned char*)sql.data(), sql.size());
    md5.finalize();
    char* s = md5.hex_digest();
    std::string res = s;
    delete[] s;
    return res;
}

std::string GetMD5key(const std::string &sql) {
    ShowLog("GetMD5key: " + sql);
    MD5 md5;
    md5.update((unsigned char*)sql.data(), sql.size());
    md5.finalize();
    char* s = md5.hex_digest();
    std::string md5name = s;
    delete[] s;
    return md5name;
}

std::string GetSHA1name(const std::string &sql) {
    ShowLog("GetSHA1name: " + sql);
    SHA1 sha;
    unsigned int digest[5];
    sha.Reset();
    sha << std::string(sql).c_str();
    if (!sha.Result(digest)) {
        throw std::runtime_error(fmt::format("cannot encrypt {}", sql));
    }
    std::string res;
    res.append(String::ToHex(digest[0], 8));
    res.append(String::ToHex(digest[1], 8));
    res.append(String::ToHex(digest[2], 8));
    res.append(String::ToHex(digest[3], 8));
    res.append(String::ToHex(digest[4], 8));
    return res;
}

std::string GetSHA256name(const std::string &sql) {
    ShowLog("GetSHA256name: " + sql);
    uint8_t digest[32];
    computeSHA256(sql.data(), sql.size(), digest);
    std::string res;
    for (int i = 0; i < 32; i++) {
        std::string hs = String::ToHex(digest[i], 2);
        res.append(hs);
    }

#ifndef NDEBUG
    {
        const char* s = "test";
        uint8_t digest[32];
        computeSHA256(s, strlen(s), digest);
        std::string r;
        for (int i = 0; i < 32; i++) {
            r.append(String::ToHex(digest[i], 2));
        }
        int i = 0;
        i++;
    }
#endif

    return res;
}
