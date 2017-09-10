#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-driver.h"
#include "FlexLexer.h"
#include "nat-scanner.h"


/*
 * contstants
 */

static const char* op_name[] = {
	"none",
	"var",
	"setvar",
	"reg",
	"setreg",
	"li",
	"and",
	"or",
	"xor",
	"seq",
	"sne",
	"slt",
	"slte",
	"sgt",
	"sgte",
	"srl",
	"sll",
	"add",
	"sub",
	"mul",
	"div",
	"rem",
	"not",
	"neg",
	"pow"
};


/*
 * node constructors
 */

node::node(op opcode)
	: opcode(opcode) {}

unaryop::unaryop(op opcode, node *l)
	: node(opcode),
	  l(std::unique_ptr<node>(l)) {}

binaryop::binaryop(op opcode, node *l, node *r)
	: node(opcode),
	  l(std::unique_ptr<node>(l)),
	  r(std::unique_ptr<node>(r)) {}

natural::natural(std::string r)
	: node(op_li),
	  r(std::unique_ptr<Nat>(new Nat(r))) {}

natural::natural(Nat &n)
	: node(op_li),
	  r(std::unique_ptr<Nat>(new Nat(n))) {}

var::var(std::string l)
	: node(op_var),
	  l(std::unique_ptr<std::string>(new std::string(l))) {}

setvar::setvar(std::string l, node *r)
	: node(op_setvar),
	  l(std::unique_ptr<std::string>(new std::string(l))),
	  r(std::unique_ptr<node>(r)) {}

reg::reg(size_t l)
	: node(op_reg), l(l) {}

setreg::setreg(size_t l, node *r)
	: node(op_setreg), l(l),
	  r(std::unique_ptr<node>(r)) {}


/*
 * evalulate expressions recursively
 */

Nat unaryop::eval(nat_driver *d)
{
	Nat v = 0;
	switch(opcode) {
		case op_not:  v = ~l->eval(d); break;
		case op_neg:  v = -l->eval(d); break;
		default: break;
	}
	return v;
}

Nat binaryop::eval(nat_driver *d)
{
	Nat v = 0;
	switch(opcode) {
		case op_and:  v = l->eval(d) &  r->eval(d); break;
		case op_or:   v = l->eval(d) |  r->eval(d); break;
		case op_xor:  v = l->eval(d) ^  r->eval(d); break;
		case op_seq:  v = l->eval(d) == r->eval(d); break;
		case op_sne:  v = l->eval(d) != r->eval(d); break;
		case op_slt:  v = l->eval(d) <  r->eval(d); break;
		case op_slte: v = l->eval(d) <= r->eval(d); break;
		case op_sgt:  v = l->eval(d) >  r->eval(d); break;
		case op_sgte: v = l->eval(d) >= r->eval(d); break;
		case op_srl:  v = l->eval(d) >> r->eval(d).limb_at(0); break;
		case op_sll:  v = l->eval(d) << r->eval(d).limb_at(0); break;
		case op_add:  v = l->eval(d) +  r->eval(d); break;
		case op_sub:  v = l->eval(d) -  r->eval(d); break;
		case op_mul:  v = l->eval(d) *  r->eval(d); break;
		case op_div:  v = l->eval(d) /  r->eval(d); break;
		case op_rem:  v = l->eval(d) %  r->eval(d); break;
		case op_pow:  v = l->eval(d).pow(r->eval(d).limb_at(0)); break;
		default: break;
	}
	return v;
}

Nat natural::eval(nat_driver *d)
{
	return *r;
}

Nat var::eval(nat_driver *d)
{
	auto vi = d->variables.find(*l);
	return vi != d->variables.end() ? vi->second->eval(d) : Nat(0);
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


/*
 * lower tree into single static assignment tuples
 */

node_list unaryop::lower(nat_driver *d)
{
	/* get register of operand */
	node_list ll = l->lower(d);
	reg *lreg = new reg(d->lower_reg(ll));

	/* create new unary op using registers */
	unaryop *op = new unaryop(opcode, lreg);
	setreg *sr = new setreg(d->regnum++, op);
	d->registers[sr->l] = Nat(0);

	/* construct node list */
	node_list nodes;
	nodes.insert(nodes.end(), ll.begin(), ll.end());
	nodes.insert(nodes.end(), sr);

	return nodes;
}

node_list binaryop::lower(nat_driver *d)
{
	/* get registers of operands */
	node_list ll = l->lower(d);
	node_list rl = r->lower(d);
	reg *lreg = new reg(d->lower_reg(ll));
	reg *rreg = new reg(d->lower_reg(rl));

	/* create binary op using registers */
	binaryop *op = new binaryop(opcode, lreg, rreg);
	setreg *sr = new setreg(d->regnum++, op);
	d->registers[sr->l] = Nat(0);

	/* construct node list */
	node_list nodes;
	nodes.insert(nodes.end(), ll.begin(), ll.end());
	nodes.insert(nodes.end(), rl.begin(), rl.end());
	nodes.insert(nodes.end(), sr);

	return nodes;
}

node_list natural::lower(nat_driver *d)
{
	/* move the number into a register */
	natural *op = new natural(*r);
	setreg *sr = new setreg(d->regnum++, op);
	d->registers[sr->l] = Nat(0);

	return node_list{sr};
}

node_list var::lower(nat_driver *d)
{
	/* return most recent register for this variable */
	size_t regnum = d->varssa[*l];
	return node_list{new reg(regnum)};
}

node_list setvar::lower(nat_driver *d)
{
	/* store most recent register for this variable */
	node_list rl = r->lower(d);
	auto sr = static_cast<setreg*>(rl.back());
	sr->v = std::unique_ptr<var>(new var(*l));
	size_t regnum = sr->l;
	d->varssa[*l] = regnum;
	return rl;
}

node_list reg::lower(nat_driver *d)
{
	return node_list();
}

node_list setreg::lower(nat_driver *d)
{
	return node_list();
}


/*
 * convert nodes to string
 */

std::string unaryop::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " " + l->to_string(d) + ")";
}

std::string binaryop::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " " + l->to_string(d) + ", " + r->to_string(d) + ")";
}

std::string natural::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " " + r->to_string(16) + ")";
}

std::string var::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " '" + *l + "')";
}

std::string setvar::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " '" + *l + "', " + r->to_string(d) + ")";
}

std::string reg::to_string(nat_driver *d)
{
	return std::string("_") + std::to_string(l);
}

std::string setreg::to_string(nat_driver *d)
{
	return std::string("(") + op_name[opcode] + " _" + std::to_string(l) + ", " + r->to_string(d) + ")";
}


/*
 * driver methods called by parser
 */

nat_driver::nat_driver() : regnum(0) {}

node* nat_driver::new_unary(op opcode, node *l)
{
	return new unaryop(opcode, l);
}

node* nat_driver::new_binary(op opcode, node *l, node *r)
{
	return new binaryop(opcode, l, r);
}

node* nat_driver::new_natural(std::string num)
{
	return new natural(num);
}

node* nat_driver::set_variable(std::string name, node *r)
{
	setvar *a = new setvar(name, r);
	variables[name] = a;
	return a;
}

node* nat_driver::get_variable(std::string name)
{
	auto vi = variables.find(name);
	if (vi == variables.end()) error("unknown variable: " + name);
	var *a = new var(name);
	return a;
}

void nat_driver::add_toplevel(node *n)
{
	nodes.push_back(n);
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


/*
 * client interface
 */

int nat_driver::parse(std::istream &in)
{
	nat_scanner scanner;
	scanner.yyrestart(&in);
	yy::nat_parser parser(scanner, *this);
	return parser.parse();
}

void nat_driver::usedef()
{
	size_t def_use_size = nodes.size() * regnum;
	def_use = std::unique_ptr<char[]>(new char[def_use_size]);
	memset(def_use.get(), ' ', def_use_size);
	for (size_t i = 0; i < nodes.size(); i++) {
		node *ndef = nodes[i];
		if (ndef->opcode != op_setreg) continue;
		setreg *srdef = static_cast<setreg*>(ndef);
		size_t defreg = srdef->l;
		def_use[i * regnum + defreg] = 'v';
		for (size_t j = i + 1; j < nodes.size(); j++) {
			if (nodes[j]->opcode != op_setreg) continue;
			setreg *sruse = static_cast<setreg*>(nodes[j]);
			node *sruse_op = sruse->r.get();
			if (typeid(*sruse_op) == typeid(unaryop)) {
				unaryop *use_op = static_cast<unaryop*>(sruse_op);
				node *l = use_op->l.get();
				if (typeid(*l) == typeid(reg)) {
					size_t usereg = static_cast<reg*>(l)->l;
					if (usereg == defreg) {
						def_use[j * regnum + defreg] = '+';
						for (size_t k = j - 1; k != i; k--) {
							if (def_use[k * regnum + defreg] == ' ') {
								def_use[k * regnum + defreg] = '|';
							}
						}
					}
				}
			} else if (typeid(*sruse_op) == typeid(binaryop)) {
				binaryop *use_op = static_cast<binaryop*>(sruse_op);
				node *l = use_op->l.get(), *r = use_op->r.get();
				if (typeid(*l) == typeid(reg)) {
					size_t usereg = static_cast<reg*>(l)->l;
					if (usereg == defreg) {
						def_use[j * regnum + defreg] = '+';
						for (size_t k = j - 1; k != i; k--) {
							if (def_use[k * regnum + defreg] == ' ') {
								def_use[k * regnum + defreg] = '|';
							}
						}
					}
				}
				if (typeid(*r) == typeid(reg)) {
					size_t usereg = static_cast<reg*>(r)->l;
					if (usereg == defreg) {
						def_use[j * regnum + defreg] = '+';
						for (size_t k = j - 1; k != i; k--) {
							if (def_use[k * regnum + defreg] == ' ') {
								def_use[k * regnum + defreg] = '|';
							}
						}
					}
				}
			}
		}
	}
}

void nat_driver::lower()
{
	node_list new_nodes;
	for (auto ni = nodes.begin(); ni != nodes.end(); ni++) {
		new_nodes.insert(new_nodes.end(), *ni);
		switch ((*ni)->opcode) {
			case op_setvar: {
				node_list lowered = (*ni)->lower(this);
				new_nodes.insert(new_nodes.end(), lowered.begin(), lowered.end());
				break;
			}
			default:
				break;
		}
	}
	nodes = std::move(new_nodes);
	usedef();
}

size_t nat_driver::lower_reg(node_list &l)
{
	size_t regnum;
	node *n = static_cast<node*>(l.back());
	switch(n->opcode) {
		case op_reg:
			regnum = static_cast<reg*>(n)->l;
			l.pop_back();
			break;
		case op_setreg:
			regnum = static_cast<setreg*>(n)->l;
			break;
		default:
			regnum = 0;
			error("expected reg or setreg node");
			break;
	}
	return regnum;
}

void nat_driver::run(op opcode)
{
	Nat num;
	for (auto n : nodes) {
		if (n->opcode != opcode) continue;
		switch (n->opcode) {
			case op_setvar:
				num = n->eval(this);
				std::cout << " "
					<< *static_cast<setvar*>(n)->l
					<< " = " << num.to_string(10)
					<< " (" << num.to_string(16) << ")" << std::endl;
				break;
			case op_setreg:
				num = n->eval(this);
				std::cout << "_"
					<< static_cast<setreg*>(n)->l
					<< " = " << num.to_string(10)
					<< " (" << num.to_string(16) << ")" << std::endl;
				break;
			default:
				break;
		}
	}
}

void nat_driver::dump(op opcode)
{
	for (size_t i = 0; i < nodes.size(); i++) {
		node *n = nodes[i];
		if (n->opcode != opcode) continue;
		switch (n->opcode) {
			case op_setvar: {
				std::cout << "\t" << n->to_string(this) << std::endl;
				break;
			}
			case op_setreg: {
				std::cout << "\t"
					<< std::left << std::setw(40) 
					<< n->to_string(this);
				for (size_t j = 0; j < regnum; j++) {
					std::cout << def_use[i * regnum + j];
				}
				std::cout << std::endl;
				break;
			}
			default:
				break;
		}
	}
}
