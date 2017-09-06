%{
#include <map>
#include <memory>
#include <string>

#include "int.h"
#include "calc-parser.hh"
#include "calc-driver.h"
#include "calc-scanner.h"

static yy::location loc;
%}

%option noyywrap nounput batch debug
%option yyclass="calc_scanner"

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+

%%
"="     return yy::calc_parser::make_ASSIGN(loc);
"&"     return yy::calc_parser::make_AND(loc);
"|"     return yy::calc_parser::make_OR(loc);
"^"     return yy::calc_parser::make_XOR(loc);
"~"     return yy::calc_parser::make_NOT(loc);
"<"     return yy::calc_parser::make_LT(loc);
"<="    return yy::calc_parser::make_LTE(loc);
">"     return yy::calc_parser::make_GT(loc);
">="    return yy::calc_parser::make_GTE(loc);
">>"    return yy::calc_parser::make_RSHIFT(loc);
"<<"    return yy::calc_parser::make_LSHIFT(loc);
"+"     return yy::calc_parser::make_PLUS(loc);
"-"     return yy::calc_parser::make_MINUS(loc);
"*"     return yy::calc_parser::make_MUL(loc);
"/"     return yy::calc_parser::make_DIV(loc);
"("     return yy::calc_parser::make_LPAREN(loc);
")"     return yy::calc_parser::make_RPAREN(loc);
"**"    return yy::calc_parser::make_POW(loc);
{int}   return yy::calc_parser::make_NUMBER(yytext, loc);
{id}    return yy::calc_parser::make_IDENTIFIER(yytext, loc);
[ \t]+  loc.step();
\n      loc.lines(yyleng); loc.step(); return yy::calc_parser::make_NEWLINE(loc);
<<EOF>> return yy::calc_parser::make_EOL(loc);
.       driver.error (loc, "invalid character");
%%
