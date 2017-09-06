#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "int.h"
#include "calc-parser.hh"
#include "calc-driver.h"
#include "FlexLexer.h"
#include "calc-scanner.h"


Nat binop::eval()
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

Nat unop::eval()
{
	Nat v = 0;
	switch(opcode) {
		case op_not: v = ~l->eval(); break;
		case op_neg: v = -l->eval(); break;
		default: break;
	}
	return v;
}

Nat natural::eval()
{
	return *number;
}

node* calc_driver::newunop(op opcode, node *l)
{
	unop *a = new unop;
	a->opcode = opcode;
	a->l = std::unique_ptr<node>(l);
	return a;
}

node* calc_driver::newbinop(op opcode, node *l, node *r)
{
	binop *a = new binop;
	a->opcode = opcode;
	a->l = std::unique_ptr<node>(l);
	a->r = std::unique_ptr<node>(r);
	return a;
}

node* calc_driver::newnat(std::string str)
{
	natural *a = new natural;
	a->opcode = op_li;
	a->number = std::unique_ptr<Nat>(new Nat(str));
	return a;
}

node* calc_driver::lookup(std::string var)
{
	node *n = variables[var];
	if (!n) error("unknown symbol: " + var);
	return n;
}

int calc_driver::parse()
{
	calc_scanner scanner;
	yy::calc_parser parser(scanner, *this);
	return parser.parse();
}

void calc_driver::error(const yy::location& l, const std::string& m)
{
	std::cerr << l << ": " << m << std::endl;
	exit(1);
}

void calc_driver::error(const std::string& m)
{
	std::cerr << m << std::endl;
	exit(1);
}
