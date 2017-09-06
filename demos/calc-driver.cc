#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "int.h"
#include "calc-parser.hh"
#include "calc-driver.h"
#include "FlexLexer.h"
#include "calc-scanner.h"


Int binop::eval()
{
	Int v = 0;
	switch(this->nodetype) {
		case '+': v = this->l->eval() + this->r->eval(); break;
		case '-': v = this->l->eval() - this->r->eval(); break;
		case '*': v = this->l->eval() * this->r->eval(); break;
		case '/': v = this->l->eval() / this->r->eval(); break;
		case '^': v = this->l->eval().pow(this->r->eval().mag.limb_at(0)); break;
	}
	return v;
}

Int unop::eval()
{
	Int v = 0;
	switch(this->nodetype) {
		case 'M': v = -l->eval(); break;
	}
	return v;
}

Int numval::eval()
{
	Int v = 0;
	switch(this->nodetype) {
		case 'K': v = *number; break;
	}
	return v;
}

node* calc_driver::newunop(int nodetype, node *l)
{
	unop *a = new unop;
	a->nodetype = nodetype;
	a->l = std::unique_ptr<node>(l);
	return a;
}

node* calc_driver::newbinop(int nodetype, node *l, node *r)
{
	binop *a = new binop;
	a->nodetype = nodetype;
	a->l = std::unique_ptr<node>(l);
	a->r = std::unique_ptr<node>(r);
	return a;
}

node* calc_driver::newnum(std::string str)
{
	numval *a = new numval;
	a->nodetype = 'K';
	a->number = std::unique_ptr<Int>(new Int(str));
	return a;
}

node* calc_driver::lookup(std::string var)
{
	node *n = variables[var];
	if (!n) {
		error("unknown symbol: " + var);
	}
	return n;
}

int calc_driver::parse()
{
	calc_scanner scanner;
	yy::calc_parser parser(scanner, *this);
	int res = parser.parse();
	return res;
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
