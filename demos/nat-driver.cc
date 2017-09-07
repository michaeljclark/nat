#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "int.h"
#include "nat-parser.hh"
#include "nat-driver.h"
#include "FlexLexer.h"
#include "nat-scanner.h"


Nat unaryop::eval()
{
	Nat v = 0;
	switch(opcode) {
		case op_not: v = ~l->eval(); break;
		case op_neg: v = -l->eval(); break;
		default: break;
	}
	return v;
}

Nat binaryop::eval()
{
	Nat v = 0;
	switch(opcode) {
		case op_and: v = this->l->eval() & this->r->eval(); break;
		case op_or:  v = this->l->eval() | this->r->eval(); break;
		case op_xor: v = this->l->eval() ^ this->r->eval(); break;
		case op_eq:  v = this->l->eval() == this->r->eval(); break;
		case op_ne:  v = this->l->eval() != this->r->eval(); break;
		case op_lt:  v = this->l->eval() < this->r->eval(); break;
		case op_lte: v = this->l->eval() <= this->r->eval(); break;
		case op_gt:  v = this->l->eval() > this->r->eval(); break;
		case op_gte: v = this->l->eval() >= this->r->eval(); break;
		case op_srl: v = this->l->eval() >> this->r->eval().limb_at(0); break;
		case op_sll: v = this->l->eval() << this->r->eval().limb_at(0); break;
		case op_add: v = this->l->eval() + this->r->eval(); break;
		case op_sub: v = this->l->eval() - this->r->eval(); break;
		case op_mul: v = this->l->eval() * this->r->eval(); break;
		case op_div: v = this->l->eval() / this->r->eval(); break;
		case op_rem: v = this->l->eval() % this->r->eval(); break;
		case op_pow: v = this->l->eval().pow(this->r->eval().limb_at(0)); break;
		default: break;
	}
	return v;
}

Nat natural::eval()
{
	return *number;
}

node* nat_driver::new_unary(op opcode, node *l)
{
	unaryop *a = new unaryop;
	a->opcode = opcode;
	a->l = std::unique_ptr<node>(l);
	return a;
}

node* nat_driver::new_binary(op opcode, node *l, node *r)
{
	binaryop *a = new binaryop;
	a->opcode = opcode;
	a->l = std::unique_ptr<node>(l);
	a->r = std::unique_ptr<node>(r);
	return a;
}

node* nat_driver::new_natural(std::string str)
{
	natural *a = new natural;
	a->opcode = op_li;
	a->number = std::unique_ptr<Nat>(new Nat(str));
	return a;
}

node* nat_driver::lookup(std::string var)
{
	node *n = variables[var];
	if (!n) error("unknown symbol: " + var);
	return n;
}

void nat_driver::eval(node *n)
{
	std::cout << "   = " << n->eval().to_string() << std::endl;
}

int nat_driver::parse(std::istream &in)
{
	nat_scanner scanner;
	scanner.yyrestart(&in);
	yy::nat_parser parser(scanner, *this);
	return parser.parse();
}

void nat_driver::error(const yy::location& l, const std::string& m)
{
	std::cerr << l << ": " << m << std::endl;
	exit(1);
}

void nat_driver::error(const std::string& m)
{
	std::cerr << m << std::endl;
	exit(1);
}
