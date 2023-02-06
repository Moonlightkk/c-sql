//
// Created by Wu Mianzhi on 2023/2/2.
//

#include "../include/U8String.h"

U8String::U8String()
{
    str.resize(u8str_length);
}

char* U8String::toRawBytes()
{
    return str.data();
}

void U8String::convertFromLocale(wstring localeStr)
{

}

std::istream& operator>>(std::istream& istream, U8String u8string)
{
    ;
}