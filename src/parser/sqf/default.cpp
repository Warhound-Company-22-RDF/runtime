#include "default.h"
#include "../../runtime/util.h"

//#include "string_op.h"



#include <algorithm>
#include <cwctype>
#include <utility>
#include <vector>
#include <string>
#include <sstream>

namespace err = logmessage::sqf;

using namespace ::sqf::runtime::util;

void ::sqf::parser::sqf::default::skip(size_t& curoff)
{
    while (true)
    {
        switch (m_contents[curoff])
        {
        case ' ': curoff++; continue;
        case '\t': curoff++; continue;
        case '\r': curoff++; continue;
        case '\n': curoff++; continue;
        default: return;
        }
    }
}
void ::sqf::parser::sqf::default::skip(::sqf::runtime::diagnostics::diag_info& info)
{
    while (true)
    {
        switch (m_contents[info.offset])
        {
        case ' ': info.offset++; info.column++; continue;
        case '\t': info.offset++; info.column++; continue;
        case '\r': info.offset++; info.column++; continue;
        case '\n': info.offset++; info.line++; info.column = 0; continue;
        case '#':
            if ((m_contents[info.offset + 1] == 'l' || m_contents[info.offset + 1] == 'L') &&
                (m_contents[info.offset + 2] == 'i' || m_contents[info.offset + 1] == 'I') &&
                (m_contents[info.offset + 3] == 'n' || m_contents[info.offset + 1] == 'N') &&
                (m_contents[info.offset + 4] == 'e' || m_contents[info.offset + 1] == 'E'))
            {
                info.offset += 6;
                size_t start = info.offset;
                for (; m_contents[info.offset] != '\0' && m_contents[info.offset] != '\n' && m_contents[info.offset] != ' '; info.offset++);
                auto str = std::string(m_contents_actual.substr(start, info.offset - start));
                info.line = static_cast<size_t>(std::stoul(str));

                for (; m_contents[info.offset] != '\0' && m_contents[info.offset] != '\n' && m_contents[info.offset] == ' '; info.offset++);
                if (m_contents[info.offset] != '\0' && m_contents[info.offset] != '\n')
                {
                    start = info.offset;
                    for (; m_contents[info.offset] != '\0' && m_contents[info.offset] != '\n'; info.offset++);
                    auto str = std::string(m_contents_actual.substr(start, info.offset - start));
                    info.file = strip_quotes(str);
                }
                break;
            }
        default: return;
        }
    }
}

//endchr = [,;];
size_t sqf::parser::sqf::default::endchr(size_t off) { return m_contents[off] == ';' || m_contents[off] == ',' ? 1 : 0; }
//identifier = [_a-zA-Z][_a-zA-Z0-9]*;
size_t sqf::parser::sqf::default::identifier(size_t off) { size_t i = off; if (!((m_contents[i] >= 'a' && m_contents[i] <= 'z') || (m_contents[i] >= 'A' && m_contents[i] <= 'Z') || m_contents[i] == '_')) return 0; for (i = off + 1; (m_contents[i] >= 'a' && m_contents[i] <= 'z') || (m_contents[i] >= 'A' && m_contents[i] <= 'Z') || (m_contents[i] >= '0' && m_contents[i] <= '9') || m_contents[i] == '_'; i++) {}; return i - off; }
//identifier = [_a-zA-Z0-9]+;
size_t sqf::parser::sqf::default::assidentifier(size_t off)
{
    size_t i = off;
    for (i++; (m_contents[i] >= 'a' && m_contents[i] <= 'z') || (m_contents[i] >= 'A' && m_contents[i] <= 'Z') || (m_contents[i] >= '0' && m_contents[i] <= '9') || m_contents[i] == '_'; i++);
    return i - off;
}
//operator_ = [+-*/%^]|&&|\|\||==|[!<>][=]?|[a-zA-Z_]+;
size_t sqf::parser::sqf::default::operator_(size_t off) {
    if (m_contents[off] == '+' || m_contents[off] == '-' || m_contents[off] == '*' || m_contents[off] == '/' || m_contents[off] == '%' || m_contents[off] == '^' || m_contents[off] == ':' || m_contents[off] == '#') return 1;
    if ((m_contents[off] == '|' && m_contents[off + 1] == '|') || (m_contents[off] == '&' && m_contents[off + 1] == '&') || (m_contents[off] == '=' && m_contents[off + 1] == '=') || (m_contents[off] == '>' && m_contents[off + 1] == '>')) return 2;
    if (m_contents[off] == '<' || m_contents[off] == '>' || m_contents[off] == '!')
    {
        if (m_contents[off + 1] == '=') return 2;
        return 1;
    }
    size_t i;
    for (i = off; (m_contents[i] >= 'a' && m_contents[i] <= 'z') || (m_contents[i] >= 'A' && m_contents[i] <= 'Z') || (m_contents[i] >= '0' && m_contents[i] <= '9') || m_contents[i] == '_'; i++);
    return i - off;
}
//hexadecimal = [0-9a-fA-F]+;
size_t sqf::parser::sqf::default::hexadecimal(size_t off) { size_t i; for (i = off; (m_contents[i] >= 'a' && m_contents[i] <= 'f') || (m_contents[i] >= 'A' && m_contents[i] <= 'F') || (m_contents[i] >= '0' && m_contents[i] <= '9'); i++) {}; return i - off; }
//scalarsub = [0-9]+;
size_t sqf::parser::sqf::default::scalarsub(size_t off) { size_t i; for (i = off; m_contents[i] >= '0' && m_contents[i] <= '9'; i++) {}; return i - off; }
//scalar = scalarsub(.scalarsub)?;
size_t sqf::parser::sqf::default::scalar(size_t off) { size_t i = off + scalarsub(off); if (m_contents[off] == '.') i += scalarsub(off); return i - off; }
//anytext = (?![ \t\r\n;])+;
size_t sqf::parser::sqf::default::anytext(size_t off) { size_t i; for (i = off; m_contents[i] != ' ' && m_contents[i] != '\t' && m_contents[i] != '\r' && m_contents[i] != '\n' && m_contents[i] != ';'; i++) {}; return i - off; }
//SQF = [ STATEMENT { endchr { endchr } STATEMENT } ]
bool ::sqf::parser::sqf::default::SQF_start(size_t curoff) { return true; }
void ::sqf::parser::sqf::default::SQF(astnode& root, bool& errflag)
{
    skip(m_info);
    size_t endchrlen;
    while ((endchrlen = endchr(m_info.offset)) > 0)
    {
        m_info.offset += endchrlen;
        m_info.column += endchrlen;
        skip(m_info);
    }
    //Iterate over statements as long as it is an instruction start.
    while (STATEMENT_start(m_info.offset))
    {
        STATEMENT(root, errflag);
        skip(m_info);
        //Make sure at least one endchr is available unless no statement follows
        if (!endchr(m_info.offset) && STATEMENT_start(m_info.offset))
        {
            log(err::ExpectedStatementTerminator(m_info));
            errflag = true;
        }
        else
        {
            //Add endchr up until no semicolon is left
            while ((endchrlen = endchr(m_info.offset)) > 0)
            {
                m_info.offset += endchrlen;
                m_info.column += endchrlen;
                skip(m_info);
            }
        }
    }
}
//STATEMENT = ASSIGNMENT | BINARYEXPRESSION;
bool ::sqf::parser::sqf::default::STATEMENT_start(size_t curoff) { return ASSIGNMENT_start(curoff) | BINARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::STATEMENT(astnode& root, bool& errflag)
{
    if (ASSIGNMENT_start(m_info.offset))
    {
        ASSIGNMENT(root, errflag);
    }
    else if (BINARYEXPRESSION_start(m_info.offset))
    {
        BINARYEXPRESSION(root, errflag);
    }
    else
    {
        log(err::NoViableAlternativeStatement(m_info));
        errflag = true;
    }
    //thisnode.length = curoff - thisnode.offset;
    //root.children.push_back(thisnode);
}
//ASSIGNMENT(2) = assidentifier '=' BINARYEXPRESSION | "private" assidentifier '=' BINARYEXPRESSION;
bool ::sqf::parser::sqf::default::ASSIGNMENT_start(size_t curoff)
{
    size_t len;
#if defined(SQFVM_ARMA2_SYNTAX)
    const char* priv = "local";
#else
    const char* priv = "private";
#endif

    if (std::equal(m_contents + m_info.offset, m_contents + m_info.offset + ::sqf::runtime::util::strlen(priv),
        priv, priv + ::sqf::runtime::util::strlen(priv), [](char l, char r) { return std::tolower(l) == std::tolower(r); }))
    {
        curoff += ::sqf::runtime::util::strlen(priv);
        skip(curoff);
    }
    if ((len = assidentifier(curoff)) > 0)
    {
        curoff += len;
        skip(curoff);
        return m_contents[curoff] == '=' && m_contents[curoff + 1] != '=';
    }
    else
    {
        return false;
    }
}
void ::sqf::parser::sqf::default::ASSIGNMENT(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::ASSIGNMENT;
    thisnode.offset = m_info.offset;
    thisnode.file = m_info.file;
    size_t len;
    bool assignlocal = false;
#if defined(SQFVM_ARMA2_SYNTAX)
    const char* priv = "local";
#else
    const char* priv = "private";
#endif

    if (std::equal(m_contents + m_info.offset, m_contents + m_info.offset + ::sqf::runtime::util::strlen(priv),
        priv, priv + ::sqf::runtime::util::strlen(priv), [](char l, char r) { return std::tolower(l) == std::tolower(r); }))
    {
        m_info.offset += ::sqf::runtime::util::strlen(priv);
        m_info.column += ::sqf::runtime::util::strlen(priv);
        skip(m_info);
        assignlocal = true;
        thisnode.kind = nodetype::ASSIGNMENTLOCAL;
    }
    //receive the ident
    len = assidentifier(m_info.offset);
    auto ident = std::string(m_contents_actual.substr(m_info.offset, len));
    thisnode.length = len;
    thisnode.line = m_info.line;
    thisnode.col = m_info.column;

    auto varnode = astnode();
    varnode.offset = m_info.offset;
    varnode.length = len;
    varnode.content = ident;
    varnode.kind = nodetype::VARIABLE;
    varnode.col = m_info.column;
    varnode.line = m_info.line;
    varnode.file = m_info.file;
    thisnode.children.emplace_back(std::move(varnode));

    if (assignlocal && ident[0] != '_')
    {
        log(err::MissingUnderscoreOnPrivateVariable(m_info, ident));
    }
    if (ident[0] >= '0' && ident[0] <= '9')
    {
        log(err::InvalidStartOfGlobalVariable(m_info, ident));
    }
    m_info.offset += len;
    m_info.column += len;
    skip(m_info);
    //skip the '=' (is confirmed to be present in ASSIGNMENT_start)
    m_info.offset++;
    m_info.column++;
    skip(m_info);

    if (BINARYEXPRESSION_start(m_info.offset))
    {
        BINARYEXPRESSION(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    thisnode.length = m_info.offset - thisnode.offset;
    root.children.emplace_back(std::move(thisnode));
}
//BINARYEXPRESSION = BEXP1;
//BEXP1 = BEXP2 [ boperator BEXP10 ];
//BEXP2 = BEXP3 [ boperator BEXP10 ];
//BEXP3 = BEXP4 [ boperator BEXP10 ];
//BEXP4 = BEXP5 [ boperator BEXP10 ];
//BEXP5 = BEXP6 [ boperator BEXP10 ];
//BEXP6 = BEXP7 [ boperator BEXP10 ];
//BEXP7 = BEXP8 [ boperator BEXP10 ];
//BEXP8 = BEXP9 [ boperator BEXP10 ];
//BEXP9 = BEXP10 [ boperator BEXP10 ];
//BEXP10 = PRIMARYEXPRESSION [ boperator BEXP10 ];
void ::sqf::parser::sqf::default::bexp_orderfix(astnode& root, astnode thisnode, nodetype plevel)
{
    auto& othernodeRef = thisnode.children.back();
    if (othernodeRef.children.size() == 3 && othernodeRef.kind == plevel)
    {
        auto othernode = thisnode.children.back();
        astnode* ptr = &othernode.children.front();
        astnode* lptr = &othernode;
        while (ptr->children.size() == 3 && ptr->kind == plevel)
        {
            lptr = ptr;
            ptr = &ptr->children.front();
        }

        thisnode.children.pop_back();
        thisnode.children.push_back(*ptr);
        lptr->children.front() = thisnode;
        root.children.push_back(othernode);
    }
    else
    {
        root.children.emplace_back(std::move(thisnode));
    }
}
bool ::sqf::parser::sqf::default::bexp10_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp10(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP10;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (PRIMARYEXPRESSION_start(m_info.offset))
    {
        PRIMARYEXPRESSION(thisnode, errflag);
    }

    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 10))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp10_start(m_info.offset))
        {
            bexp10(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP10);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp9_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp9(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP9;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp10_start(m_info.offset))
    {
        bexp10(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 9))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp9_start(m_info.offset))
        {
            bexp9(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP9);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp8_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp8(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP8;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp9_start(m_info.offset))
    {
        bexp9(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 8))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp8_start(m_info.offset))
        {
            bexp8(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP8);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp7_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp7(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP7;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp8_start(m_info.offset))
    {
        bexp8(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 7))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp7_start(m_info.offset))
        {
            bexp7(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP7);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp6_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp6(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP6;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp7_start(m_info.offset))
    {
        bexp7(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 6))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp6_start(m_info.offset))
        {
            bexp6(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP6);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp5_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp5(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP5;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp6_start(m_info.offset))
    {
        bexp6(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 5))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp5_start(m_info.offset))
        {
            bexp5(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP5);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp4_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp4(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP4;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp5_start(m_info.offset))
    {
        bexp5(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 4))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp4_start(m_info.offset))
        {
            bexp4(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP4);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp3_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp3(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP3;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp4_start(m_info.offset))
    {
        bexp4(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 3))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp3_start(m_info.offset))
        {
            bexp3(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP3);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp2_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp2(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP2;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp3_start(m_info.offset))
    {
        bexp3(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 2))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp2_start(m_info.offset))
        {
            bexp2(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP2);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::bexp1_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::bexp1(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.offset = m_info.offset;
    thisnode.kind = nodetype::BEXP1;
    thisnode.file = m_info.file;
    thisnode.line = m_info.line;
    size_t oplen;
    std::string op;
    skip(m_info);
    if (bexp2_start(m_info.offset))
    {
        bexp2(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if ((oplen = operator_(m_info.offset)) > 0 && m_contains_binary(op = std::string(m_contents_actual.substr(m_info.offset, oplen)), 1))
    {
        auto node = astnode();
        node.content = op;
        node.offset = m_info.offset;
        node.kind = nodetype::BINARYOP;
        node.col = m_info.column;
        node.line = m_info.line;
        node.length = oplen;
        node.file = m_info.file;
        thisnode.children.emplace_back(std::move(node));
        m_info.offset += oplen;
        m_info.column += oplen;
        skip(m_info);
        if (bexp1_start(m_info.offset))
        {
            bexp1(thisnode, errflag);
        }
        else
        {
            log(err::MissingRightArgument(m_info, op));
            errflag = true;
        }
        bexp_orderfix(root, std::move(thisnode), nodetype::BEXP1);
    }
    else
    {
        //We won't need this node anymore. Just move all children to root
        root.children.insert(root.children.end(), std::make_move_iterator(thisnode.children.begin()), std::make_move_iterator(thisnode.children.end()));
    }
}
bool ::sqf::parser::sqf::default::BINARYEXPRESSION_start(size_t curoff) { return PRIMARYEXPRESSION_start(curoff); }
void ::sqf::parser::sqf::default::BINARYEXPRESSION(astnode& root, bool& errflag)
{
    bexp1(root, errflag);
}
//BRACKETS = '(' BINARYEXPRESSION ')';
bool ::sqf::parser::sqf::default::BRACKETS_start(size_t curoff) { return m_contents[curoff] == '('; }
void ::sqf::parser::sqf::default::BRACKETS(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::BRACKETS;
    thisnode.offset = m_info.offset;
    thisnode.file = m_info.file;
    m_info.offset++;
    m_info.column++;
    skip(m_info);
    if (BINARYEXPRESSION_start(m_info.offset))
    {
        BINARYEXPRESSION(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedBinaryExpression(m_info));
        errflag = true;
    }
    skip(m_info);
    if (m_contents[m_info.offset] == ')')
    {
        m_info.offset++;
        m_info.column++;
    }
    else
    {
        log(err::MissingRoundClosingBracket(m_info));
        errflag = true;
    }
    thisnode.length = m_info.offset - thisnode.offset;
    root.children.emplace_back(std::move(thisnode));
}
//PRIMARYEXPRESSION = NUMBER | UNARYEXPRESSION | NULAREXPRESSION | VARIABLE | STRING | CODE | BRACKETS | ARRAY;
bool ::sqf::parser::sqf::default::PRIMARYEXPRESSION_start(size_t curoff) { return NUMBER_start(curoff) || UNARYEXPRESSION_start(curoff) || NULAREXPRESSION_start(curoff) || VARIABLE_start(curoff) || STRING_start(curoff) || CODE_start(curoff) || BRACKETS_start(curoff) || ARRAY_start(curoff); }
void ::sqf::parser::sqf::default::PRIMARYEXPRESSION(astnode& root, bool& errflag)
{
    if (NUMBER_start(m_info.offset))
    {
        NUMBER(root, errflag);
    }
    else if (UNARYEXPRESSION_start(m_info.offset))
    {
        UNARYEXPRESSION(root, errflag);
    }
    else if (NULAREXPRESSION_start(m_info.offset))
    {
        NULAREXPRESSION(root, errflag);
    }
    else if (VARIABLE_start(m_info.offset))
    {
        VARIABLE(root, errflag);
    }
    else if (STRING_start(m_info.offset))
    {
        STRING(root, errflag);
    }
    else if (CODE_start(m_info.offset))
    {
        CODE(root, errflag);
    }
    else if (BRACKETS_start(m_info.offset))
    {
        BRACKETS(root, errflag);
    }
    else if (ARRAY_start(m_info.offset))
    {
        ARRAY(root, errflag);
    }
    else
    {
        log(err::NoViableAlternativePrimaryExpression(m_info));
        errflag = true;
    }
    //thisnode.length = curoff - thisnode.offset;
    //root.children.push_back(thisnode);
}
//NULAREXPRESSION = operator;
bool ::sqf::parser::sqf::default::NULAREXPRESSION_start(size_t curoff) { auto oplen = operator_(curoff); return oplen > 0 ? m_contains_nular(std::string(m_contents_actual.substr(curoff, oplen))) : false; }
void ::sqf::parser::sqf::default::NULAREXPRESSION(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::NULAROP;
    thisnode.file = m_info.file;
    auto len = operator_(m_info.offset);
    auto ident = std::string(m_contents_actual.substr(m_info.offset, len));
    thisnode.content = ident;
    thisnode.length = len;
    thisnode.offset = m_info.offset;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;
    m_info.offset += len;
    m_info.column += len;
    root.children.emplace_back(std::move(thisnode));
}
//UNARYEXPRESSION = operator PRIMARYEXPRESSION;
bool ::sqf::parser::sqf::default::UNARYEXPRESSION_start(size_t curoff) { auto oplen = operator_(curoff); return oplen > 0 ? m_contains_unary(std::string(m_contents_actual.substr(curoff, oplen))) : false; }
void ::sqf::parser::sqf::default::UNARYEXPRESSION(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::UNARYEXPRESSION;
    thisnode.offset = m_info.offset;
    thisnode.file = m_info.file;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;

    auto len = operator_(m_info.offset);
    auto operatorname = std::string(m_contents_actual.substr(m_info.offset, len));
    auto opnode = astnode();
    opnode.kind = nodetype::UNARYOP;
    opnode.offset = m_info.offset;
    opnode.length = len;
    opnode.content = operatorname;
    opnode.col = m_info.column;
    opnode.file = m_info.file;
    opnode.line = m_info.line;
    thisnode.children.emplace_back(std::move(opnode));
    m_info.offset += len;
    m_info.column += len;
    skip(m_info);

    if (PRIMARYEXPRESSION_start(m_info.offset))
    {
        PRIMARYEXPRESSION(thisnode, errflag);
    }
    else
    {
        log(err::MissingRightArgument(m_info, operatorname));
        errflag = true;
    }
    thisnode.length = m_info.offset - thisnode.offset;
    root.children.emplace_back(std::move(thisnode));
}
//NUMBER = ("0x" | '$' | '.') hexadecimal | scalar;
bool ::sqf::parser::sqf::default::NUMBER_start(size_t curoff)
{
    return  m_contents[curoff] == '$' ||
        (
            m_contents[curoff] == '.' &&
            m_contents[curoff + 1] >= '0' &&
            m_contents[curoff + 1] <= '9'
            ) ||
        (m_contents[curoff] >= '0' && m_contents[curoff] <= '9');
}
void ::sqf::parser::sqf::default::NUMBER(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::NUMBER;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;
    thisnode.file = m_info.file;
    if (m_contents[m_info.offset] == '$')
    {
        thisnode.kind = nodetype::HEXNUMBER;
        size_t i;
        for (i = m_info.offset + 1; (m_contents[i] >= '0' && m_contents[i] <= '9') || (m_contents[i] >= 'A' && m_contents[i] <= 'F') || (m_contents[i] >= 'a' && m_contents[i] <= 'f'); i++);
        auto ident = std::string(m_contents_actual.substr(m_info.offset + 1, i - m_info.offset));
        thisnode.content = ident;
        thisnode.offset = m_info.offset;
        thisnode.length = i - m_info.offset;
        m_info.column += i - m_info.offset;
        m_info.offset = i;
    }
    else if (m_contents[m_info.offset] == '0' && m_contents[m_info.offset + 1] == 'x')
    {
        thisnode.kind = nodetype::HEXNUMBER;
        size_t i;
        for (i = m_info.offset + 2; (m_contents[i] >= '0' && m_contents[i] <= '9') || (m_contents[i] >= 'A' && m_contents[i] <= 'F') || (m_contents[i] >= 'a' && m_contents[i] <= 'f'); i++);
        auto ident = std::string(m_contents_actual.substr(m_info.offset, i - m_info.offset));
        thisnode.content = ident;
        thisnode.offset = m_info.offset;
        thisnode.length = i - m_info.offset;
        m_info.column += i - m_info.offset;
        m_info.offset = i;
    }
    else
    {
        size_t i = m_info.offset;
        bool numhaddot = false;
        unsigned short numhadexp = 0;
        while (true)
        {
            if (m_contents[i] >= '0' && m_contents[i] <= '9')
            {
                i++;
                continue;
            }
            else if (!numhaddot && m_contents[i] == '.')
            {
                i++;
                numhaddot = true;
                continue;
            }
            else if (numhadexp == 0 && (m_contents[i] == 'e' || m_contents[i] == 'E'))
            {
                i++;
                numhadexp++;
                continue;
            }
            else if (numhadexp == 1 && (m_contents[i] == '+' || m_contents[i] == '-'))
            {
                i++;
                numhadexp++;
                continue;
            }
            else
            {
                break;
            }

        }
        auto ident = std::string(m_contents_actual.substr(m_info.offset, i));
        thisnode.content = ident;
        thisnode.offset = m_info.offset;
        thisnode.length = i - m_info.offset;
        m_info.column += i - m_info.offset;
        m_info.offset = i;
    }
    if (thisnode.content.empty())
    {
        log(err::EmptyNumber(m_info));
        errflag = true;
    }
    root.children.emplace_back(std::move(thisnode));
}
//VARIABLE = identifier;
bool ::sqf::parser::sqf::default::VARIABLE_start(size_t curoff) { auto len = identifier(curoff); return len > 0 && !m_contains_binary(std::string(m_contents_actual.substr(curoff, len)), 0); }
void ::sqf::parser::sqf::default::VARIABLE(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::VARIABLE;
    thisnode.file = m_info.file;
    auto len = identifier(m_info.offset);
    auto ident = std::string(m_contents_actual.substr(m_info.offset, len));
    thisnode.content = ident;
    thisnode.length = len;
    thisnode.offset = m_info.offset;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;

    m_info.offset += len;
    m_info.column += len;
    root.children.emplace_back(std::move(thisnode));
}
//STRING = '"' { any | "\"\"" } '"' | '\'' { any | "''" } '\'';
bool ::sqf::parser::sqf::default::STRING_start(size_t curoff) { return m_contents[curoff] == '\'' || m_contents[curoff] == '"'; }
void ::sqf::parser::sqf::default::STRING(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::STRING;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;
    thisnode.file = m_info.file;
    size_t i;
    auto startchr = m_contents[m_info.offset];
    m_info.column++;
    for (i = m_info.offset + 1; m_contents[i] != '\0' && (m_contents[i] != startchr || m_contents[i + 1] == startchr); i++)
    {
        if (m_contents[i] == startchr)
        {
            m_info.column += 2;
            i++;
        }
        else if (m_contents[i] == '\n')
        {
            m_info.column = 0;
            m_info.line++;
        }
        else
        {
            m_info.column++;
        }
    }
    i++;
    m_info.column++;
    auto fullstring = std::string(m_contents_actual.substr(m_info.offset, i - m_info.offset));
    thisnode.content = fullstring;
    thisnode.length = i - m_info.offset;
    thisnode.offset = m_info.offset;
    m_info.offset = i;
    root.children.emplace_back(std::move(thisnode));
}
//CODE = "{" SQF "}";
bool ::sqf::parser::sqf::default::CODE_start(size_t curoff) { return m_contents[curoff] == '{'; }
void ::sqf::parser::sqf::default::CODE(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::CODE;
    thisnode.offset = m_info.offset;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;
    thisnode.file = m_info.file;
    m_info.offset++;
    m_info.column++;
    skip(m_info);

    if (SQF_start(m_info.offset))
    {
        SQF(thisnode, errflag);
    }
    else
    {
        log(err::ExpectedSQF(m_info));
        errflag = true;
    }

    if (m_contents[m_info.offset] == '}')
    {
        m_info.offset++;
        m_info.column++;
    }
    else
    {
        log(err::MissingCurlyClosingBracket(m_info));
        errflag = true;
    }
    thisnode.length = m_info.offset - thisnode.offset;
    root.children.emplace_back(std::move(thisnode));
}
//ARRAY = '[' [ BINARYEXPRESSION { ',' BINARYEXPRESSION } ] ']';
bool ::sqf::parser::sqf::default::ARRAY_start(size_t curoff) { return m_contents[curoff] == '['; }
void ::sqf::parser::sqf::default::ARRAY(astnode& root, bool& errflag)
{
    auto thisnode = astnode();
    thisnode.kind = nodetype::ARRAY;
    thisnode.offset = m_info.offset;
    thisnode.col = m_info.column;
    thisnode.line = m_info.line;
    thisnode.file = m_info.file;
    m_info.offset++;
    m_info.column++;
    skip(m_info);
    if (BINARYEXPRESSION_start(m_info.offset))
    {
        BINARYEXPRESSION(thisnode, errflag);
        skip(m_info);
        while (m_contents[m_info.offset] == ',')
        {
            m_info.column++;
            m_info.offset++;
            skip(m_info);

            if (BINARYEXPRESSION_start(m_info.offset))
            {
                BINARYEXPRESSION(thisnode, errflag);
                skip(m_info);
            }
            else
            {
                log(err::ExpectedBinaryExpression(m_info));
                errflag = true;
            }
        }
    }
    if (m_contents[m_info.offset] == ']')
    {
        m_info.offset++;
        m_info.column++;
    }
    else
    {
        log(err::MissingSquareClosingBracket(m_info));
        errflag = true;
    }
    thisnode.length = m_info.offset - thisnode.offset;
    thisnode.content = std::string(m_contents_actual.substr(thisnode.offset, thisnode.length));
    root.children.emplace_back(std::move(thisnode));
}

sqf::parser::sqf::default::astnode sqf::parser::sqf::default::parse(bool& errflag)
{
    astnode node;
    node.kind = nodetype::SQF;
    node.offset = 0;
    node.content = m_contents;
    node.file = m_file;
    SQF(node, errflag);
    node.length = m_info.offset;
    skip(m_info);
    if (!errflag && m_contents[m_info.offset] != '\0') {
        log(err::EndOfFile(m_info));
        errflag = true;
    }
    return node;
}


bool sqf::parser::sqf::default::to_assembly(astnode& node, std::vector<::sqf::runtime::instruction>& set)
{
    // execute_parsing_callbacks(full, node, evaction::enter);
    switch (node.kind)
    {
    case nodetype::BEXP1:
    case nodetype::BEXP2:
    case nodetype::BEXP3:
    case nodetype::BEXP4:
    case nodetype::BEXP5:
    case nodetype::BEXP6:
    case nodetype::BEXP7:
    case nodetype::BEXP8:
    case nodetype::BEXP9:
    case nodetype::BEXP10:
    case nodetype::BINARYEXPRESSION:
    {
        to_assembly(node.children[0], set);
        to_assembly(node.children[2], set);
        auto inst = std::make_shared<sqf::inst::callbinary>(sqf::commandmap::get().getrange_b(node.children[1].content));
        inst->setdbginf(node.children[1].line, node.children[1].col, node.file, sqf::parse::dbgsegment(full, node.children[1].offset, node.children[1].length));
        stack->push_back(inst);
    }
    break;
    case nodetype::UNARYEXPRESSION:
    {
        to_assembly(node.children[1], set);
        auto inst = std::make_shared<sqf::inst::callunary>(sqf::commandmap::get().getrange_u(node.children[0].content));
        inst->setdbginf(node.children[0].line, node.children[0].col, node.file, sqf::parse::dbgsegment(full, node.children[0].offset, node.children[0].length));
        stack->push_back(inst);
    }
    break;
    case nodetype::NULAROP:
    {
        auto inst = std::make_shared<sqf::inst::callnular>(sqf::commandmap::get().get(node.content));
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::HEXNUMBER:
    {
        try
        {
            auto inst = std::make_shared<sqf::inst::push>(sqf::value(std::stol(node.content, nullptr, 16)));
            inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
            stack->push_back(inst);
        }
        catch (std::out_of_range&)
        {
            auto inst = std::make_shared<sqf::inst::push>(sqf::value(std::make_shared<sqf::scalardata>(std::nanf(""))));
            inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
            log(logmessage::assembly::NumberOutOfRange(*inst));
            stack->push_back(inst);
        }
    }
    break;
    case nodetype::NUMBER:
    {
        try
        {
            auto inst = std::make_shared<sqf::inst::push>(sqf::value(std::stod(node.content)));
            inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
            stack->push_back(inst);
        }
        catch (std::out_of_range&)
        {
            auto inst = std::make_shared<sqf::inst::push>(sqf::value(std::make_shared<sqf::scalardata>(std::nanf(""))));
            inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
            log(logmessage::assembly::NumberOutOfRange(*inst));
            stack->push_back(inst);
        }
    }
    break;
    case nodetype::STRING:
    {
        auto inst = std::make_shared<sqf::inst::push>(sqf::value(std::make_shared<sqf::stringdata>(node.content, true)));
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::CODE:
    {
        auto cs = std::make_shared<sqf::callstack>(missionnamespace());
        sqf::parse::astnode previous_node;
        for (size_t i = 0; i < node.children.size(); i++)
        {
            if (i != 0)
            {
                auto inst = std::make_shared<sqf::inst::endstatement>();
                inst->setdbginf(previous_node.line, previous_node.col + previous_node.length, previous_node.file, sqf::parse::dbgsegment(full, previous_node.offset, previous_node.length));
                cs->push_back(inst);
            }
            previous_node = node.children[i];
            to_assembly(previous_node, set);
        }
        auto inst = std::make_shared<sqf::inst::push>(sqf::value(cs));
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::ARRAY:
    {
        for (auto& subnode : node.children)
        {
            to_assembly(subnode, set);
        }
        auto inst = std::make_shared<sqf::inst::makearray>(node.children.size());
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::ASSIGNMENT:
    {
        to_assembly(node.children[1], set);
        auto inst = std::make_shared<sqf::inst::assignto>(node.children[0].content);
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::ASSIGNMENTLOCAL:
    {
        to_assembly(node.children[1], set);
        auto inst = std::make_shared<sqf::inst::assigntolocal>(node.children[0].content);
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    case nodetype::VARIABLE:
    {
        auto inst = std::make_shared<sqf::inst::getvariable>(node.content);
        inst->setdbginf(node.line, node.col, node.file, sqf::parse::dbgsegment(full, node.offset, node.length));
        stack->push_back(inst);
    }
    break;
    default:
    {
        astnode previous_node;
        for (size_t i = 0; i < node.children.size(); i++)
        {
            if (i != 0)
            {
                auto inst = std::make_shared<sqf::inst::endstatement>();
                inst->setdbginf(previous_node.line, previous_node.col + previous_node.length, previous_node.file, sqf::parse::dbgsegment(full, previous_node.offset, previous_node.length));
                set.push_back(inst);
            }
            previous_node = node.children[i];
            to_assembly(previous_node, set);
        }
    }
    }
    // execute_parsing_callbacks(full, node, evaction::exit);
}

bool sqf::parser::sqf::default::check_syntax(::sqf::runtime::runtime & runtime, std::string contents)
{
    return false;
}

std::optional<::sqf::runtime::instruction_set> sqf::parser::sqf::default::parse(::sqf::runtime::runtime & runtime, std::string contents)
{
    return std::optional<::sqf::runtime::instruction_set>();
}
