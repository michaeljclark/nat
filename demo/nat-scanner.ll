%{
#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <string>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-compiler.h"
#include "nat-scanner.h"

using namespace nat;

static location loc;
%}

%option noyywrap nounput batch debug
%option yyclass="lexer"

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+|0b[01]+|0x[0-9A-Fa-f]+
ws    [ \t]+
eol   \n

%{
  #define YY_USER_ACTION loc.columns (yyleng);
%}

%%

"="|"←"     return parser::make_ASSIGN(loc);
"&"|"∧"     return parser::make_AND(loc);
"|"|"∨"     return parser::make_OR(loc);
"^"|"⊻"     return parser::make_XOR(loc);
"~"|"¬"     return parser::make_NOT(loc);
"=="        return parser::make_EQ(loc);
"!="|"≠"    return parser::make_NE(loc);
"<"         return parser::make_LT(loc);
"<="|"≤"    return parser::make_LTE(loc);
">"         return parser::make_GT(loc);
">="|"≥"    return parser::make_GTE(loc);
">>"|"»"    return parser::make_RSHIFT(loc);
"<<"|"«"    return parser::make_LSHIFT(loc);
"+"         return parser::make_ADD(loc);
"-"         return parser::make_SUB(loc);
"*"|"×"     return parser::make_MUL(loc);
"/"|"÷"     return parser::make_DIV(loc);
"%"|"mod"   return parser::make_REM(loc);
"("         return parser::make_LPAREN(loc);
")"         return parser::make_RPAREN(loc);
{int}       return parser::make_NUMBER(yytext, loc);
{id}        return parser::make_IDENTIFIER(yytext, loc);
{ws}        loc.step();
{eol}       loc.lines(yyleng); loc.step(); return parser::make_NEWLINE(loc);
<<EOF>>     return parser::make_END(loc);
.           driver.error (loc, "invalid character");
%%
