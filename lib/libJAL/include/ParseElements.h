#include<strExt/StrExt.h>

#ifndef PARSE_ELEMENTS_H
#define PARSE_ELEMENTS_H

namespace jal
{
    using namespace std;

    struct ParsePos
    {
        size_t line;
        size_t col;
    };
    struct ParseToken
    {
        ParsePos from;
        ParsePos to;
        ext::StrExt token;
        bool isMultiline; //is this a string contained in triple quotes """<string>"""
        bool isAtomicParse; //this is purely a string with no further structure to parse such as a whole word, value, or multiline text
    };
}

#endif
