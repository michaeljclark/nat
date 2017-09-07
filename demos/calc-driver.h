#pragma once

#undef YY_DECL
#define YY_DECL yy::calc_parser::symbol_type calc_scanner::yylex(calc_driver &driver)

enum op
{
	op_none,
	op_li,
	op_and,
	op_or,
	op_xor,
	op_eq,
	op_ne,
	op_lt,
	op_lte,
	op_gt,
	op_gte,
	op_srl,
	op_sll,
	op_add,
	op_sub,
	op_mul,
	op_div,
	op_rem,
	op_not,
	op_neg,
	op_pow,
};

struct node
{
	op opcode;
	virtual ~node() {}
	virtual Nat eval() = 0;
};

struct unaryop : node
{
	std::unique_ptr<node> l;
	virtual Nat eval();
};

struct binaryop : node
{
	std::unique_ptr<node> l, r;
	virtual Nat eval();
};

struct natural : node
{
	std::unique_ptr<Nat> number;
	virtual Nat eval();
};

struct calc_driver
{
	std::map<std::string,node*> variables;

	node* new_unary(op opcode, node *l);
	node* new_binary(op opcode, node *l, node *r);
	node* new_natural(std::string str);
	node* lookup(std::string var);

	void eval(node *n);
	int parse(std::istream &in);
	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);
};
