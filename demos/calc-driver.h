#pragma once

#undef YY_DECL
#define YY_DECL yy::calc_parser::symbol_type calc_scanner::yylex(calc_driver &driver)

struct node
{
	int nodetype;

	virtual ~node() {}
	virtual Int eval() = 0;
};

struct unop : node
{
	std::unique_ptr<node> l;

	virtual Int eval();
};

struct binop : node
{
	std::unique_ptr<node> l, r;

	virtual Int eval();
};

struct numval : node
{
	std::unique_ptr<Int> number;

	virtual Int eval();
};

struct calc_driver
{
	std::map<std::string,node*> variables;

	node* newunop(int nodetype, node *l);
	node* newbinop(int nodetype, node *l, node *r);
	node* newnum(std::string str);
	node* lookup(std::string var);

	int parse();
	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);
};
