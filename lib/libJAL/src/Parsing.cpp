#include "Parsing.h"
#include <set>
#include <optional>
#include <map>

#include <iostream>
#include <sstream>
#include <fstream>

#define ML_TOK "\"\"\""
#define ML_TOK_FTRIM "\\\"\"\""
#define ML_TOK_BTRIM "\"\"\"\\"
#define Q_TOK "\""
#define STR_ESC_TOK "~"

#define _CONTAINS(col, target) (col.find(target)!=col.end())


std::set<size_t> _vecToSet(std::vector<size_t> &&vec)
{
    return std::set<size_t>(vec.begin(), vec.end());
}

std::set<size_t> _findEscaped(const ext::StrExt &line)
{
    auto escList = line.findAll(STR_ESC_TOK);
    std::set<size_t> escaped;
    for(auto escPt : escList)
    {
        if((escaped.find(escPt) == escaped.end()) && (escPt != (line.size()-1)))
        {
            escaped.insert(escPt+1); //next index is a candidate
        }
    }
    return escaped;
}

ext::StrExt _filterEscaped(const ext::StrExt &line)
{
    auto escaped = _findEscaped(line);
    ext::StrExt out;
    out.reserve(line.size());
    for(size_t i=0; i < line.size(); i++)
    {
        if(!_CONTAINS(escaped, i+1))
        {
            out.append(1,line[i]);
        }
        else if((line[i+1]!='"')&&(line[i+1]!='~'))
        {
            out.append(1,line[i]);
        }
    }
    return out;
}

namespace jal
{
    ext::StrExt readFile(const ext::StrExt &path)
    {
        try
        {
            ifstream file(path);
            if(file.fail())
            {
                throw JalException("File error; unable to read file: "+path,
                                   ErrorLevel::io);
            }
            ostringstream strStream;
            strStream << file.rdbuf();
            return strStream.str();
        }
        catch(...)
        {
            throw JalException("File error; unable to read file: "+path,
                               ErrorLevel::io);
        }
    }

    vector<ParseToken> splitToLines(const ext::StrExt &fullText)
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
        ParsePos from, to;
        optional<ParseToken> partial;
        auto inMulti = [&]()->bool { return partial.has_value(); };
        bool trimFront{false};
        for(const auto &token : tokens)
        {
            if(token.isMultiline || token.isAtomicParse)
            {
                if(inMulti())
                {
                    throw JalException("Internal error; encountered a processed string within string literal",
                                       ErrorLevel::document,
                                       to_underlying(ErrorType::stringFormatting), token.from);
                }
                newSet.push_back(token);
                continue; 
            }

            const auto &line = token.token;
            auto found = line.findAll(ML_TOK);
            auto frontTrim = _vecToSet(line.findAll(ML_TOK_FTRIM));
            auto backTrim = _vecToSet(line.findAll(ML_TOK_BTRIM));
            auto escaped = _findEscaped(line);
            const auto &absolute = token.from;         
            size_t nextFront{0};

            if(found.empty())
            {
                if(inMulti())
                {
                    partial.value().token += (line + "\n");
                }
                else { newSet.push_back(token); }
            }
            else
            {
                for(size_t col : found)
                {
                    if(inMulti())
                    {
                        if(_CONTAINS(escaped, col))
                        {
                            if(line.find(ML_TOK, col+1) == col+1)
                            { //address ~"""" case (findAll() will skip over substring matches)
                                col++;
                            }
                            else { continue; } //escaped, go to next
                        }
                        
                        ext::StrExt extracted = _filterEscaped(line.substr(nextFront, col-nextFront));
                        
                        if(!partial.value().token.empty())
                        {
                            extracted = partial.value().token+extracted;
                        }
                        if(trimFront)
                        {
                            extracted = extracted.trimFront();
                            trimFront = false;
                        }
                        if(_CONTAINS(backTrim, col)) { extracted = extracted.trimBack(); }
                        partial.value().token = extracted;
                        partial.value().to = {absolute.line, absolute.col+col};
                        newSet.push_back(partial.value());
                        partial.reset();
                        nextFront = col+3+(_CONTAINS(backTrim, col) ? 1 : 0);
                    }
                    else
                    {
                        trimFront =  ((col != 0) ? _CONTAINS(frontTrim, col-1) : false);
                        int outside = trimFront ? col-1 : col;
                        int inside = col+3;
                        if((outside != 0) && (nextFront < outside))
                        {
                            ParseToken other = {{absolute.line, absolute.col+nextFront},
                                                {absolute.line, absolute.col+outside-1},
                                                line.substr(nextFront, outside-nextFront),
                                                false, false};
                            newSet.push_back(other);
                        }
                        ParseToken start;
                        start.from = {absolute.line, absolute.col+inside};
                        start.isMultiline = true;
                        start.isAtomicParse = true;
                        partial = start;
                        nextFront = inside;
                    }
                }
                //end cases
                if(nextFront < line.length()) // check if there's more characters past last str tok
                {
                    ext::StrExt last = line.substr(nextFront);
                    if(!last.trim().empty() && !inMulti())
                    {
                        ParseToken other = {{absolute.line, absolute.col+nextFront},
                                            {absolute.line, absolute.col+line.length()-1},
                                            last,
                                            false, false};
                        newSet.push_back(other);
                    }
                    else if(inMulti())
                    {
                        if(trimFront)
                        {
                            if(partial.value().token.empty())
                            {
                                last = last.trimFront();
                            }
                            else
                            {
                                partial.value().token = partial.value().token.trimFront();
                            }
                            trimFront = false;
                        } 
                        partial.value().token += (_filterEscaped(last) + "\n"); //has yet to terminate, add to partial
                    }
                }
                else if(!trimFront && inMulti())
                {
                    partial.value().token += "\n";
                }
            }
        }
        
        if(inMulti())
        {
            throw JalException("Document error; incomplete string literal",
                               ErrorLevel::document,
                               to_underlying(ErrorType::stringFormatting), partial.value().from);
        }
        
        return newSet;
    }
    vector<ParseToken> splitOutQuoted(vector<ParseToken> &tokens)
    {
        vector<ParseToken> newSet;
        for(const auto &token : tokens)
        {
            if(token.isAtomicParse)
            {
                newSet.push_back(token);
            }
            else
            {
                auto line = token.token;
                auto found = line.findAll(Q_TOK);
                
                auto escaped = _findEscaped(line);
                ext::StrExt frag;
                bool inQuote {false};
                int lastPt{0};
                if(found.empty())
                {
                    newSet.push_back(token);
                    continue;
                }
                for(auto nextPt : found)
                {
                    if(inQuote)
                    {
                        if(_CONTAINS(escaped, lastPt))
                        {
                            continue;
                        }
                        frag = _filterEscaped(line.substr(lastPt, nextPt-lastPt));
                        newSet.push_back({{token.to.line, token.from.col+lastPt},
                                          {token.to.line, token.from.col+nextPt},
                                           frag, false, true});
                        inQuote = false;
                    }
                    else
                    {
                        frag = line.substr(lastPt, nextPt-lastPt);
                        auto frontLen = frag.size();
                        frag = frag.trimFront();
                        frontLen -= frag.size();
                        frag = frag.trimBack();
                        if(!frag.empty())
                        {
                            newSet.push_back({{token.to.line, token.from.col+lastPt+frontLen},
                                              {token.to.line, token.from.col+nextPt},
                                              frag, false, false});
                        }
                        inQuote = true;
                    }
                    lastPt = nextPt+1;
                }
                if(inQuote)
                {
                    throw JalException("Document error; incomplete string",
                                       ErrorLevel::document,
                                       to_underlying(ErrorType::stringFormatting), {token.to.line, token.from.col+lastPt});
                }
                else
                {
                    frag = line.substr(lastPt);
                    if(!frag.empty())
                    {
                        newSet.push_back({{token.to.line, token.from.col+lastPt},
                                          {token.to.line, token.from.col+lastPt+frag.size()},
                                          frag, false, false});
                    }
                }
            }
        }
        
        return newSet;
    }
    vector<VarToken> splitOutDelimiters(vector<ParseToken> &tokens)
    {
        vector<VarToken> newSet;
        for(const auto &token : tokens)
        {
            if(token.isAtomicParse)
            {
                newSet.push_back({ParseType::atomicValue, token});
            }
            else
            {
                map<size_t, ParsDelimiter> split;
                map<ParsDelimiter,string> del = {{ParsDelimiter::openBrace,"{"},
                                                 {ParsDelimiter::closedBrace,"}"},
                                                 {ParsDelimiter::memberCol,":"},
                                                 {ParsDelimiter::openList,"["},
                                                 {ParsDelimiter::closedList,"]"},
                                                 {ParsDelimiter::listDel,","}};
                auto line = token.token;
                for(const auto & [d, s] : del)
                {    
                    auto delimiters = line.findAll(s);
                    for(auto idx : delimiters)
                    {
                        split[idx] = d;
                    }
                }
                if(split.empty())
                {
                    newSet.push_back({ParseType::unparsed, token});
                }
                else
                {
                    size_t front{0};
                    for(const auto & [idx, delEnum] : split)
                    {
                        if(idx > front)
                        {
                            ParseToken chunk = {{token.from.line, token.from.col+front},
                                                {token.to.line, token.to.col+idx-1},
                                                line.substr(front, idx-front),false,false};
                            chunk.token = chunk.token.trim();
                            if(!chunk.token.empty())
                            {
                                newSet.push_back({ParseType::unparsed, chunk});
                            }

                        }
                       
                        DelimPt dp{delEnum, {token.from.line, token.from.col+idx}};
                        newSet.push_back({ParseType::delimiter, dp});
                        front = (idx+1);
                    }
                    if(front < line.size())
                    {
                        
                        ParseToken chunk = {{token.from.line, token.from.col+front},
                                            {token.to.line, token.to.col+line.size()-1},
                                            line.substr(front, line.size()-front),false,false};
                        chunk.token = chunk.token.trim();
                        if(!chunk.token.empty())
                        {
                            newSet.push_back({ParseType::unparsed, chunk});
                        }
                    }
                }
            }
        }
        return newSet;
    }
}

