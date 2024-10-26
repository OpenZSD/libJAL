#include <iostream>
#include <jal/Error.h>
#include <jal/Parsing.h>
#include <set>
#include <unordered_set>
#include <map>
#include <deque>
#include <unordered_map>
#include <utility>

#define STRINGIFY_TOKEN_VALUE(x) #x
#define STRINGIFY(x) STRINGIFY_TOKEN_VALUE(x)

struct TestOutput
{
    std::string name;
    bool pass;
};
typedef TestOutput (*Test)();

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

bool match(const jal::JalException &e, jal::ErrorLevel lvl, int type)
{
    return ((e.level() == lvl) && (e.type() == type));
}

bool match(const jal::ParsePos &a, const jal::ParsePos &b)
{
    if(a.line != b.line)
    {
        std::cerr << "LINE MISMATCH: " << a.line << " <VS> " << b.line << " <END>\n";
        return false;
    }
    if(a.col != b.col)
    {
        std::cerr << "COL MISMATCH: " << a.col << " <VS> " << b.col << " @" << a.line << " <END>\n";
        return false;
    }
    return true;
}

bool match(const jal::ParseToken &a, const jal::ParseToken &b)
{
    if((!match(a.from, b.from))||(!match(a.to, b.to)))
    {
        return false;
    }
    if(a.token != b.token)
    {
        std::cerr << "MISMATCH:\n" << a.token << "\n<VS>\n" << b.token << "\n<END>\n";
        return false;
    }
    return true;
}

bool match(const jal::DelimPt &a, const jal::DelimPt &b)
{
    if(a.type != b.type)
    {
        std::cerr << "MISMATCH: <delim type> " << std::underlying_type_t<jal::ParsDelimiter>(a.type) << " <VS> " <<  std::underlying_type_t<jal::ParsDelimiter>(b.type) << "<END>\n";
        match(a.pt, b.pt);
        return false;
    }
    if(!match(a.pt, b.pt)) { return false; }
    return true;
}

std::string gTestRootPath;

TestOutput test1()
{
    bool healthy{true};
    int idx{0};
    
    chkExcp(healthy, idx, [&](){ jal::readFile("/fake/path.jal"); });
    
    return {"Bad file path check", healthy};
}

TestOutput test2()
{
    bool healthy{true};
    int idx{0};
    
    auto text = jal::readFile(gTestRootPath+"/in/line_cnt.jal").trim();
    chkTst(healthy, idx, [&](){ return jal::splitToLines(text).size() == 9; });
    text = std::string("\n\n{}");
    chkTst(healthy, idx, [&](){ return jal::splitToLines(text).size() == 3; });
    
    return {"Line count check", healthy};
}

TestOutput test3()
{
    bool healthy{true};
    
    auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/bad_str_1.jal"));
    try
    {
        jal::splitOutMultilines(lines);
    }
    catch(const jal::JalException &e)
    {
        healthy = match(e, jal::ErrorLevel::document, std::to_underlying(jal::ErrorType::stringFormatting));
    }
    catch(...)
    {
        healthy=false;
    }
    lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/line_cnt.jal"));
    try
    {
        jal::splitOutMultilines(lines);
    }
    catch(...)
    {
        healthy=false;
    }
    
    return {"Bad format check (string)", healthy};
}

TestOutput test4()
{
    bool healthy{true};
    int idx{0};
    std::deque<jal::ParseToken> keys {{{2,11},{4,0},"FIRST STRING WITH TRIMMED NEWLINE"},
                                     {{5,10},{5,23},"second string"},
                                     {{5,64},{6,18},"3rd as\nmultiline string~"},
                                     {{7,10},{7,13},"4th"},
                                     {{7,20},{7,23},"5th"},
                                     {{7,29},{8,8},"\n    6th"},
                                     {{8,16},{10,8},"\n    \n    7th"},
                                     {{11,11},{21,0},"10 lines of \"empty space\"\n\n\n\n\n\n\n\n\n\n"},
                                     {{22,12},{22,16},"~1"}};
    try
    {
        auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/str.jal"));
        auto tokens = jal::splitOutMultilines(lines);
        for(auto &token : tokens)
        {
            if(token.isMultiline)
            {
                chkTst(healthy, idx, [&](){ return match(token, keys.front()); });
                keys.pop_front();
            }
            if(!healthy) { break; }
        }
    }
    catch(...)
    {
        healthy=false;
    }
    
    return {"Multi-line parse check", healthy};
}

TestOutput test5()
{
    bool healthy{true};
    int idx{0};
    std::deque<jal::ParseToken> keys {{{5,28},{5,58},"~first basic enclosed string~"},
                                     {{22,5},{22,6},"f"}};
    try
    {
        auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/str.jal"));
        auto tokens = jal::splitOutMultilines(lines);
        tokens = jal::splitOutQuoted(tokens);
        for(auto &token : tokens)
        {
            if(token.isAtomicParse && !token.isMultiline)
            {
                chkTst(healthy, idx, [&](){ return match(token, keys.front()); });
                keys.pop_front();
            }
            if(!healthy) { break; }
        }
    }
    catch(...)
    {
        healthy=false;
    }
    
    return {"Single quote parse check", healthy};
}

TestOutput test6()
{
    bool healthy{true};
    int idx{0};
    std::deque<jal::DelimPt> keys {{jal::ParsDelimiter::openBrace,{0,0}},
                                   {jal::ParsDelimiter::memberCol,{1,11}},
                                   {jal::ParsDelimiter::memberCol,{2,5}},
                                   {jal::ParsDelimiter::memberCol,{5,5}},
                                   {jal::ParsDelimiter::memberCol,{7,5}},
                                   {jal::ParsDelimiter::memberCol,{11,9}},
                                   {jal::ParsDelimiter::openList,{11,11}},
                                   {jal::ParsDelimiter::listDel,{11,13}},
                                   {jal::ParsDelimiter::listDel,{11,15}},
                                   {jal::ParsDelimiter::listDel,{11,19}},
                                   {jal::ParsDelimiter::closedList,{11,25}},
                                   {jal::ParsDelimiter::memberCol,{12,7}},
                                   {jal::ParsDelimiter::openBrace,{12,9}},
                                   {jal::ParsDelimiter::closedBrace,{12,10}},
                                   {jal::ParsDelimiter::memberCol,{13,11}},
                                   {jal::ParsDelimiter::openBrace,{13,13}},
                                   {jal::ParsDelimiter::memberCol,{13,16}},
                                   {jal::ParsDelimiter::closedBrace,{13,21}},
                                   {jal::ParsDelimiter::memberCol,{14,6}},
                                   {jal::ParsDelimiter::memberCol,{25,7}},
                                   {jal::ParsDelimiter::closedBrace,{26,0}}};
    try
    {
        auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/str2.jal"));
        auto tokens = jal::splitOutMultilines(lines);
        tokens = jal::splitOutQuoted(tokens);
        auto varTokens = jal::splitOutDelimiters(tokens);
        for(const auto &vt : varTokens)
        {
            if(vt.type == jal::ParseType::delimiter)
            {
                chkTst(healthy, idx, [&](){ return match(std::get<jal::DelimPt>(vt.value), keys.front()); });
                keys.pop_front();
            }
        }
    }
    catch(...)
    {
        healthy=false;
    }
    
    return {"Delimiters check", healthy};
}

int main(int argc, char *argv[]){
    gTestRootPath = argv[1];

    std::cout << "======START OF TESTS [StrExt]======" << std::endl;
    Test tests[] = {test1, test2, test3, test4, test5, test6};
    int testCount = 6;
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
