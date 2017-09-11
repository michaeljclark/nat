#pragma once

struct nat_scanner : yyFlexLexer
{
	using yyFlexLexer::yylex;

	virtual yy::nat_parser::symbol_type yylex(nat_compiler &driver);
};
