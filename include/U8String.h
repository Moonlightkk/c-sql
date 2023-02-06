#ifndef C_SQL_U8STRING_H
#define C_SQL_U8STRING_H
#include <string>
#include <codecvt>
#include <istream>
#include <fstream>
#include <ostream>

#ifdef _WIN32
#elif defined(__linux__)
#endif
using std::string;
using std::wstring;
using std::string_view;

static inline constexpr unsigned u8str_length = 256;
/* U8String - UTF8 encoded fixed length string (256 character)
 *
 */
class U8String
{
private:
    string str;
    void convertFromLocale (wstring localeStr);
public:
    /* Constructor */
    U8String();
    /* Convert u8string to bytes */
    char* toRawBytes();
    /* Read u8string from input stream */
    friend std::istream& operator>>(std::istream& istream, U8String u8string);
    friend std::ostream& operator<<(std::ostream& ostream, U8String u8String);
};

/* Stream Operations */
std::istream& operator>>(std::istream& istream, U8String u8string);
std::ostream& operator<<(std::ostream& ostream, U8String u8String);

#endif //C_SQL_U8STRING_H
