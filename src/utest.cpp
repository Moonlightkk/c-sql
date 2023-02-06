//
// Created by Wu Mianzhi on 2023/2/2.
//
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>
#include "../include/FileIO.h"
using namespace std;

int testFileIO()
{
    int32_t a = 0;
    cout<<a<<endl;
    vector<unsigned char> buf;
    buf = FileIO::i32ToBytes(a);
    string str(buf.begin(), buf.end());
    vector<unsigned char> buf2(str.begin(), str.end());
    cout<<FileIO::bytesToI32(buf2)<<endl;
    //cout<<static_cast<char>(321)<<endl;
    return 0;
}