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
  END  0  "end of file"
  NEWLINE "\n"
  ASSIGN  "<-"
  AND     "&"
  OR      "|"
  XOR     "^"
  NOT     "~"
  EQ      "="
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
	| unit expr end  { driver.eval($2); };
	| unit assignment end
	;

end:  NEWLINE
	| END
	;

assignment:
	  "identifier" "<-" expr { driver.variables[$1] = $3; };

%left "|";
%left "^";
%left "&";
%left "=" "!=";
%left "<" "<=" ">" ">=";
%left "<<" ">>";
%left "+" "-";
%left "*" "/" "%";
%left "~";
%left "**";

expr:
	  expr "|" expr  { $$ = driver.new_binary(op_or,  $1, $3); }
	| expr "&" expr  { $$ = driver.new_binary(op_and, $1, $3); }
	| expr "^" expr  { $$ = driver.new_binary(op_xor, $1, $3); }
	| expr "=" expr  { $$ = driver.new_binary(op_eq,  $1, $3); }
	| expr "!=" expr { $$ = driver.new_binary(op_ne,  $1, $3); }
	| expr "<" expr  { $$ = driver.new_binary(op_lt,  $1, $3); }
	| expr "<=" expr { $$ = driver.new_binary(op_lte, $1, $3); }
	| expr ">" expr  { $$ = driver.new_binary(op_gt,  $1, $3); }
	| expr ">=" expr { $$ = driver.new_binary(op_gte, $1, $3); }
	| expr ">>" expr { $$ = driver.new_binary(op_srl, $1, $3); }
	| expr "<<" expr { $$ = driver.new_binary(op_sll, $1, $3); }
	| expr "+" expr  { $$ = driver.new_binary(op_add, $1, $3); }
	| expr "-" expr  { $$ = driver.new_binary(op_sub, $1, $3); }
	| expr "*" expr  { $$ = driver.new_binary(op_mul, $1, $3); }
	| expr "/" expr  { $$ = driver.new_binary(op_div, $1, $3); }
	| expr "%" expr  { $$ = driver.new_binary(op_rem, $1, $3); }
	| "(" expr ")"   { $$ = std::move($2); }
	| "identifier"   { $$ = driver.lookup($1); }
	| "~" expr       { $$ = driver.new_unary(op_not, $2); }
	| "-" expr       { $$ = driver.new_unary(op_neg, $2); }
	| expr "**" expr { $$ = driver.new_binary(op_pow, $1, $3); }
	| "number"       { $$ = driver.new_natural($1); }
	;

%%

void
yy::calc_parser::error (const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
