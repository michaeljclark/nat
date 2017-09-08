#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-driver.h"
#include "FlexLexer.h"
#include "nat-scanner.h"


Nat unaryop::eval(nat_driver *d)
{
	Nat v = 0;
	switch(opcode) {
		case op_not: v = ~l->eval(d); break;
		case op_neg: v = -l->eval(d); break;
		default: break;
	}
	return v;
}

Nat binaryop::eval(nat_driver *d)
{
	Nat v = 0;
	switch(opcode) {
		case op_and: v = l->eval(d) &  r->eval(d); break;
		case op_or:  v = l->eval(d) |  r->eval(d); break;
		case op_xor: v = l->eval(d) ^  r->eval(d); break;
		case op_eq:  v = l->eval(d) == r->eval(d); break;
		case op_ne:  v = l->eval(d) != r->eval(d); break;
		case op_lt:  v = l->eval(d) <  r->eval(d); break;
		case op_lte: v = l->eval(d) <= r->eval(d); break;
		case op_gt:  v = l->eval(d) >  r->eval(d); break;
		case op_gte: v = l->eval(d) >= r->eval(d); break;
		case op_srl: v = l->eval(d) >> r->eval(d).limb_at(0); break;
		case op_sll: v = l->eval(d) << r->eval(d).limb_at(0); break;
		case op_add: v = l->eval(d) +  r->eval(d); break;
		case op_sub: v = l->eval(d) -  r->eval(d); break;
		case op_mul: v = l->eval(d) *  r->eval(d); break;
		case op_div: v = l->eval(d) /  r->eval(d); break;
		case op_rem: v = l->eval(d) %  r->eval(d); break;
		case op_pow: v = l->eval(d).pow(r->eval(d).limb_at(0)); break;
		default: break;
	}
	return v;
}

Nat natural::eval(nat_driver *d)
{
	return *r;
}

Nat setvar::eval(nat_driver *d)
{
	d->variables[*l] = r.get();
	return r->eval(d);
}

Nat reg::eval(nat_driver *d)
{
	auto ri = d->registers.find(l);
	return ri != d->registers.end() ? ri->second : Nat(0);
}

Nat setreg::eval(nat_driver *d)
{
	Nat val = r->eval(d);
	d->registers[l] = val;
	return val;
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
	a->r = std::unique_ptr<Nat>(new Nat(str));
	return a;
}

node* nat_driver::new_variable(std::string str, node *r)
{
	setvar *a = new setvar;
	a->opcode = op_setvar;
	a->l = std::unique_ptr<std::string>(new std::string(str));
	a->r = std::unique_ptr<node>(r);
	variables[str] = a;
	return a;
}

node* nat_driver::lookup_variable(std::string var)
{
	node *n = variables[var];
	if (!n) error("unknown symbol: " + var);
	return n;
}

void nat_driver::add_toplevel(node *n)
{
	nodes.push_back(n);
}

int nat_driver::parse(std::istream &in)
{
	nat_scanner scanner;
	scanner.yyrestart(&in);
	yy::nat_parser parser(scanner, *this);
	return parser.parse();
}

void nat_driver::run(op opcode)
{
	for (auto n : nodes) {
		if (n->opcode != opcode) continue;
		switch (n->opcode) {
			case op_setvar:
				std::cout << " "
					<< *static_cast<setvar*>(n)->l
					<< " = " << n->eval(this).to_string() << std::endl;
				break;
			case op_setreg:
				std::cout << "_"
					<< static_cast<setreg*>(n)->l
					<< " = " << n->eval(this).to_string() << std::endl;
				break;
			default:
				break;
		}
	}
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
