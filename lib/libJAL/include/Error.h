#include <exception>
#include <utility>
#include <string>
#include "ParseElements.h"

#ifndef JAL_ERROR_H
#define JAL_ERROR_H

namespace jal
{
    using namespace std;

    enum class ErrorLevel: int
    {
        io=0,
        document=1,
        objectModel=2
    };
    
    enum class ErrorType: int
    {
        unset=-1,
        stringFormatting=0,
        improperEnclosure=1,
        valueParsing=2
    };

    class JalException : public exception
    {
    public:
        JalException(string err, ErrorLevel lvl, int type = to_underlying(ErrorType::unset), ParsePos pos = {0, 0});
        virtual const char* what() const noexcept override;
        ErrorLevel level() const noexcept { return mLevel; }
        int type() const noexcept { return mType; }
    private:
        ErrorLevel mLevel;
        int mType;
        string mErr;
        ParsePos mPos;
    };
}

#endif
