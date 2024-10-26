#ifndef PARSE_ELEMENTS_H
#define PARSE_ELEMENTS_H

namespace jal
{
    using namespace std;
    
    enum class ParsDelimiter: int
    {
        openBrace=0,
        closedBrace,
        memberCol,
        openList,
        closedList,
        listDel,
    };

    enum class ParseType: int
    {
        unparsed=0,
        delimiter,
        groupedMembers,
        list,
        valuePair,
        atomicValue
    };

    struct ParsePos
    {
        size_t line;
        size_t col;
    };
}

#endif
