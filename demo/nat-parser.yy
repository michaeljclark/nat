%skeleton "lalr1.cc"
%require "3.0.4"

%defines
%define parser_class_name {parser}
%define api.namespace {nat}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error verbose

%code requires
{
namespace nat {
	struct lexer;
	struct compiler;
	struct node;
}
}

%parse-param { lexer& scanner }
%parse-param { compiler& driver }
%lex-param { compiler& driver }

%locations

%code
{
#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <string>

#include "nat.h"
#include "FlexLexer.h"
#include "nat-compiler.h"
#include "nat-scanner.h"

#undef yylex
#define yylex scanner.yylex
}

%token
  END  0  "end of file"
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
  ADD     "+"
  SUB     "-"
  MUL     "*"
  DIV     "/"
  REM     "%"
  LPAREN  "("
  RPAREN  ")"
  POW     "**"
;

%token <std::string> NUMBER "number"
%token <std::string> IDENTIFIER "identifier"
%type <node*> assignment
%type <node*> expr

%%

unit:
	| unit assignment end   { driver.add_toplevel($2); }
	;

end:  NEWLINE
	| END
	;

assignment:
	  "identifier" "=" expr { $$ = driver.set_variable($1, $3); };

%left "|";
%left "^";
%left "&";
%left "==" "!=";
%left "<" "<=" ">" ">=";
%left "<<" ">>";
%left "+" "-";
%left "*" "/" "%";
%left "~";

expr:
	  expr "|" expr  { $$ = driver.new_binary(op_or,   $1, $3); }
	| expr "&" expr  { $$ = driver.new_binary(op_and,  $1, $3); }
	| expr "^" expr  { $$ = driver.new_binary(op_xor,  $1, $3); }
	| expr "==" expr { $$ = driver.new_binary(op_seq,  $1, $3); }
	| expr "!=" expr { $$ = driver.new_binary(op_sne,  $1, $3); }
	| expr "<" expr  { $$ = driver.new_binary(op_slt,  $1, $3); }
	| expr "<=" expr { $$ = driver.new_binary(op_slte, $1, $3); }
	| expr ">" expr  { $$ = driver.new_binary(op_sgt,  $1, $3); }
	| expr ">=" expr { $$ = driver.new_binary(op_sgte, $1, $3); }
	| expr ">>" expr { $$ = driver.new_binary(op_srl,  $1, $3); }
	| expr "<<" expr { $$ = driver.new_binary(op_sll,  $1, $3); }
	| expr "+" expr  { $$ = driver.new_binary(op_add,  $1, $3); }
	| expr "-" expr  { $$ = driver.new_binary(op_sub,  $1, $3); }
	| expr "*" expr  { $$ = driver.new_binary(op_mul,  $1, $3); }
	| expr "/" expr  { $$ = driver.new_binary(op_div,  $1, $3); }
	| expr "%" expr  { $$ = driver.new_binary(op_rem,  $1, $3); }
	| "(" expr ")"   { $$ = std::move($2); }
	| "identifier"   { $$ = driver.get_variable($1); }
	| "~" expr       { $$ = driver.new_unary(op_not,  $2); }
	| "-" expr       { $$ = driver.new_unary(op_neg,  $2); }
	| "number"       { $$ = driver.new_const_int($1); }
	;

%%

void
nat::parser::error (const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
