#ifndef STR_LIST_EXT_H
#define STR_LIST_EXT_H

#include <string>
#include <vector>

#include "StrExt.h"

namespace ext
{
    using namespace std;
    class StrExt;

    class StrListExt: public vector<string>
    {
    public:
        StrListExt() : vector<string>() {}
        StrListExt(const vector<string> &v) : vector<string>(v) {}
        StrListExt(initializer_list<string> il) : vector<string>(il) {}
        StrExt join(const string &joiner) const;    
        StrListExt operator+ (const vector<string>& list);
        StrListExt operator+ (const StrListExt& list) { return (*this) + ((vector<string>)list); }
    };
}

#endif
