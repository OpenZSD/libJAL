#include "Parsing.h"
#include <set>
#include <optional>

#define ML_TOK "\"\"\""
#define ML_TOK_FTRIM "\\\"\"\""
#define ML_TOK_BTRIM "\"\"\"\\"

std::set<size_t> _vecToSet(std::vector<size_t> &&vec)
{
    return std::set<size_t>(vec.begin(), vec.end());
}

std::set<size_t> _findEscaped(const ext::StrExt &line)
{
    auto escList = line.findAll("~");
    std::set<size_t> escaped;
    for(auto escPt : escList)
    {
        if(escaped.find(escPt) != escaped.end())
        {
            escaped.insert(escPt+1); //next index is a candidate
        }
    }
    return escaped;
}

namespace jal
{
    vector<ParseToken> splitToLines(ext::StrExt &fullText)
    {
        auto lines = fullText.split("\n");
        vector<ParseToken> tokens;
        for(int i=0; i<lines.size(); i++)
        {
            auto &line = lines.at(i);
            ParsePos from{i, 0};
            ParsePos to{i, max((long)0,((long)line.size())-1)};
            ParseToken token{from, to, line, false, false};
            tokens.push_back(token);
        }
        return tokens;
    }
    vector<ParseToken> splitOutMultilines(vector<ParseToken> &tokens)
    {
        vector<ParseToken> newSet;
        bool inMulti{false};
        ParsePos from, to;
        optional<ParseToken> partial;
        for(const auto &token : tokens)
        {
            if(token.isMultiline || token.isAtomicParse)
            {
                if(inMulti) { return {}; /*TODO throw an exception here*/ }
                newSet.push_back(token);
                continue; 
            }

            const auto &line = token.token;
            auto found = line.findAll(ML_TOK);
            auto frontTrim = _vecToSet(line.findAll(ML_TOK_FTRIM));
            auto backTrim = _vecToSet(line.findAll(ML_TOK_BTRIM));
            auto escaped = _findEscaped(line);
            
            if(found.empty())
            {
                if(inMulti)
                {
                    if(!partial) { return {}; /*TODO throw an exception here*/ }
                    partial.value().token += ("\n" + line);
                }
                else { newSet.push_back(token); }
            }
            else
            {
                for(auto col : found)
                {
                    if(inMulti)
                    {
                    }
                    else
                    {
                        if(col != 0)
                        {
                        }
                        else
                        {
                        }
                    }
                }
            }
        }
        return newSet;
    }
    vector<ParseToken> splitOutQuoted(vector<ParseToken> &tokens)
    {
        return {};
    }
}
