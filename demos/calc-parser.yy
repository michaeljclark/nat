%skeleton "lalr1.cc"
%require "3.0.4"

%defines
%define parser_class_name {calc_parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error verbose

%code requires
{
#include "int.h"
struct calc_scanner;
struct calc_driver;
struct node;
}

%parse-param { calc_scanner& scanner }
%parse-param { calc_driver& driver }
%lex-param { calc_driver& driver }

%locations

%code
{
#include <map>
#include "int.h"
#include "calc-driver.h"
#include "FlexLexer.h"
#include "calc-scanner.h"

#undef yylex
#define yylex scanner.yylex
}

%token
  EOL  0  "end of file"
  NEWLINE "\n"
  ASSIGN  "="
  MINUS   "-"
  PLUS    "+"
  STAR    "*"
  SLASH   "/"
  LPAREN  "("
  RPAREN  ")"
  CARAT   "^"
;

%token <std::string> NUMBER "number"
%token <std::string> IDENTIFIER "identifier"
%type <node*> expr

%%

unit:
	| unit expr "\n" { printf("= %s\n", $2->eval().to_string().c_str()); };
	| unit assignment "\n"
	| unit "\n"
	;

assignment:
	  "identifier" "=" expr { driver.variables[$1] = $3; };

%left "+" "-";
%left "*" "/";
%left "^";

expr:
	  expr "+" expr  { $$ = driver.newbinop('+', $1, $3); }
	| expr "-" expr  { $$ = driver.newbinop('-', $1, $3); }
	| expr "*" expr  { $$ = driver.newbinop('*', $1, $3); }
	| expr "/" expr  { $$ = driver.newbinop('/', $1, $3); }
	| "(" expr ")"   { $$ = std::move($2); }
	| "identifier"   { $$ = driver.variables[$1]; }
	| "-" expr       { $$ = driver.newunop('M', $2); }
	| expr "^" expr  { $$ = driver.newbinop('^', $1, $3); }
	| "number"       { $$ = driver.newnum($1); }
	;

%%

void
yy::calc_parser::error (const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
