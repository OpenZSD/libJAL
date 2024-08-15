#ifndef PARSING_H
#define PARSING_H

#include "Error.h"
#include "ParseElements.h"
#include <strExt/StrExt.h>
#include <vector>

namespace jal
{
    using namespace std;
    
    vector<ParseToken> splitToLines(ext::StrExt &fullText);
    vector<ParseToken> splitOutMultilines(vector<ParseToken> &tokens);
    vector<ParseToken> splitOutQuoted(vector<ParseToken> &tokens);
}

#endif

