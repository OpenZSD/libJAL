#include<iostream>
#include<strExt/StrExt.h>
#include<limits>

struct TestOutput
{
    std::string name;
    bool pass;
};
typedef TestOutput (*Test)();

#define STRINGIFY_TOKEN_VALUE(x) #x
#define STRINGIFY(x) STRINGIFY_TOKEN_VALUE(x)

//normal test
#define chkTst(healthy, idx, fnc) \
if(healthy) \
{ \
    healthy &= fnc(); \
    if(healthy) { idx++; } \
}
//make sure it throws an exception
#define chkExcp(healthy, idx, fnc) \
if(healthy) \
{ \
    bool thrown{false}; \
    try { fnc(); } \
    catch(...) { thrown=true; }\
    healthy=thrown; \
    if(healthy) { idx++; } \
}

TestOutput test1()
{
    ext::StrExt extString = ext::StrExt("message");
    std::string stdString{"message"};
    ext::StrExt fromStdString = stdString;
    bool healthy{true};
    int idx{0};
    chkTst(healthy, idx, [&](){ return (extString==stdString); });
    chkTst(healthy, idx, [&](){ return (extString==fromStdString); });
    chkTst(healthy, idx, [&](){ return (stdString==fromStdString); });
    chkTst(healthy, idx, [&](){ return (extString!=""); });
    chkTst(healthy, idx, [&](){ return (extString!="Message"); });
    chkTst(healthy, idx, [&](){ return (extString=="message"); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt()==""); });
    if(!healthy) { std::cerr << "(test1) Err: failed comparison #" << idx << std::endl; }
   
    return {"StrExt/std::string compare",healthy};
}

TestOutput test2()
{
    bool healthy{true};
    int idx{0};
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t \t\r\n").trim()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t A \t\r\n").trim()=="A"); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t a A \t\r\n").trim()=="a A"); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t a A \t\r\n").trimFront()=="a A \t\r\n"); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t a A \t\r\n").trimBack()==" \n\n\t a A"); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t \t\r\n").trimFront()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \n\n\t \t\r\n").trimBack()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("").trimFront()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("").trimBack()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("1").trimFront()==std::string("1")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("1").trimBack()==std::string("1")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" ").trimFront()==std::string()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" ").trimBack()==std::string()); });
    if(!healthy) { std::cerr << "(test2) Err: failed trim test #" << idx << std::endl; }
    return {"String trim check",healthy};
}

TestOutput test3()
{
    bool healthy{true};
    int idx{0};
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0").toInt()==0); });

    chkExcp(healthy, idx, [&](){ return (ext::StrExt("").toInt()==0); });

    chkTst(healthy, idx, [&](){ return (ext::StrExt(-1).toInt()==-1); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" -1").toInt()==-1); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" 10 ").toInt()==10); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" 10.01 ").toInt()==10); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("\t \n 10.9 \t").toInt()==10); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("\t \n 10.9 \t").toInt(true)==10); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0 0 1").toInt()==0); })
    chkTst(healthy, idx, [&](){ return (ext::StrExt("2 abc").toInt()==2); })
    chkTst(healthy, idx, [&](){ return (ext::StrExt("2abc").toInt()==2); })

    chkExcp(healthy, idx, [&](){ return (ext::StrExt("abc 2").toInt()==2); })

    chkTst(healthy, idx, [&](){ return (ext::StrExt(10.01).toInt()==10); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.000000477).toDouble()==0.000000477); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.000000477, 2).toDouble()==0.0); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.010000477, 2).toDouble()==0.01); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.01, 1).toDouble()==0.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0.").toDouble()==0.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("00.").toDouble()==0.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(".0").toDouble()==0.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0.").toInt()==0); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("00.").toInt()==0); });

    chkExcp(healthy, idx, [&](){ return (ext::StrExt(".0").toInt()==0); });

    chkTst(healthy, idx, [&](){ return (ext::StrExt(5).toFloat()==5.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.00000000000000000000000000000000000000000000000000000000000000000000001).toFloat()==0.00); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0.00000000000000000000000000000000000000000000000000000000000000000000001).toDouble()
                                                 == 0.00000000000000000000000000000000000000000000000000000000000000000000001); });
    unsigned long long largeInt {18446744073709551615};
    chkTst(healthy, idx, [&](){ return (ext::StrExt(largeInt).toULongLong()
                                                 == largeInt); });
    if(!healthy) { std::cerr << "(test3) Err: failed num test #" << idx << std::endl; }
    return {"To num check",healthy};
}

TestOutput test4()
{
    bool healthy{true};
    int idx{0};

    chkTst(healthy, idx, [&](){ return (ext::StrExt(10.01).isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(10).isNum()==true); }); 
    chkTst(healthy, idx, [&](){ return (ext::StrExt().isNum()==false); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0").isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(0).isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0..0").isNum()==false); }); 
    chkTst(healthy, idx, [&](){ return (ext::StrExt("..0").isNum()==false); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0..").isNum()==false); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(".0").isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("0.").isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("00.").isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("\t \n 00.").isNum()==true); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("\t \n 00. 0").isNum()==false); });

    if(!healthy) { std::cerr << "(test4) Err: failed isNum test #" << idx << std::endl; }
    return {"isNum() check",healthy};
}

TestOutput test5()
{
    bool healthy{true};
    int idx{0};

    ext::StrExt a{"\t\t \t\t \n\t,1,2,3\t"}, b{}, c{","}, d{",a"}, e{"\t\t \t\t\t\t \t\t\t"};
    std::vector<size_t> k1{0,1,3,4,7,14}, k2{}, k3{0}, k4{1}, k5{0,3,5,8};

    chkTst(healthy, idx, [&](){ return (a.findAll("\t")==k1); });
    chkTst(healthy, idx, [&](){ return (a.findAll("N")==k2); });
    chkTst(healthy, idx, [&](){ return (b.findAll("\t")==k2); });
    chkTst(healthy, idx, [&](){ return (c.findAll("\t")==k2); });
    chkTst(healthy, idx, [&](){ return (c.findAll(",")==k3); });
    chkTst(healthy, idx, [&](){ return (d.findAll(",")==k3); });
    chkTst(healthy, idx, [&](){ return (d.findAll("a")==k4); });
    chkTst(healthy, idx, [&](){ return (d.findAll("")==k2); });
    chkTst(healthy, idx, [&](){ return (e.findAll("\t\t")==k5); });

    if(!healthy) { std::cerr << "(test5) Err: failed find test #" << idx << std::endl; }
    return {"findAll() check",healthy};
}

TestOutput test6()
{
    bool healthy{true};
    int idx{0};

    ext::StrListExt listA{"A","B","C"}, listB{"","",""};
    chkTst(healthy, idx, [&](){ return (listA.join(",") == "A,B,C"); });
    chkTst(healthy, idx, [&](){ return (listB.join(",") == ",,"); });
    chkTst(healthy, idx, [&](){ return (ext::StrListExt().join(",") == ""); });
    chkTst(healthy, idx, [&](){ return (ext::StrListExt({"A"}).join(",") == "A"); });
    chkTst(healthy, idx, [&](){ return (ext::StrListExt({"A","d"}).join("bC") == "AbCd"); });
    if(!healthy) { std::cerr << "(test6) Err: failed string list test #" << idx << std::endl; }
    return {"String list check",healthy};
}

TestOutput test7()
{
    bool healthy{true};
    int idx{0};

    chkTst(healthy, idx, [&](){ return (ext::StrExt().toLower()==ext::StrExt()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt().toUpper()==ext::StrExt()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("aBc").toUpper()==std::string("ABC")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("aBc").toLower()==std::string("abc")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\n  aBc \na").toLower()==std::string(" \t\n  abc \na")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\n   \n").toUpper()==std::string(" \t\n   \n")); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\n   \na").toUpper()==" \t\n   \nA"); });

    if(!healthy) { std::cerr << "(test7) Err: failed to upper/lower test #" << idx << std::endl; }
    return {"To upper/lower check",healthy};
}

TestOutput test8()
{
    bool healthy{true};
    int idx{0};

    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\n   \na").split(",") == ext::StrListExt({" \t\n   \na"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("").split(",") == ext::StrListExt()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("").splitOnSpace() == ext::StrListExt()); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\n   \na").splitOnSpace() == ext::StrListExt({"a"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("a \t\n   \n").splitOnSpace() == ext::StrListExt({"a"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(" \t\na   \n").splitOnSpace() == ext::StrListExt({"a"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("a \t\nb   \nc").splitOnSpace() == ext::StrListExt({"a","b","c"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("a,b,c").split(",") == ext::StrListExt({"a","b","c"})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt(",").split(",") == ext::StrListExt({""})); });
    chkTst(healthy, idx, [&](){ return (ext::StrExt("      ").splitOnSpace() == ext::StrListExt({})); });

    if(!healthy) { std::cerr << "(test8) Err: failed split test #" << idx << std::endl; }
    return {"Split check",healthy};
}

TestOutput test9()
{
    bool healthy{true};
    int idx{0};

    chkTst(healthy, idx, [&](){ return (ext::libStrExt_version()==STRINGIFY(TEST_LIB_STR_EXT_VER)); });

    if(!healthy) { std::cerr << "(test9) Err: version mismatch " << ext::libStrExt_version() << " VS " << STRINGIFY(TEST_LIB_STR_EXT_VER) << std::endl; }
    return {"Version check",healthy};
}

TestOutput test10()
{
    bool healthy{true};
    int idx{0};
    auto a = ext::StrExt("1");
    auto b = std::string("2");
    a+=b;
    chkTst(healthy, idx, [&](){ return ((ext::StrExt("1")+ext::StrExt("2")) == ext::StrExt("12")); });
    chkTst(healthy, idx, [&](){ return ((ext::StrExt("1")+std::string("2")) == ext::StrExt("12")); });
    chkTst(healthy, idx, [&](){ return ((std::string("1")+ext::StrExt("2")) == ext::StrExt("12")); });
    chkTst(healthy, idx, [&](){ return ((ext::StrExt("1")+ext::StrExt("2")).findAll("2") == std::vector<size_t>({1})); });
    chkTst(healthy, idx, [&](){ return ((ext::StrExt("1")+ext::StrExt("2")).toInt() == 12); });
    chkTst(healthy, idx, [&](){ return (a.toInt() == 12); });
    chkTst(healthy, idx, [&](){ return (ext::StrListExt({"a","b","c"})+ext::StrListExt()+ext::StrListExt({"d","e","f"}) 
                                        == ext::StrListExt({"a","b","c","d","e","f"}));  });
    if(!healthy) { std::cerr << "(test10) Err: failed (+) test #" << idx << std::endl; }
    return {"(+) check",healthy};
}

int main(int argc, char *argv[])
{
    std::cout << "======START OF TESTS [StrExt]======" << std::endl;
    Test tests[] = {test1, test2, test3, test4, test5, test6, test7, test8, test9, test10};
    int testCount = 10;
    bool healthy {true};

    for(int i=0; i<testCount; i++)
    {
        auto res = tests[i]();
        healthy &= res.pass;

        std::cout << "[" << (res.pass ? "PASS" : "FAILED") << "] " << res.name << std::endl;

        if(!healthy) { break; }
    }

    return healthy ? 0 : 1;
}
