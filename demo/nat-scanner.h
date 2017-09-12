#pragma once

namespace nat {

	struct lexer : yyFlexLexer
	{
		using yyFlexLexer::yylex;

		virtual parser::symbol_type yylex(compiler &driver);
	};

}
