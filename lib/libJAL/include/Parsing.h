#ifndef PARSING_H
#define PARSING_H

#include "Error.h"
#include "ParseElements.h"
#include <strExt/StrExt.h>
#include <vector>
#include <variant>
#include <map>

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
    
    struct DelimPt
    {
        ParsDelimiter type;
        ParsePos pt;
    };
    
    struct VarToken
    {
        ParseType type;
        variant<ParseToken, DelimPt, vector<VarToken>, map<ext::StrExt, VarToken>> value;
    };
    
    
    ext::StrExt readFile(const ext::StrExt &path);
    vector<ParseToken> splitToLines(const ext::StrExt &fullText);
    vector<ParseToken> splitOutMultilines(vector<ParseToken> &tokens);
    vector<ParseToken> splitOutQuoted(vector<ParseToken> &tokens);
    vector<VarToken> splitOutDelimiters(vector<ParseToken> &tokens);
    /*vector<VarToken> joinMembers();
    vector<VarToken> joinCollections();
    void produceDocument();*/
}

#endif

