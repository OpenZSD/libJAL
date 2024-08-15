#ifndef STR_EXT_H
#define STR_EXT_H

#include <string>
#include "StrListExt.h"

namespace ext
{
    using namespace std;
    class StrListExt;

    string libStrExt_version();

    class StrExt: public string
    {
    public:
        StrExt() {}
        StrExt(const StrExt& str): string(str) {}
        StrExt(const string& str): string(str) {}
        StrExt(const char* s): string(s) {}

        StrExt(int v): string(to_string(v)) {}
        StrExt(long v): string(to_string(v)) {}
        StrExt(long long v): string(to_string(v)) {}
        StrExt(unsigned long v): string(to_string(v)) {}
        StrExt(unsigned long long v): string(to_string(v)) {}
        StrExt(float v, short dec=-1);
        StrExt(double v, short dec=-1);
        StrExt(long double v, short dec=-1);

        StrExt& operator= (const string& str) { assign(str); return *this; }
        StrExt& operator= (const StrExt& str) { assign(str); return *this; }

        StrExt operator+ (const string& str);
        StrExt operator+ (const StrExt& str);

        vector<size_t> findAll(const string& str) const;
        StrListExt split(const string& str) const;
        StrListExt splitOnSpace() const;

        StrExt toLower() const;
        StrExt toUpper() const;

        StrExt trim() const;
        StrExt trimFront() const;
        StrExt trimBack() const;
        bool isNum() const;
        bool isWord() const;
        double toDouble(bool autoTrim=false) const;
        float toFloat(bool autoTrim=false) const;
        int toInt(bool autoTrim=false) const;
        long toLong(bool autoTrim=false) const;
        long double toLongDouble(bool autoTrim=false) const;
        long long toLongLong(bool autoTrim=false) const;
        unsigned long toULong(bool autoTrim=false) const;
        unsigned long long toULongLong(bool autoTrim=false) const;
    };
}

#endif
