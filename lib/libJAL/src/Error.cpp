#include "Error.h"

namespace jal
{
    JalException::JalException(string err, ErrorLevel lvl, int type, ParsePos pos) : 
        mErr(err),
        mLevel(lvl),
        mType(type),
        mPos(pos) {}

    const char* JalException::what() const noexcept
    {
        return mErr.c_str();
    }
}
