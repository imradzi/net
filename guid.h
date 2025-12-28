#pragma once
#include <mutex>
#include <memory>
#include <string>
#include <thread>
#include <cstring>
#include <string_view>

namespace String {
    template<typename T>
    inline std::string ToHex(const T& v, int nDigit) {
        char p[20];
        sprintf(p, "%0*x", nDigit, v);
        return p;
    }
}

template<typename T>
T FNVHash32(const T& key) {
    if (key.empty()) return T("<empty>");
    unsigned int h = 0x811c9dc5;
    for (auto str : key) {
        unsigned int p = str;
        h = h * 0x01000193;
        h = h ^ p;
    }
    unsigned char* s = (unsigned char*)&h;
    T res;
    for (size_t i = 0; i < sizeof(int); i++, s++)
        res.append(String::ToHex(*s, 2));
    return res;
}

template<typename T>
T FNVHash64(const T& k) {
    if (k.empty()) return T("<empty>");
    unsigned long long h = 0xcbf29ce484222325ULL;
    for (auto str : k) {
        unsigned int p = str;
        h = h * 0x100000001b3ULL;
        h = h ^ p;
    }
    unsigned char* s = (unsigned char*)&h;
    T res;
    for (size_t i = 0; i < sizeof(long long); i++, s++)
        res.append(String::ToHex(*s, 2));
    return res;
}

#define UUID_SIZE 16
class UniversalUniqueID {
    unsigned char bytes[UUID_SIZE];
    std::recursive_mutex guard;

public:
    static unsigned char emptyUUID[UUID_SIZE];
    // static unsigned char terminalGuid[UUID_SIZE];

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & bytes;
    }

    UniversalUniqueID() { Copy(emptyUUID); }
    explicit UniversalUniqueID(const unsigned char* b, size_t len = UUID_SIZE) {
        if (len == UUID_SIZE)
            Copy(b);
        else
            Set(b, len);
    }  // Set is more expensive.
    explicit UniversalUniqueID(unsigned char fillChar) {
        std::lock_guard lock(guard);
        memset(bytes, fillChar, sizeof(bytes));
    }  // initiate with fillChar =
    UniversalUniqueID(const UniversalUniqueID& r) {
        std::lock_guard lock(guard);
        std::memcpy(bytes, r.bytes, sizeof(unsigned char) * UUID_SIZE);
    }

    // explicit UniversalUniqueID(const std::string &r) { Import(r); }

    UniversalUniqueID& operator=(const UniversalUniqueID& other) {
        std::lock_guard lock(guard);
        memcpy(bytes, other.bytes, sizeof(unsigned char) * UUID_SIZE);
        return *this;
    }
    UniversalUniqueID& operator=(const unsigned char* other) {
        std::lock_guard lock(guard);
        memcpy(bytes, other, sizeof(bytes));
        return *this;
    }

    // UniversalUniqueID& operator=(const std::string &other) {
    //     Import(other);
    //     return *this;
    // }

    bool operator==(const UniversalUniqueID& other) const { return memcmp(bytes, other.bytes, sizeof(bytes)) == 0; }
    bool operator==(const unsigned char* p) const { return memcmp(bytes, p, sizeof(bytes)) == 0; }
    bool operator==(const std::string &p) const {
        UniversalUniqueID tmp;
        tmp.Import(p);
        return *this == tmp;
    }
    bool operator!=(const UniversalUniqueID& other) const { return !((*this) == other); }
    std::string operator()() const { return GetString(); }  // show in hyphenated format
    std::string Hex() const;                                // show in Hex
    std::string GetString() const;
    std::string GetAbbreviated() const { return FNVHash32(GetString()); }
    std::string GetLongAbbreviation() const { return FNVHash64(GetString()); }

    UniversalUniqueID& Copy(const unsigned char* b) {
        std::lock_guard lock(guard);
        memcpy(bytes, b, sizeof(bytes));
        return *this;
    }                                                            // copy from external binary uuid
    UniversalUniqueID& Set(const unsigned char* b, size_t len);  // fill internal data with x
    UniversalUniqueID& Copy(const char* b) { return Copy(reinterpret_cast<const unsigned char*>(b)); }
    UniversalUniqueID& Set(const char* b, size_t len) {
        return Set(reinterpret_cast<const unsigned char*>(b), len);
    }

    const unsigned char* GetData() const { return bytes; }
    static size_t size() { return UUID_SIZE * sizeof(unsigned char); }
    UniversalUniqueID& Clear() {
        std::lock_guard lock(guard);
        memset(bytes, 0, sizeof(bytes));
        return *this;
    }
    bool IsEmpty() const { return IsEmpty(static_cast<const void*>(bytes)); }
    bool empty() const { return IsEmpty(); }
    static bool IsEmpty(const void* p) { return memcmp(p, emptyUUID, sizeof(bytes)) == 0; }
    static bool empty(const void* p) { return IsEmpty(p); }
    UniversalUniqueID& Import(const std::string &b);  // import hyphenated string
    int Import(const char* b);                      // import hyphenated string
    int Export(void* p) const {
        memcpy(p, bytes, sizeof(bytes));
        return sizeof(bytes);
    }

public:
    UniversalUniqueID& Build();  // create new uuid in the private array
    static UniversalUniqueID Build(const std::string &b) {
        UniversalUniqueID uuid;
        return uuid.Import(b);
    }
    static void Create(unsigned char* b);  // this is to generate new uuid into *b
    static UniversalUniqueID Create() {
        UniversalUniqueID uuid;
        return uuid.Build();
    }
    static size_t Size() { return UUID_SIZE * sizeof(unsigned char); }
};

// this template enable UniversalUniqueID be used in std::unordered_set and std::unordered_map
namespace std {
    template<>
    struct hash<UniversalUniqueID> {
        std::size_t operator()(const UniversalUniqueID& k) const { return std::hash<std::string>()(std::string(k())); }
    };
}

std::string generateShortUUID();

extern UniversalUniqueID nullUUID;
extern UniversalUniqueID thisServerID;
extern UniversalUniqueID thisGroupID;
