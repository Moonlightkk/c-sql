#ifndef C_SQL_U8STRING_H
#define C_SQL_U8STRING_H
#include <string>
#include <codecvt>
#include <istream>

using std::string;
using std::string_view;

static inline constexpr unsigned u8str_length = 256;
/* U8String - UTF8 encoded fixed length string (256 character)
 *
 */
class U8String
{
private:
    char str[u8str_length];
public:
    /* Constructor */
    U8String();
    /* Convert u8string to bytes */
    string toRawBytes();
    /* Read u8string from input stream */
    friend std::istream& operator>>(std::istream& istream, U8String u8string);
};

/* Read u8string from input stream */
std::istream& operator>>(std::istream& istream, U8String u8string);


#endif //C_SQL_U8STRING_H
