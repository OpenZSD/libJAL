#include <iostream>
#include <jal/Error.h>
#include <jal/Parsing.h>
#include <set>
#include <unordered_set>
#include <map>
#include <deque>
#include <unordered_map>
#include <utility>
#include <tuple>

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
        std::cerr << "MISMATCH: <delim type> " << std::underlying_type_t<jal::ParsDelimiter>(a.type)
                  << " <VS> " <<  std::underlying_type_t<jal::ParsDelimiter>(b.type) << "<END>\n";
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
                                     {{5,66},{6,18},"3rd as\nmultiline string~"},
                                     {{7,10},{7,13},"4th"},
                                     {{7,21},{7,24},"5th"},
                                     {{7,31},{8,8},"\n    6th"},
                                     {{8,17},{10,8},"\n    \n    7th"},
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
    std::deque<jal::ParseToken> keys {{{5,29},{5,59},"~first basic enclosed string~"},
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
                                   {jal::ParsDelimiter::listDel,{1,17}},
                                   {jal::ParsDelimiter::memberCol,{2,5}},
                                   {jal::ParsDelimiter::listDel,{4,4}},
                                   {jal::ParsDelimiter::memberCol,{5,5}},
                                   {jal::ParsDelimiter::listDel,{5,26}},
                                   {jal::ParsDelimiter::listDel,{5,63}},
                                   {jal::ParsDelimiter::listDel,{6,19}},
                                   {jal::ParsDelimiter::memberCol,{7,5}},
                                   {jal::ParsDelimiter::listDel,{7,16}},
                                   {jal::ParsDelimiter::listDel,{7,27}},
                                   {jal::ParsDelimiter::listDel,{8,12}},
                                   {jal::ParsDelimiter::listDel,{10,12}},
                                   {jal::ParsDelimiter::memberCol,{11,9}},
                                   {jal::ParsDelimiter::openList,{11,11}},
                                   {jal::ParsDelimiter::listDel,{11,13}},
                                   {jal::ParsDelimiter::listDel,{11,15}},
                                   {jal::ParsDelimiter::listDel,{11,19}},
                                   {jal::ParsDelimiter::closedList,{11,25}},
                                   {jal::ParsDelimiter::listDel,{11,26}},
                                   {jal::ParsDelimiter::memberCol,{12,7}},
                                   {jal::ParsDelimiter::openBrace,{12,9}},
                                   {jal::ParsDelimiter::closedBrace,{12,10}},
                                   {jal::ParsDelimiter::listDel,{12,11}},
                                   {jal::ParsDelimiter::memberCol,{13,11}},
                                   {jal::ParsDelimiter::openBrace,{13,13}},
                                   {jal::ParsDelimiter::memberCol,{13,16}},
                                   {jal::ParsDelimiter::closedBrace,{13,21}},
                                   {jal::ParsDelimiter::listDel,{13,22}},
                                   {jal::ParsDelimiter::memberCol,{14,6}},
                                   {jal::ParsDelimiter::listDel,{24,3}},
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

TestOutput test7()
{
    bool healthy{true};
    int idx{0};
    
    std::vector<jal::ParseToken> input = {{{0,0},{0,4},"1 2 3",false,false},
                                          {{1,2},{1,6},"1 2 3",false,false},
                                          {{0,0},{0,3},"1 22",false,false},
                                          {{1,2},{1,6},"22  1",false,false},
                                          {{0,0},{0,10},"1  22   333",false,false},
                                          {{0,0},{0,10},"333  22   1",false,false}};
    std::vector<std::vector<jal::ParseToken>> keys{{{{0,0},{0,0},"1",false,true},{{0,2},{0,2},"2",false,true},{{0,4},{0,4},"3",false,true}},
                                                   {{{1,2},{1,2},"1",false,true},{{1,4},{1,4},"2",false,true},{{1,6},{1,6},"3",false,true}},
                                                   {{{0,0},{0,0},"1",false,true},{{0,2},{0,3},"22",false,true}},
                                                   {{{1,2},{1,3},"22",false,true},{{1,6},{1,6},"1",false,true}},
                                                   {{{0,0},{0,0},"1",false,true},{{0,3},{0,4},"22",false,true},{{0,8},{0,10},"333",false,true}},
                                                   {{{0,0},{0,2},"333",false,true},{{0,5},{0,6},"22",false,true},{{0,10},{0,10},"1",false,true}}};
                                               
    while((idx < input.size()) && healthy)
    {
        chkTst(healthy, idx, [&]()
        {
            auto output = jal::splitOnGaps(input[idx]);
            for(int i=0; ((healthy) && (i < keys[idx].size())); i++)
            {
                healthy = match(output[i], keys[idx][i]);
            }
            return healthy;
        });
    }                                   
    if(!healthy) { std::cerr << "(test7) Err: failed comparison #" << idx << std::endl; }

    return {"Gap split check", healthy};
}

TestOutput test8()
{
    bool healthy{true};
    int idx{0};
    
    std::vector<std::tuple<size_t, jal::ParseToken>> keys
     = {{1,{{1,4},{1,10},"memberA", false, true}}, {3,{{1,13},{1,16},"test", false, true}}, {5,{{2,4},{2,4},"B", false, true}}, {9,{{5,4},{5,4},"c", false, true}},
        {17,{{7,4},{7,4},"d", false, true}}, {27,{{11,4},{11,8},"array", false, true}}};
    
    try
    {
        auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/str2.jal"));
        auto tokens = jal::splitOutMultilines(lines);
        tokens = jal::splitOutQuoted(tokens);
        auto varTokens = jal::splitOutDelimiters(tokens);
        varTokens = trimAndSplitUnparsed(varTokens);
        
        for(auto & [pos, keyVal] : keys)
        {
            chkTst(healthy, idx, [&](){ return (varTokens[pos].type == jal::ParseType::atomicValue); });
            if(!healthy) { break; }
            chkTst(healthy, idx, [&]()
            {
                auto &value = std::get<jal::ParseToken>(varTokens[pos].value);
                return match(value, keyVal);
            });
            if(!healthy) { break; }
        }
    }
    catch(...)
    {
        healthy=false;
    } 
        
    return {"White space clearing check", healthy};
}

TestOutput test9()
{
    bool healthy{true};
    int idx{0};
    
    
    try
    {
        auto lines = jal::splitToLines(jal::readFile(gTestRootPath+"/in/str2.jal"));
        auto tokens = jal::splitOutMultilines(lines);
        tokens = jal::splitOutQuoted(tokens);
        auto varTokens = jal::splitOutDelimiters(tokens);
        varTokens = trimAndSplitUnparsed(varTokens);
        auto root = joinMembers(varTokens);
      
        std::vector<std::tuple<size_t, jal::ParseType, size_t>> nestedGroups = {{36, jal::ParseType::groupedMembers, 3},
                                                                                {32, jal::ParseType::groupedMembers, 0},
                                                                                {28, jal::ParseType::list, 7}};
        chkTst(healthy, idx, [&]()
        {
            if(root.type == jal::ParseType::groupedMembers)
            {
                auto &rootVec = std::get<std::vector<jal::VarToken>>(root.value);
                if(rootVec.size() == 45)
                {

                    for(auto const &[vidx, vtype, vsize] : nestedGroups)
                    {
                        if(rootVec[vidx].type == vtype)
                        {
                            if(vsize != std::get<std::vector<jal::VarToken>>(rootVec[vidx].value).size())
                            {
                                return false;
                            }
                        }
                        else
                        {
                            std::cerr << "NOT DESIRED TYPE:" << std::to_underlying(vtype)
                                                             << " GOT:" << std::to_underlying(rootVec[vidx].type) << "\n";
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        });
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        healthy=false;
    } 
        
    return {"Value Grouping", healthy};
}

int main(int argc, char *argv[]){
    gTestRootPath = argv[1];

    std::cout << "======START OF TESTS [StrExt]======" << std::endl;
    Test tests[] = {test1, test2, test3, test4, test5, test6, test7, test8, test9};
    int testCount = 9;
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
