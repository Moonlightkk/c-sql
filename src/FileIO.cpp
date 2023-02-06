//
// Created by Wu Mianzhi on 2023/2/5.
//
#include "../include/FileIO.h"

vector<unsigned char> FileIO::i32ToBytes(int32_t val)
{
    vector<unsigned char> buffer;
    auto bitMode = static_cast<std::uint32_t> (val);
    buffer.push_back(static_cast< unsigned char>(bitMode >> 24));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 16));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 8));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 0));
    return buffer;
}

int32_t FileIO::bytesToI32(const vector<unsigned char> &bytes)
{
    int32_t buf = 0;
    buf = buf + (static_cast<int32_t>(bytes[0]) << 24);
    buf = buf + (static_cast<int32_t>(bytes[1]) << 16);
    buf = buf + (static_cast<int32_t>(bytes[2]) << 8);
    buf = buf + (static_cast<int32_t>(bytes[3]) << 0);
    return buf;
}

FileIO::FileIO()
{
    rowOffset = 0;
    isValid = false;
}

FileIO::FileIO(const string& fileName)
{
    rowOffset = 0;
    isValid = false;
    fp.open(fileName, ios_base::in | ios_base::out | ios_base::binary);
    if (fp.is_open())
    {
        isValid = true;
    }
}

string FileIO::readPos(unsigned int offset)
{
    string res;
    fp.seekg(offset);

    return res;
}
