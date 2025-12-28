#pragma once

#include <list>
#include <chrono>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range.hpp>
#include <boost/range/join.hpp>
#if defined(snprintf) && defined(_MSC_VER)
#undef snprintf
#endif
using namespace std::chrono_literals;

std::string GetHostIP(const std::string &hostName);
std::string GetMD5name(const std::string &key);
std::string GetMD5key(const std::string &key);
std::string GetSHA1name(const std::string &key);
std::string GetSHA256name(const std::string &key);
inline std::string GetSHA1key(const std::string &key) { return GetSHA1name(key); }
inline std::string GetSHA256key(const std::string &key) { return GetSHA256name(key); }

inline std::string GetEncryptedString(const std::string &text) { return GetSHA256name(text); }
inline std::string GetHashedString(const std::string &text) { return GetSHA256name(text); }
inline std::string GetShortHashedString(const std::string &text) { return GetSHA1name(text); }
#ifdef HAS_EXPIRY
bool CheckValidCopy(wxFile& f, const char*);
void SetValidCopy(wxFile& f, const char*);
#endif
// extern int g_MyTimeZone;

namespace DB {
    enum SoftwareExtensions { 
        accounting,
        ticketing,
        catering,
        online,
        onlineMarketPlace,
        rental,
        stockTracking,
        mykad,
        backup,
        dispensing,
        patientRecord,
        hr,
        eInvoice};
    extern const char* softwareExtensionKey[];
    inline std::string GetActivationKey(const std::string &cidHash, const std::string &key) {return GetShortHashedString(cidHash + key);}
    inline std::string GetAccountingActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::accounting]); }
    inline std::string GetHRActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::hr]); }
    inline std::string GetTicketingActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::ticketing]); }
    inline std::string GetCateringActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::catering]); }
    inline std::string GetOnlineActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::online]); }
    inline std::string GetOnlineMarketPlaceActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::onlineMarketPlace]); }
    inline std::string GetRentalActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::rental]); }
    inline std::string GetStockTrackingActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::stockTracking]); }
    inline std::string GetMyKadActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::mykad]); }
    inline std::string GetBackupActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::backup]); }
    inline std::string GetDispensingActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::dispensing]); }
    inline std::string GetPatientRecordActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::patientRecord]); }
    inline std::string Get_eInvoiceActivationKey(const std::string &cid) { return GetActivationKey(cid, softwareExtensionKey[SoftwareExtensions::eInvoice]); }
}
