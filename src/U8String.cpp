//
// Created by Wu Mianzhi on 2023/2/2.
//

#include "../include/U8String.h"

InvalidU8StrngFromStream::InvalidU8StrngFromStream(const char *msg)
{
    strcpy_s(_msg, buffer_size, msg);
}

const char *InvalidU8StrngFromStream::what() const noexcept
{
    return _msg;
}

U8String::U8String()
{
    buffer.resize(u8str_length);
    end = 0;
}

U8String::U8String(const string &str)
{
    buffer.resize(u8str_length);
    end = 0;
    fromString(str);
}

void U8String::fromString(const string &str) {
    buffer.assign(u8str_length, 0);
    auto u8end =  std::copy(str.begin(), str.end(), buffer.begin());
    end = u8end - buffer.begin();
}

const char* U8String::toRawChars() const
{
    return buffer.data();
}

U8String::operator string() const
{
    return buffer;
}

string U8String::toRawString()const {
    return buffer;
}

std::istream& operator>>(std::istream& istream, U8String &u8string)
{
    string str;
    while (!isspace(istream.peek()))
    {
        char ch = istream.get();
        str.push_back(ch);
    }
    u8string.fromString(str);
    return istream;
}


std::ostream& operator<<(std::ostream& ostream, const U8String& u8string)
{
    string result;
    result.resize(u8string.end);
    std::copy(u8string.buffer.begin(), u8string.buffer.begin() + u8string.end, result.begin());
    ostream << result;
    return ostream;
}