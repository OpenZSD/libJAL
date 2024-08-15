#include "StrExt.h"
#include <cctype>
#include<iostream>

#define STRINGIFY_TOKEN_VALUE(x) #x
#define STRINGIFY(x) STRINGIFY_TOKEN_VALUE(x)

namespace ext
{
    string libStrExt_version() { return STRINGIFY(LIB_STR_EXT_VER); }

    StrExt::StrExt(float v, short dec)
    {
        if(dec >= 0)
        {
            char buff[512];
            StrExt format = StrExt("%.")+StrExt(dec)+StrExt("f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
        else
        {
            char buff[512];
            StrExt format = StrExt("%.64f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
    }
    StrExt::StrExt(double v, short dec)
    {
        if(dec >= 0)
        {
            char buff[512];
            StrExt format = StrExt("%.")+StrExt(dec)+StrExt("f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
        else
        {
            char buff[512];
            StrExt format = StrExt("%.128f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
    }
    StrExt::StrExt(long double v, short dec)
    {
        if(dec >= 0)
        {
            char buff[512];
            StrExt format = StrExt("%.")+StrExt(dec)+StrExt("f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
        else
        {
            char buff[512];
            StrExt format = StrExt("%.128f");
            sprintf(buff, format.c_str(), v);
            assign(buff);
        }
    }
    
    StrExt StrExt::operator+ (const string& str)
    {
        return StrExt(((string)*this)+(str));
    }

    StrExt StrExt::operator+ (const StrExt& str)
    {
        return StrExt(((string)*this)+((string)str));
    }

    vector<size_t> StrExt::findAll(const string& str) const
    {
        size_t lastPt{0};
        vector<size_t> list;
        if(!str.empty())
        {
            while(lastPt != npos)
            {
                lastPt = find(str, lastPt);
                if(lastPt != npos)
                {
                    list.push_back(lastPt);
                    lastPt+=str.size();
                }
            }
        }
        return list;
    }

    StrListExt StrExt::split(const string& str) const
    {
        if(empty()) { return {}; }
        else
        {
            vector<size_t> foundIdx = findAll(str);
            size_t from{0}, to{npos};
            StrListExt list;
            for(size_t idx : foundIdx)
            {
                to=idx;
                list.push_back(substr(from, to-from));
                from=to+str.length();
            }
            if(from < length())
            {
                to = length();
                list.push_back(substr(from, to-from));
            }

            return list;
        }
    }

    StrListExt StrExt::splitOnSpace() const
    {
        if(empty()) { return {}; }
        else
        {
            size_t from{npos};
            const char *raw = c_str();
            StrListExt list;
            for(size_t i = 0; i < length(); i++)
            {
                if(isspace(raw[i]))
                {
                    if(from != npos)
                    {
                        list.push_back(substr(from, i-from));
                        from = npos;
                    }
                }
                else if(from == npos) { from = i; }
            }

            if(from != npos)
            {
                list.push_back(substr(from, length()-from));
            }

            return list;
        }  
    }

    StrExt StrExt::toLower() const
    {
        StrExt s;
        s.reserve(size());
        for(char c : *this) { s+=std::tolower(c); }
        return s;
    }

    StrExt StrExt::toUpper() const
    {
        StrExt s;
        s.reserve(size());
        for(char c : *this) { s+=std::toupper(c); }
        return s;
    }

    bool StrExt::isNum() const
    {
        auto trimmed = trim();
        bool hasDec{false};

        if(trimmed.empty())
        {
            return false;
        }

        for(char c : trimmed)
        {
            if(!((c >= '0') && (c <= '9')))
            {
                if(c=='.' && !hasDec) { hasDec=true; }
                else
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool StrExt::isWord() const
    {
        auto trimmed = trim();
        if(trimmed.empty())
        {
            return false;
        }

        for(char c : trimmed)
        {
            if(isspace(c)) { return false; }
        }
        
        return true;
    }

    StrExt StrExt::trim() const
    {
        long int front{0};
        long int back{(long int) length()-1};
        const char *raw = c_str();
        while(front < back)
        {
            if(isspace(raw[front])) { front++; }
            else { break; }
        }
        while(front <= back) //check = for all spaces case (return empty)
        {
            if(isspace(raw[back])) { back--; }
            else { break; }
        }
        return substr(front,(back-front+1));
    }

    StrExt StrExt::trimFront() const
    {
        long int front{0};
        long int back{(long int) length()-1};
        const char *raw = c_str();
        while(front < back)
        {
            if(isspace(raw[front])) { front++; }
            else { break; }
        }
        while(front <= back) //check = for all spaces case (return empty)
        {
            if(isspace(raw[back])) { back--; }
            else { break; }
        }
        return substr(front,(back-front+1));
    }

    StrExt StrExt::trimBack() const
    {
        long int front{0};
        long int back{(long int) length()-1};
        const char *raw = c_str();
        while(front < back)
        {
            if(isspace(raw[front])) { front++; }
            else { break; }
        }
        while(front <= back) //check = for all spaces case (return empty)
        {
            if(isspace(raw[back])) { back--; }
            else { break; }
        }
        return substr(front,(back-front+1));
    }


    double StrExt::toDouble(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stold(str);
    }
    float StrExt::toFloat(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stold(str);
    }
    int StrExt::toInt(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stoi(str);
    }
    long StrExt::toLong(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stol(str);
    }
    long double StrExt::toLongDouble(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stold(str);
    }
    long long StrExt::toLongLong(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stoll(str);
    }
    unsigned long StrExt::toULong(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stoul(str);
    }
    unsigned long long StrExt::toULongLong(bool autoTrim) const
    {
        const StrExt &str = (autoTrim ? trim() : *this);
        return stoull(str);
    }
}
