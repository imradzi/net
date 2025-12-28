
#ifndef _MACADDRESS_UTILITY_H
#define _MACADDRESS_UTILITY_H

class MACAddressUtility {
public:
    static long GetMACAddress(unsigned char* result);

private:
#if defined(WIN32) || defined(UNDER_CE)
    static long GetMACAddressMSW(unsigned char* result);
#elif defined(__APPLE__)
    static long GetMACAddressMAC(unsigned char* result);
#elif defined(LINUX) || defined(linux)
    static long GetMACAddressLinux(unsigned char* result);
#endif
};

class wxMACAddressUtility {
public:
    static std::string GetMACAddress() {
        unsigned char result[6];
        if (MACAddressUtility::GetMACAddress(result) == 0) {
            return fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                (unsigned int)result[0], (unsigned int)result[1], (unsigned int)result[2],
                (unsigned int)result[3], (unsigned int)result[4], (unsigned int)result[5]);
        }
        return "";
    }
};

#endif
