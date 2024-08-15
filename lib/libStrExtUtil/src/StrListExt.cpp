#include "StrListExt.h"

namespace ext
{
    StrExt StrListExt::join(const string &joiner) const
    {
        long joinSize{0};
        for(const auto &str : *this)
        {
            joinSize+=str.size();
        }
        if(size()>0)
        {
            joinSize+=((size()-1)*joiner.size());
        }
        std::string joined;
        joined.reserve(joinSize+1);
        for(int i=0; i < size(); i++)
        {
            joined.append(at(i));
            if(i != size()-1)
            {
                joined.append(joiner);
            }
        }
        return joined;
    }

    StrListExt StrListExt::operator+ (const vector<string>& list)
    {
        StrListExt out;
        out.reserve(size()+list.size());
        for(auto &str : (*this))
        {
            out.push_back(str);
        }
        for(auto &str : list)
        {
            out.push_back(str);
        }
        return out; 
    }
}
