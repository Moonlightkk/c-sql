//
// Created by lizhi on 2023/2/6.
//
#include "public.h"
#include "ConditionFunctor.hpp"



bool utest()
{
    using namespace fop;
    FileIO::initFileSystem();
    FileIO fo;
    map<string ,bool> cls;
    cls["col"] = true;
    cls["aol"] = false;
    cls["bol"] = true;
    fo.use("db1");
    fo.create("tb1", cls);
    fo.select("tb1");
    return true;
}

int main() {
    //start_terminal();
    /* do some test */
    utest();
    cout <<"2"<<endl;
    return 0;
}
