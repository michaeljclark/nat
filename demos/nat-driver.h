#pragma once

#undef YY_DECL
#define YY_DECL yy::nat_parser::symbol_type nat_scanner::yylex(nat_driver &driver)

enum op
{
	op_none,
	op_setvar,
	op_setreg,
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

struct nat_driver;

struct node
{
	op opcode;
	virtual ~node() {}
	virtual Nat eval(nat_driver *) = 0;
};

struct unaryop : node
{
	std::unique_ptr<node> l;
	virtual Nat eval(nat_driver *);
};

struct binaryop : node
{
	std::unique_ptr<node> l, r;
	virtual Nat eval(nat_driver *);
};

struct natural : node
{
	std::unique_ptr<Nat> r;
	virtual Nat eval(nat_driver *);
};

struct setvar : node
{
	std::unique_ptr<std::string> l;
	std::unique_ptr<node> r;
	virtual Nat eval(nat_driver *);
};

struct reg : node
{
	size_t l;
	virtual Nat eval(nat_driver *);
};

struct setreg : node
{
	size_t l;
	std::unique_ptr<node> r;
	virtual Nat eval(nat_driver *);
};

struct nat_driver
{
	std::vector<node*> nodes;
	std::map<std::string,node*> variables;
	std::map<size_t,Nat> registers;

	node* new_unary(op opcode, node *l);
	node* new_binary(op opcode, node *l, node *r);
	node* new_natural(std::string str);
	node* new_variable(std::string str, node *r);
	node* lookup_variable(std::string var);

	void add_toplevel(node *n);

	int parse(std::istream &in);
	void run();

	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);
};
