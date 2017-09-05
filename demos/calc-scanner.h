#pragma once

struct calc_scanner : yyFlexLexer
{
	using yyFlexLexer::yylex;

	virtual yy::calc_parser::symbol_type yylex(calc_driver &driver);
};
