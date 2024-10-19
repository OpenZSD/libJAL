#ifndef PARSING_H
#define PARSING_H

#include "Error.h"
#include "ParseElements.h"
#include <strExt/StrExt.h>
#include <vector>

namespace jal
{
    using namespace std;
    
    struct ParseToken
    {
        ParsePos from;
        ParsePos to;
        ext::StrExt token;
        bool isMultiline; //is this a string contained in triple quotes """<string>"""
        bool isAtomicParse; //this is purely a string with no further structure to parse such as a whole word, value, or multiline text
    };
    
    ext::StrExt readFile(const ext::StrExt &path);
    vector<ParseToken> splitToLines(const ext::StrExt &fullText);
    vector<ParseToken> splitOutMultilines(vector<ParseToken> &tokens);
    vector<ParseToken> splitOutQuoted(vector<ParseToken> &tokens);
}

#endif

