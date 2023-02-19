//
// Created by lizhi on 2023/2/6.
//
#include "public.h"
#include "ConditionFunctor.hpp"
#include "U8String.h"



bool utest()
{
    using namespace fop;
    FileIO::initFileSystem();
    FileIO fo;
    map<string ,bool> cls;
    cls["col"] = true;
    cls["aol"] = false;
    cls["bol"] = true;
    fo.create("db1");
    fo.use("db1");
    fo.create("tb1", cls, "col");
    fo.create("tb2", cls, "aol");
    fo.select("tb1");
    fo.select("tb2");
    fo.dropTable("tb1");
    return true;
}

bool testStr()
{
    U8String u8str;
    cin >> u8str;
    cout << u8str << endl;
    cout << u8str.toRawString()<<endl;
    cout << u8str.toRawString().size()<<endl;
    return true;
}


int main() {
    //start_terminal();
    /* do some test */
    utest();
    cout <<"2"<<endl;
    return 0;
}
