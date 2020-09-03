
// %skeleton "lalr1.cc" /* -*- C++ -*- */
%language "c++"
%require "3.0"

%define api.value.type variant
%define api.token.constructor
%define api.namespace { sqf::sqc::bison }
%code top {
    #include "tokenizer.h"
    #include <string>
    #include <vector>
}

%code requires
{
     namespace sqf::sqc
     {
          class parser;
     }
     namespace sqf::sqc::bison
     {
          enum class astkind
          {
               __TOKEN = -1,
               NA = 0,
               RETURN,
               THROW,
               ASSIGNMENT,
               DECLARATION,
               FORWARD_DECLARATION,
               FUNCTION_DECLARATION,
               FINAL_FUNCTION_DECLARATION,
               FUNCTION,
               ARGLIST,
               ARGITEM,
               ARGITEM_DEFAULT,
               ARGITEM_TYPE,
               ARGITEM_TYPE_DEFAULT,
               CODEBLOCK,
               IF,
               IFELSE,
               FOR,
               FORSTEP,
               FOREACH,
               WHILE,
               DOWHILE,
               TRYCATCH,
               SWITCH,
               CASE,
               CASE_DEFAULT,
               OP_TERNARY,
               OP_OR,
               OP_AND,
               OP_EQUALEXACT,
               OP_EQUAL,
               OP_NOTEQUALEXACT,
               OP_NOTEQUAL,
               OP_LESSTHAN,
               OP_GREATERTHAN,
               OP_LESSTHANEQUAL,
               OP_GREATERTHANEQUAL,
               OP_PLUS,
               OP_MINUS,
               OP_MULTIPLY,
               OP_DIVIDE,
               OP_REMAINDER,
               OP_NOT,
               OP_BINARY,
               OP_UNARY,
               OP_ARRAY_GET,
               OP_ARRAY_SET,
               VAL_STRING,
               VAL_ARRAY,
               VAL_NUMBER,
               VAL_TRUE,
               VAL_FALSE,
               VAL_NIL,
               GET_VARIABLE
          };
          struct astnode
          {
               sqf::sqc::tokenizer::token token;
               astkind kind;
               std::vector<astnode> children;

               astnode() : token(), kind(astkind::NA)
               {
               }
               astnode(astkind kind) : token(), kind(kind)
               {
               }
               astnode(sqf::sqc::tokenizer::token t) : token(t), kind(astkind::__TOKEN)
               {
               }
               astnode(astkind kind, sqf::sqc::tokenizer::token t) : token(t), kind(kind) {}

               void append(astnode node)
               {
                   children.push_back(node);
               }
               void append_children(const astnode& other)
               { 
                   for (auto node : other.children)
                   {
                       append(node); 
                   } 
               }
          };
     }
}
%code
{
     namespace sqf::sqc::bison
     {
          // Return the next token.
          parser::symbol_type yylex (sqf::sqc::tokenizer&);
     }
}

%lex-param { sqf::sqc::tokenizer &tokenizer }
%parse-param { sqf::sqc::tokenizer &tokenizer }
%parse-param { sqf::sqc::bison::astnode& result }
%parse-param { sqf::sqc::parser& actual }
%parse-param { std::string fpath }
%locations
%define parse.error verbose

%define api.token.prefix {}



/* Tokens */

%token NA 0
%token BE                        "be"
%token BREAK                     "break"
%token RETURN                    "return"
%token THROW                     "throw"
%token LET                       "let"
%token FUNCTION                  "function"
%token FINAL                     "final"
%token FALSE                     "false"
%token FOR                       "for"
%token IF                        "if"
%token ELSE                      "else"
%token FROM                      "from"
%token TO                        "to"
%token DO                        "do"
%token TRY                       "try"
%token CATCH                     "catch"
%token STEP                      "step"
%token SWITCH                    "switch"
%token CASE                      "case"
%token DEFAULT                   "default"
%token NIL                       "nil"
%token TRUE                      "true"
%token PARAMS                    "params"
%token PRIVATE                   "private"
%token WHILE                     "while"
%token CURLYO                    "{"
%token CURLYC                    "}"
%token ROUNDO                    "("
%token ROUNDC                    ")"
%token SQUAREO                   "["
%token SQUAREC                   "]"
%token SEMICOLON                 ";"
%token COMMA                     ","
%token EQUAL                     "="
%token DOT                       "."
%token QUESTIONMARK              "?"

%token <tokenizer::token> ANDAND                    "&&"
%token <tokenizer::token> SLASH                     "/"
%token <tokenizer::token> STAR                      "*"
%token <tokenizer::token> PERCENT                   "%"
%token <tokenizer::token> VLINEVLINE                "||"
%token <tokenizer::token> COLON                     ":"
%token <tokenizer::token> PLUS                      "+"
%token <tokenizer::token> MINUS                     "-"
%token <tokenizer::token> LTEQUAL                   "<="
%token <tokenizer::token> LT                        "<"
%token <tokenizer::token> GTEQUAL                   ">="
%token <tokenizer::token> GT                        ">"
%token <tokenizer::token> EQUALEQUALEQUAL           "==="
%token <tokenizer::token> EQUALEQUAL                "=="
%token <tokenizer::token> EXCLAMATIONMARKEQUALEQUAL "!=="
%token <tokenizer::token> EXCLAMATIONMARKEQUAL      "!="
%token <tokenizer::token> EXCLAMATIONMARK           "!"
%token <tokenizer::token> NUMBER 
%token <tokenizer::token> IDENT  
%token <tokenizer::token> STRING 


%type <sqf::sqc::bison::astnode> statements statement assignment vardecl funcdecl function
%type <sqf::sqc::bison::astnode> funchead arglist codeblock if for while trycatch switch
%type <sqf::sqc::bison::astnode> caselist case exp01 exp02 exp03 exp04 exp05 exp06 exp07
%type <sqf::sqc::bison::astnode> exp08 exp09 expp value array explist filehead argitem arrget

%start start

%%

/*** BEGIN - Change the grammar rules below ***/
/*** BEGIN - Change the grammar rules below ***/
/*** BEGIN - Change the grammar rules below ***/
start: %empty
     | filehead statements                        { result = sqf::sqc::bison::astnode{}; result.append_children($1); result.append_children($2); }
     | statements                                 { result = sqf::sqc::bison::astnode{}; result.append_children($1); }
     ;

filehead: "params" funchead                       { $$ = $2; }
        ;

statements: statement                             { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
          | statement statements                  { $$ = sqf::sqc::bison::astnode{}; $$.append($1); $$.append_children($2); }
          ;

statement: "return" exp01 ";"                     { $$ = sqf::sqc::bison::astnode{ astkind::RETURN, tokenizer.create_token() }; $$.append($2); }
         | "return" ";"                           { $$ = sqf::sqc::bison::astnode{ astkind::RETURN, tokenizer.create_token() }; }
         | "throw" exp01 ";"                      { $$ = sqf::sqc::bison::astnode{ astkind::THROW, tokenizer.create_token() }; $$.append($2); }
         | vardecl ";"                            { $$ = $1; }
         | funcdecl                               { $$ = $1; }
         | if                                     { $$ = $1; }
         | for                                    { $$ = $1; }
         | while                                  { $$ = $1; }
         | trycatch                               { $$ = $1; }
         | switch                                 { $$ = $1; }
         | assignment ";"                         { $$ = $1; }
         | exp01 ";"                              { $$ = $1; }
         | ";"                                    { $$ = sqf::sqc::bison::astnode{}; }
         | error                                  { $$ = sqf::sqc::bison::astnode{}; }
         ;

assignment: IDENT "=" exp01                       { $$ = sqf::sqc::bison::astnode{ astkind::ASSIGNMENT, tokenizer.create_token() }; $$.append($1); $$.append($3); }
          | arrget "=" exp01                      { $$ = sqf::sqc::bison::astnode{ astkind::OP_ARRAY_SET, tokenizer.create_token() }; $$.append($1); $$.append($3); }
          ;

vardecl: "let" IDENT "=" exp01                    { $$ = sqf::sqc::bison::astnode{ astkind::DECLARATION, tokenizer.create_token() }; $$.append($2); $$.append($4); }
       | "let" IDENT "be" exp01                   { $$ = sqf::sqc::bison::astnode{ astkind::DECLARATION, tokenizer.create_token() }; $$.append($2); $$.append($4); }
       | "let" IDENT                              { $$ = sqf::sqc::bison::astnode{ astkind::FORWARD_DECLARATION, tokenizer.create_token() }; $$.append($2); }
       | "private" IDENT "=" exp01                { $$ = sqf::sqc::bison::astnode{ astkind::DECLARATION, tokenizer.create_token() }; $$.append($2); $$.append($4); }
       | "private" IDENT "be" exp01               { $$ = sqf::sqc::bison::astnode{ astkind::DECLARATION, tokenizer.create_token() }; $$.append($2); $$.append($4); }
       | "private" IDENT                          { $$ = sqf::sqc::bison::astnode{ astkind::FORWARD_DECLARATION, tokenizer.create_token() }; $$.append($2); }
       ;

funcdecl: "function" IDENT funchead codeblock         { $$ = sqf::sqc::bison::astnode{ astkind::FUNCTION_DECLARATION, tokenizer.create_token() }; $$.append($2); $$.append($3); $$.append($4); }
        | "final" "function" IDENT funchead codeblock { $$ = sqf::sqc::bison::astnode{ astkind::FINAL_FUNCTION_DECLARATION, tokenizer.create_token() }; $$.append($3); $$.append($4); $$.append($5); }
        ;

function: "function" funchead codeblock           { $$ = sqf::sqc::bison::astnode{ astkind::FUNCTION, tokenizer.create_token() }; $$.append($2); $$.append($3); }
        ;

funchead: "(" ")"                                 { $$ = sqf::sqc::bison::astnode{ astkind::ARGLIST, tokenizer.create_token() }; }
        | "(" arglist ")"                         { $$ = sqf::sqc::bison::astnode{ astkind::ARGLIST, tokenizer.create_token() }; $$.append_children($2); }
        ;

arglist: argitem                                  { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
       | argitem ","                              { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
       | argitem "," arglist                      { $$ = sqf::sqc::bison::astnode{}; $$.append($1); $$.append_children($3); }
       ;
argitem: IDENT                                    { $$ = sqf::sqc::bison::astnode{ astkind::ARGITEM, $1 }; }
       | IDENT "=" exp01                          { $$ = sqf::sqc::bison::astnode{ astkind::ARGITEM_DEFAULT, $1 }; $$.append($3); }
       | IDENT IDENT                              { $$ = sqf::sqc::bison::astnode{ astkind::ARGITEM_TYPE, $2 }; $$.append($1); }
       | IDENT IDENT "=" exp01                    { $$ = sqf::sqc::bison::astnode{ astkind::ARGITEM_TYPE_DEFAULT, $2 }; $$.append($1); $$.append($4); }
       ;
codeblock: statement                              { $$ = sqf::sqc::bison::astnode{ astkind::CODEBLOCK, tokenizer.create_token() }; $$.append($1); }
         | "{" "}"                                { $$ = sqf::sqc::bison::astnode{ astkind::CODEBLOCK, tokenizer.create_token() }; }
         | "{" statements "}"                     { $$ = sqf::sqc::bison::astnode{ astkind::CODEBLOCK, tokenizer.create_token() }; $$.append_children($2); }
         ;

if: "if" "(" exp01 ")" codeblock                  { $$ = sqf::sqc::bison::astnode{ astkind::IF, tokenizer.create_token() }; $$.append($3); $$.append($5); }
  | "if" "(" exp01 ")" codeblock "else" codeblock { $$ = sqf::sqc::bison::astnode{ astkind::IFELSE, tokenizer.create_token() }; $$.append($3); $$.append($5); $$.append($7); }
  ;

for: "for" IDENT "from" exp01 "to" exp01 codeblock                { $$ = sqf::sqc::bison::astnode{ astkind::FOR, tokenizer.create_token() }; $$.append($2); $$.append($4); $$.append($6); $$.append($7); }
   | "for" IDENT "from" exp01 "to" exp01 "step" exp01 codeblock   { $$ = sqf::sqc::bison::astnode{ astkind::FORSTEP, tokenizer.create_token() }; $$.append($2); $$.append($4); $$.append($6); $$.append($8); $$.append($9); }
   | "for" "(" IDENT ":" exp01 ")" codeblock                      { $$ = sqf::sqc::bison::astnode{ astkind::FOREACH, $4 }; $$.append($3); $$.append($5); $$.append($7); }
   ;

while: "while" "(" exp01 ")" codeblock                    { $$ = sqf::sqc::bison::astnode{ astkind::WHILE, tokenizer.create_token() }; $$.append($3); $$.append($5); }
     | "do" codeblock "while" "(" exp01 ")"               { $$ = sqf::sqc::bison::astnode{ astkind::DOWHILE, tokenizer.create_token() }; $$.append($2); $$.append($5); }
     ;

trycatch: "try" codeblock "catch" "(" IDENT ")" codeblock { $$ = sqf::sqc::bison::astnode{ astkind::TRYCATCH, tokenizer.create_token() }; $$.append($2); $$.append($5); $$.append($7); }
        ;

switch: "switch" "(" exp01 ")" "{" caselist "}"           { $$ = sqf::sqc::bison::astnode{ astkind::SWITCH, tokenizer.create_token() }; $$.append($3); $$.append_children($6); }
      ;

caselist: case                        { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
        | case caselist               { $$ = sqf::sqc::bison::astnode{}; $$.append($1); $$.append_children($2); }
        ;

case: "case" exp01 ":" codeblock      { $$ = sqf::sqc::bison::astnode{ astkind::CASE, $3 }; $$.append($2); $$.append($4); }
    | "case" exp01 ":"                { $$ = sqf::sqc::bison::astnode{ astkind::CASE, $3 }; $$.append($2); }
    | "default" ":" codeblock         { $$ = sqf::sqc::bison::astnode{ astkind::CASE_DEFAULT, $2 }; $$.append($3); }
    ;

exp01: exp02                          { $$ = $1; }
     | exp02 "?" exp01 ":" exp01      { $$ = sqf::sqc::bison::astnode{ astkind::OP_TERNARY, $4 }; $$.append($1); $$.append($3); $$.append($5); }
     ;
exp02: exp03                          { $$ = $1; }
     | exp03 "||" exp03               { $$ = sqf::sqc::bison::astnode{ astkind::OP_OR, $2 }; $$.append($1); $$.append($3); }
     ;
exp03: exp04                          { $$ = $1; }
     | exp04 "&&" exp04               { $$ = sqf::sqc::bison::astnode{ astkind::OP_AND, $2 }; $$.append($1); $$.append($3); }
     ;
exp04: exp05                          { $$ = $1; }
     | exp05 "===" exp05              { $$ = sqf::sqc::bison::astnode{ astkind::OP_EQUALEXACT, $2 }; $$.append($1); $$.append($3); }
     | exp05 "!==" exp05              { $$ = sqf::sqc::bison::astnode{ astkind::OP_NOTEQUALEXACT, $2 }; $$.append($1); $$.append($3); }
     | exp05 "==" exp05               { $$ = sqf::sqc::bison::astnode{ astkind::OP_EQUAL, $2 }; $$.append($1); $$.append($3); }
     | exp05 "!=" exp05               { $$ = sqf::sqc::bison::astnode{ astkind::OP_NOTEQUAL, $2 }; $$.append($1); $$.append($3); }
     ;
exp05: exp06                          { $$ = $1; }
     | exp06 "<"  exp06               { $$ = sqf::sqc::bison::astnode{ astkind::OP_LESSTHAN, $2 }; $$.append($1); $$.append($3); }
     | exp06 "<=" exp06               { $$ = sqf::sqc::bison::astnode{ astkind::OP_LESSTHANEQUAL, $2 }; $$.append($1); $$.append($3); }
     | exp06 ">"  exp06               { $$ = sqf::sqc::bison::astnode{ astkind::OP_GREATERTHAN, $2 }; $$.append($1); $$.append($3); }
     | exp06 ">=" exp06               { $$ = sqf::sqc::bison::astnode{ astkind::OP_GREATERTHANEQUAL, $2 }; $$.append($1); $$.append($3); }
     ;
exp06: exp07                          { $$ = $1; }
     | exp07 "+" exp07                { $$ = sqf::sqc::bison::astnode{ astkind::OP_PLUS, $2 }; $$.append($1); $$.append($3); }
     | exp07 "-" exp07                { $$ = sqf::sqc::bison::astnode{ astkind::OP_MINUS, $2 }; $$.append($1); $$.append($3); }
     ;
exp07: exp08                          { $$ = $1; }
     | exp08 "*" exp08                { $$ = sqf::sqc::bison::astnode{ astkind::OP_MULTIPLY, $2 }; $$.append($1); $$.append($3); }
     | exp08 "/" exp08                { $$ = sqf::sqc::bison::astnode{ astkind::OP_DIVIDE, $2 }; $$.append($1); $$.append($3); }
     | exp08 "%" exp08                { $$ = sqf::sqc::bison::astnode{ astkind::OP_REMAINDER, $2 }; $$.append($1); $$.append($3); }
     ;
exp08: exp09                          { $$ = $1; }
     | "!" exp09                      { $$ = sqf::sqc::bison::astnode{ astkind::OP_NOT, $1 }; $$.append($2);  }
     ;
exp09: expp                           { $$ = $1; }
     | expp "." IDENT "(" explist ")" { $$ = sqf::sqc::bison::astnode{ astkind::OP_BINARY, $3 }; $$.append($1); $$.append($3); $$.append($5); }
     | arrget                         { $$ = $1; }
     ;
arrget: exp09 "[" exp01 "]"           { $$ = sqf::sqc::bison::astnode{ astkind::OP_ARRAY_GET, tokenizer.create_token() }; $$.append($1); $$.append($3); }
      ;
expp: "(" exp01 ")"                   { $$ = $2; }
    | IDENT "(" explist ")"           { $$ = sqf::sqc::bison::astnode{ astkind::OP_UNARY, $1 }; $$.append($1); $$.append($3); }
    | IDENT                           { $$ = sqf::sqc::bison::astnode{ astkind::GET_VARIABLE, $1 }; }
    | value                           { $$ = $1; }
    ;
value: function                       { $$ = $1; }
     | STRING                         { $$ = sqf::sqc::bison::astnode{ astkind::VAL_STRING, $1 }; }
     | array                          { $$ = $1; }
     | NUMBER                         { $$ = sqf::sqc::bison::astnode{ astkind::VAL_NUMBER, $1 }; }
     | "true"                         { $$ = sqf::sqc::bison::astnode{ astkind::VAL_TRUE, tokenizer.create_token() }; }
     | "false"                        { $$ = sqf::sqc::bison::astnode{ astkind::VAL_FALSE, tokenizer.create_token() }; }
     | "nil"                          { $$ = sqf::sqc::bison::astnode{ astkind::VAL_NIL, tokenizer.create_token() }; }
     ;
array: "[" "]"                        { $$ = sqf::sqc::bison::astnode{ astkind::VAL_ARRAY, tokenizer.create_token() }; }
     | "[" explist "]"                { $$ = sqf::sqc::bison::astnode{ astkind::VAL_ARRAY, tokenizer.create_token() }; $$.append_children($2); }
     ;
explist: exp01                        { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
       | exp01 ","                    { $$ = sqf::sqc::bison::astnode{}; $$.append($1); }
       | exp01 "," explist            { $$ = sqf::sqc::bison::astnode{}; $$.append($1); $$.append_children($3); }
       ;

%%

#include "sqc_parser.h"
namespace sqf::sqc::bison
{
     void parser::error (const location_type& loc, const std::string& msg)
     {
          actual.__log(logmessage::sqf::ParseError({ fpath, loc.begin.line, loc.begin.column }, msg));
     }
     inline parser::symbol_type yylex (sqf::sqc::tokenizer& tokenizer)
     {
         auto token = tokenizer.next();
         parser::location_type loc;
         loc.begin.line = token.line;
         loc.begin.column = token.column;
         loc.end.line = token.line;
         loc.end.column = token.column + token.contents.length();

         switch (token.type)
         {
         case tokenizer::etoken::eof: return parser::make_NA(loc);
         case tokenizer::etoken::invalid: return parser::make_NA(loc);
         case tokenizer::etoken::m_line: return yylex(tokenizer);
         case tokenizer::etoken::i_comment_line: return yylex(tokenizer);
         case tokenizer::etoken::i_comment_block: return yylex(tokenizer);
         case tokenizer::etoken::i_whitespace: return yylex(tokenizer);

         case tokenizer::etoken::t_return: return parser::make_RETURN(loc);
         case tokenizer::etoken::t_throw: return parser::make_THROW(loc);
         case tokenizer::etoken::t_let: return parser::make_LET(loc);
         case tokenizer::etoken::t_be: return parser::make_BE(loc);
         case tokenizer::etoken::t_break: return parser::make_BREAK(loc);
         case tokenizer::etoken::t_function: return parser::make_FUNCTION(loc);
         case tokenizer::etoken::t_final: return parser::make_FINAL(loc);
         case tokenizer::etoken::t_if: return parser::make_IF(loc);
         case tokenizer::etoken::t_else: return parser::make_ELSE(loc);
         case tokenizer::etoken::t_from: return parser::make_FROM(loc);
         case tokenizer::etoken::t_to: return parser::make_TO(loc);
         case tokenizer::etoken::t_step: return parser::make_STEP(loc);
         case tokenizer::etoken::t_while: return parser::make_WHILE(loc);
         case tokenizer::etoken::t_do: return parser::make_DO(loc);
         case tokenizer::etoken::t_try: return parser::make_TRY(loc);
         case tokenizer::etoken::t_catch: return parser::make_CATCH(loc);
         case tokenizer::etoken::t_switch: return parser::make_SWITCH(loc);
         case tokenizer::etoken::t_case: return parser::make_CASE(loc);
         case tokenizer::etoken::t_default: return parser::make_DEFAULT(loc);
         case tokenizer::etoken::t_nil: return parser::make_NIL(loc);
         case tokenizer::etoken::t_true: return parser::make_TRUE(loc);
         case tokenizer::etoken::t_false: return parser::make_FALSE(loc);
         case tokenizer::etoken::t_for: return parser::make_FOR(loc);
         case tokenizer::etoken::t_private: return parser::make_PRIVATE(loc);

         case tokenizer::etoken::s_curlyo: return parser::make_CURLYO(loc);
         case tokenizer::etoken::s_curlyc: return parser::make_CURLYC(loc);
         case tokenizer::etoken::s_roundo: return parser::make_ROUNDO(loc);
         case tokenizer::etoken::s_roundc: return parser::make_ROUNDC(loc);
         case tokenizer::etoken::s_edgeo: return parser::make_SQUAREO(loc);
         case tokenizer::etoken::s_edgec: return parser::make_SQUAREC(loc);
         case tokenizer::etoken::s_equalequalequal: return parser::make_EQUALEQUALEQUAL(token, loc);
         case tokenizer::etoken::s_equalequal: return parser::make_EQUALEQUAL(token, loc);
         case tokenizer::etoken::s_equal: return parser::make_EQUAL(loc);
         case tokenizer::etoken::s_greaterthenequal: return parser::make_GTEQUAL(token, loc);
         case tokenizer::etoken::s_greaterthen: return parser::make_GT(token, loc);
         case tokenizer::etoken::s_lessthenequal: return parser::make_LTEQUAL(token, loc);
         case tokenizer::etoken::s_lessthen: return parser::make_LT(token, loc);
         case tokenizer::etoken::s_plus: return parser::make_PLUS(token, loc);
         case tokenizer::etoken::s_minus: return parser::make_MINUS(token, loc);
         case tokenizer::etoken::s_notequalequal: return parser::make_EXCLAMATIONMARKEQUALEQUAL(token, loc);
         case tokenizer::etoken::s_notequal: return parser::make_EXCLAMATIONMARKEQUAL(token, loc);
         case tokenizer::etoken::s_exclamationmark: return parser::make_EXCLAMATIONMARK(token, loc);
         case tokenizer::etoken::s_percent: return parser::make_PERCENT(token, loc);
         case tokenizer::etoken::s_star: return parser::make_STAR(token, loc);
         case tokenizer::etoken::s_slash: return parser::make_SLASH(token, loc);
         case tokenizer::etoken::s_andand: return parser::make_ANDAND(token, loc);
         case tokenizer::etoken::s_oror: return parser::make_VLINEVLINE(token, loc);
         case tokenizer::etoken::s_questionmark: return parser::make_QUESTIONMARK(loc);
         case tokenizer::etoken::s_colon: return parser::make_COLON(token, loc);
         case tokenizer::etoken::s_semicolon: return parser::make_SEMICOLON(loc);
         case tokenizer::etoken::s_comma: return parser::make_COMMA(loc);
         case tokenizer::etoken::s_dot: return parser::make_DOT(loc);

         case tokenizer::etoken::t_string: return parser::make_STRING(token, loc);
         case tokenizer::etoken::t_ident: return parser::make_IDENT(token, loc);
         case tokenizer::etoken::t_number: return parser::make_NUMBER(token, loc);
         default:
             return parser::make_NA(loc);
         }
     }
}