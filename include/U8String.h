#ifndef C_SQL_U8STRING_H
#define C_SQL_U8STRING_H
#include <string>
#include <codecvt>
#include <istream>
#include <ostream>
#include <cstring>
#include <cctype>

#ifdef _WIN32
#elif defined(__linux__)
#endif
using std::string;
using std::wstring;
using std::string_view;

static constexpr unsigned u8str_length = 256;
static constexpr unsigned buffer_size = 1024;

struct InvalidU8StrngFromStream : public std::exception
{
    explicit InvalidU8StrngFromStream(const char* msg);
    [[nodiscard]] const char* what() const noexcept override;
private:
    char _msg[buffer_size]{};
};

/* U8String - UTF8 encoded fixed length string (256 character)
 *
 */
class U8String
{
private:
    string buffer;
    long long end;
    //void convertFromLocale (wstring localeStr);
public:
    /* Constructor */
    U8String();
    explicit U8String(const string& str);
    /* Conver string to u8string */
    void fromString(const string& str);
    /* Convert u8string to null-terminated string */
    [[nodiscard]] const char* toRawChars()const;
    [[nodiscard]] string toRawString()const;
    explicit operator string()const;
    /* Read u8string from input stream */
    friend std::istream& operator>>(std::istream& istream, U8String& u8string);
    /* Write u8string to output stream */
    friend std::ostream& operator<<(std::ostream& ostream, const U8String& u8String);
};

/* Stream Operations */
/* Read 256 char from stream, null(0x0) filled */
//std::istream& operator>>(std::istream& istream, U8String& u8string);
//std::ostream& operator<<(std::ostream& ostream, U8String u8String);

#endif //C_SQL_U8STRING_H
