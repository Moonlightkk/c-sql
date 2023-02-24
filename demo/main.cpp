//
// Created by lizhi on 2023/2/6.
//
#include "public.h"
#include "ConditionFunctor.hpp"
#include "U8String.h"



bool utest()
{
    map<string, string> testColNames;
    testColNames["acol"] = "int";
    testColNames["bcol"] = "int";
    testColNames["ccol"] = "string";
    map<string, string> testVals1;
    testVals1["acol"] = "1";
    testVals1["bcol"] = "1";
    testVals1["ccol"] = "hi";
    map<string, string> testVals2;
    testVals2["acol"] = "-1";
    testVals2["bcol"] = "-1";
    testVals2["ccol"] = "hi";
    using namespace fop;
    FileIO fo;
    fo.initFileSystem();
    fo.create("db1");
    fo.use("db1");
    fo.create("atable", testColNames);
    //fo.dropTable("atable");
//    fo.insert("atable", testVals1);
//    fo.insert("atable", testVals2);
    string cond = "ccol=hi";
    fo.select("atable", "acol", cond);
    cout << fo.dataName() << endl;
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
    cout <<"end test"<<endl;
    return 0;
}
