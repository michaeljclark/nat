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
#include <memory>
#include <string>

#include "int.h"
#include "FlexLexer.h"
#include "calc-driver.h"
#include "calc-scanner.h"

#undef yylex
#define yylex scanner.yylex
}

%token
  EOL  0  "end of file"
  NEWLINE "\n"
  ASSIGN  "="
  AND     "&"
  OR      "|"
  XOR     "^"
  NOT     "~"
  EQ      "=="
  NE      "!="
  LT      "<"
  LTE     "<="
  GT      ">"
  GTE     ">="
  RSHIFT  ">>"
  LSHIFT  "<<"
  MINUS   "-"
  PLUS    "+"
  MUL     "*"
  DIV     "/"
  REM     "%"
  LPAREN  "("
  RPAREN  ")"
  POW     "**"
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

%left "|";
%left "^";
%left "&";
%left "==" "!=";
%left "<" "<=" ">" ">=";
%left "<<" ">>";
%left "+" "-";
%left "*" "/" "%";
%left "~";
%left "**";

expr:
	  expr "|" expr  { $$ = driver.newbinop(op_or,  $1, $3); }
	| expr "&" expr  { $$ = driver.newbinop(op_and, $1, $3); }
	| expr "^" expr  { $$ = driver.newbinop(op_xor, $1, $3); }
	| expr "==" expr { $$ = driver.newbinop(op_eq,  $1, $3); }
	| expr "!=" expr { $$ = driver.newbinop(op_ne,  $1, $3); }
	| expr "<" expr  { $$ = driver.newbinop(op_lt,  $1, $3); }
	| expr "<=" expr { $$ = driver.newbinop(op_lte, $1, $3); }
	| expr ">" expr  { $$ = driver.newbinop(op_gt,  $1, $3); }
	| expr ">=" expr { $$ = driver.newbinop(op_gte, $1, $3); }
	| expr ">>" expr { $$ = driver.newbinop(op_srl, $1, $3); }
	| expr "<<" expr { $$ = driver.newbinop(op_sll, $1, $3); }
	| expr "+" expr  { $$ = driver.newbinop(op_add, $1, $3); }
	| expr "-" expr  { $$ = driver.newbinop(op_sub, $1, $3); }
	| expr "*" expr  { $$ = driver.newbinop(op_mul, $1, $3); }
	| expr "/" expr  { $$ = driver.newbinop(op_div, $1, $3); }
	| expr "%" expr  { $$ = driver.newbinop(op_rem, $1, $3); }
	| "(" expr ")"   { $$ = std::move($2); }
	| "identifier"   { $$ = driver.lookup($1); }
	| "~" expr       { $$ = driver.newunop(op_not, $2); }
	| "-" expr       { $$ = driver.newunop(op_neg, $2); }
	| expr "**" expr { $$ = driver.newbinop(op_pow, $1, $3); }
	| "number"       { $$ = driver.newnat($1); }
	;

%%

void
yy::calc_parser::error (const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
