#include <map>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-compiler.h"
#include "FlexLexer.h"
#include "nat-scanner.h"


/*
 * constants
 */

static const char* op_name[] = {
	"none",
	"const_int",
	"var",
	"setvar",
	"ssareg",
	"phyreg",
	"setreg",
	"imm",
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
	"srli",
	"sll",
	"slli",
	"add",
	"addi",
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

const_int::const_int(std::string r)
	: node(op_const_int),
	  r(std::unique_ptr<Nat>(new Nat(r))) {}

const_int::const_int(Nat &n)
	: node(op_const_int),
	  r(std::unique_ptr<Nat>(new Nat(n))) {}

unaryop::unaryop(op opcode, node *l)
	: node(opcode),
	  l(std::unique_ptr<node>(l)) {}

binaryop::binaryop(op opcode, node *l, node *r)
	: node(opcode),
	  l(std::unique_ptr<node>(l)),
	  r(std::unique_ptr<node>(r)) {}

var::var(std::string l)
	: node(op_var),
	  l(std::unique_ptr<std::string>(new std::string(l))) {}

setvar::setvar(std::string l, node *r)
	: node(op_setvar),
	  l(std::unique_ptr<std::string>(new std::string(l))),
	  r(std::unique_ptr<node>(r)) {}

reg::reg(op opcode, size_t l)
	: node(opcode), l(l) {}

ssareg::ssareg(size_t l)
	: reg(op_ssareg, l) {}

phyreg::phyreg(size_t l)
	: reg(op_phyreg, l) {}

setreg::setreg(reg *l, node *r)
	: node(op_setreg),
	  l(std::unique_ptr<reg>(l)),
	  r(std::unique_ptr<node>(r)) {}

imm::imm(int r)
	: node(op_imm), r(r) {}


/*
 * evalulate expressions recursively
 */

Nat unaryop::eval(nat_compiler *d)
{
	Nat v = 0;
	switch(opcode) {
		case op_li:   v =  l->eval(d); break;
		case op_not:  v = ~l->eval(d); break;
		case op_neg:  v = -l->eval(d); break;
		default: break;
	}
	return v;
}

Nat binaryop::eval(nat_compiler *d)
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
		case op_srli: v = l->eval(d) >> r->eval(d).limb_at(0); break;
		case op_sll:  v = l->eval(d) << r->eval(d).limb_at(0); break;
		case op_slli: v = l->eval(d) << r->eval(d).limb_at(0); break;
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

Nat const_int::eval(nat_compiler *d)
{
	return *r;
}

Nat var::eval(nat_compiler *d)
{
	auto vi = d->variables.find(*l);
	return vi != d->variables.end() ? vi->second->eval(d) : Nat(0);
}

Nat setvar::eval(nat_compiler *d)
{
	d->variables[*l] = r.get();
	return r->eval(d);
}

Nat ssareg::eval(nat_compiler *d)
{
	return Nat(0);
}

Nat phyreg::eval(nat_compiler *d)
{
	auto ri = d->reg_values.find(l);
	return ri != d->reg_values.end() ? ri->second : Nat(0);
}

Nat setreg::eval(nat_compiler *d)
{
	Nat val = r->eval(d);
	d->reg_values[l->l] = val;
	return val;
}

Nat imm::eval(nat_compiler *d)
{
	return Nat(r);
}


/*
 * lower tree into single static assignment tuples
 */

node_list node::lower(nat_compiler *)
{
	return node_list();
}

node_list unaryop::lower(nat_compiler *d)
{
	/* get register of operand */
	node_list ll = l->lower(d);
	ssareg *lreg = new ssareg(d->lower_reg(ll));

	/* create new unary op using registers */
	unaryop *op = new unaryop(opcode, lreg);
	setreg *sr = new setreg(new ssareg(d->ssaregcount++), op);

	/* construct node list */
	node_list nodes;
	nodes.insert(nodes.end(), ll.begin(), ll.end());
	nodes.insert(nodes.end(), sr);

	return nodes;
}

node_list binaryop::lower(nat_compiler *d)
{
	/* lower const_int operand as immediate where possible */
	switch (opcode) {
		case op_srl:
		case op_sll:
			if (r->opcode == op_const_int) {
				/* get immediate opcode */
				op imm_opcode;
				switch(opcode) {
					case op_srl: imm_opcode = op_srli; break;
					case op_sll: imm_opcode = op_slli; break;
					default: imm_opcode = op_none; break;
				}

				/* get registers of operand */
				node_list ll = l->lower(d);
				ssareg *lreg = new ssareg(d->lower_reg(ll));
				imm *rimm = new imm(static_cast<const_int*>(r.get())->r->limb_at(0));

				/* create binary op using registers */
				binaryop *op = new binaryop(imm_opcode, lreg, rimm);
				setreg *sr = new setreg(new ssareg(d->ssaregcount++), op);

				/* construct node list */
				node_list nodes;
				nodes.insert(nodes.end(), ll.begin(), ll.end());
				nodes.insert(nodes.end(), sr);

				return nodes;
			}
		default:
			break;
	}

	/* get registers of operands */
	node_list ll = l->lower(d);
	node_list rl = r->lower(d);
	ssareg *lreg = new ssareg(d->lower_reg(ll));
	ssareg *rreg = new ssareg(d->lower_reg(rl));

	/* create binary op using registers */
	binaryop *op = new binaryop(opcode, lreg, rreg);
	setreg *sr = new setreg(new ssareg(d->ssaregcount++), op);

	/* construct node list */
	node_list nodes;
	nodes.insert(nodes.end(), ll.begin(), ll.end());
	nodes.insert(nodes.end(), rl.begin(), rl.end());
	nodes.insert(nodes.end(), sr);

	return nodes;
}

node_list const_int::lower(nat_compiler *d)
{
	/* move the number into a register */
	imm *op_imm = new imm(r->limb_at(0));
	unaryop *op = new unaryop(op_li, op_imm);
	setreg *sr = new setreg(new ssareg(d->ssaregcount++), op);

	return node_list{sr};
}

node_list var::lower(nat_compiler *d)
{
	/* return most recent register for this variable */
	size_t ssaregnum = d->varssa[*l];
	return node_list{new ssareg(ssaregnum)};
}

node_list setvar::lower(nat_compiler *d)
{
	/* store most recent register for this variable */
	node_list rl = r->lower(d);
	auto sr = static_cast<setreg*>(rl.back());
	sr->v = std::unique_ptr<var>(new var(*l));
	size_t ssaregnum = sr->l->l;
	d->varssa[*l] = ssaregnum;
	return rl;
}


/*
 * convert nodes to string
 */

std::string unaryop::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " " + l->to_string(d) + ")";
}

std::string binaryop::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " " + l->to_string(d) + ", " + r->to_string(d) + ")";
}

std::string const_int::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " " + r->to_string(16) + ")";
}

std::string var::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " '" + *l + "')";
}

std::string setvar::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " '" + *l + "', " + r->to_string(d) + ")";
}

std::string ssareg::to_string(nat_compiler *d)
{
	return std::string("_") + std::to_string(reg::l);
}

std::string phyreg::to_string(nat_compiler *d)
{
	return std::string("x") + std::to_string(reg::l);
}

std::string setreg::to_string(nat_compiler *d)
{
	return std::string("(") + op_name[opcode] + " " + l->to_string(d) + ", " + r->to_string(d) + ")";
}

std::string imm::to_string(nat_compiler *d)
{
	return Nat(r).to_string(16);
}


/*
 * driver methods called by parser
 */

nat_compiler::nat_compiler() : ssaregcount(0), phyregcount(31) {}

node* nat_compiler::new_unary(op opcode, node *l)
{
	return new unaryop(opcode, l);
}

node* nat_compiler::new_binary(op opcode, node *l, node *r)
{
	return new binaryop(opcode, l, r);
}

node* nat_compiler::new_const_int(std::string num)
{
	return new const_int(num);
}

node* nat_compiler::set_variable(std::string name, node *r)
{
	setvar *a = new setvar(name, r);
	variables[name] = a;
	return a;
}

node* nat_compiler::get_variable(std::string name)
{
	auto vi = variables.find(name);
	if (vi == variables.end()) error("unknown variable: " + name);
	var *a = new var(name);
	return a;
}

void nat_compiler::add_toplevel(node *n)
{
	nodes.push_back(n);
}

void nat_compiler::error(const yy::location& l, const std::string& m)
{
	std::cerr << l << ": " << m << std::endl;
	exit(1);
}

void nat_compiler::error(const std::string& m)
{
	std::cerr << m << std::endl;
	exit(1);
}


/*
 * compiler interface
 */

int nat_compiler::parse(std::istream &in)
{
	nat_scanner scanner;
	scanner.yyrestart(&in);
	yy::nat_parser parser(scanner, *this);
	return parser.parse();
}

void nat_compiler::use_ssa_scan(std::unique_ptr<node> &nr,
	size_t i, size_t j, size_t defreg)
{
	node *l = nr.get();
	if (typeid(*l) != typeid(ssareg)) return;
	size_t usereg = static_cast<ssareg*>(l)->l;
	if (usereg != defreg) return;
	def_use_ssa[j * ssaregcount + defreg] = '+';
	for (size_t k = j - 1; k != i; k--) {
		if (def_use_ssa[k * ssaregcount + defreg] == ' ') {
			def_use_ssa[k * ssaregcount + defreg] = '|';
		} else {
			break;
		}
	}
}

void nat_compiler::def_use_ssa_analysis()
{
	size_t def_use_ssa_size = nodes.size() * ssaregcount;
	def_use_ssa = std::unique_ptr<char[]>(new char[def_use_ssa_size]());
	memset(def_use_ssa.get(), ' ', def_use_ssa_size);
	for (size_t i = 0; i < nodes.size(); i++) {
		node *ndef = nodes[i];
		if (ndef->opcode != op_setreg) continue;
		setreg *srdef = static_cast<setreg*>(ndef);
		size_t defreg = srdef->l->l;
		def_use_ssa[i * ssaregcount + defreg] = 'v';
		for (size_t j = i + 1; j < nodes.size(); j++) {
			if (nodes[j]->opcode != op_setreg) continue;
			setreg *sruse = static_cast<setreg*>(nodes[j]);
			node *sruse_op = sruse->r.get();
			if (typeid(*sruse_op) == typeid(unaryop)) {
				unaryop *use_op = static_cast<unaryop*>(sruse_op);
				use_ssa_scan(use_op->l, i, j, defreg);
			} else if (typeid(*sruse_op) == typeid(binaryop)) {
				binaryop *use_op = static_cast<binaryop*>(sruse_op);
				use_ssa_scan(use_op->l, i, j, defreg);
				use_ssa_scan(use_op->r, i, j, defreg);
			}
		}
	}
}

void nat_compiler::allocate_registers()
{
	/* create physical registers */
	size_t def_use_phy_size = nodes.size() * phyregcount;
	def_use_phy = std::unique_ptr<char[]>(new char[def_use_phy_size]());
	for (size_t i = phyregcount; i > 0; i--) {
		reg_values[i] = 0;
		reg_free.push_back(i);
	}

	/* loop through nodes to assign physical registers */
	for (size_t i = 0; i < nodes.size(); i++) {
		node *ndef = nodes[i];
		if (ndef->opcode != op_setreg) continue;
		setreg *srdef = static_cast<setreg*>(ndef);

		/* free unused physical registers */
		auto rfi = reg_free.end();
		for (auto ri = reg_used.begin(); ri != reg_used.end();) {
			size_t ssaregnum = ri->first, phyregnum = ri->second;
			def_use_phy[i * phyregcount + phyregnum] =
				def_use_ssa[i * ssaregcount + ssaregnum];
			if (def_use_ssa[i * ssaregcount + ssaregnum] == ' ') {
				ri = reg_used.erase(ri);
				reg_free.insert(rfi, phyregnum);
			} else {
				ri++;
			}
		}

		/* replace ssa registers with physical registers */
		node *n = srdef->r.get();
		if (typeid(*n) == typeid(unaryop)) {
			unaryop *op = static_cast<unaryop*>(n);
			node *l = op->l.get();
			if (typeid(*l) == typeid(ssareg)) {
				size_t ssaregnum = static_cast<ssareg*>(l)->l;
				op->l = std::unique_ptr<phyreg>(new phyreg(reg_used[ssaregnum]));
			}
		} else if (typeid(*n) == typeid(binaryop)) {
			binaryop *op = static_cast<binaryop*>(n);
			node *l = op->l.get(), *r = op->r.get();
			if (typeid(*l) == typeid(ssareg)) {
				size_t ssaregnum = static_cast<ssareg*>(l)->l;
				op->l = std::unique_ptr<phyreg>(new phyreg(reg_used[ssaregnum]));
			}
			if (typeid(*r) == typeid(ssareg)) {
				size_t ssaregnum = static_cast<ssareg*>(r)->l;
				op->r = std::unique_ptr<phyreg>(new phyreg(reg_used[ssaregnum]));
			}
		}

		/* assign new physical register */
		if (reg_free.size() == 0) {
			error("register spilling not implemented");
		}
		size_t ssaregnum = srdef->l->l;
		size_t phyregnum = reg_free.back();
		def_use_phy[i * phyregcount + phyregnum] =
			def_use_ssa[i * ssaregcount + ssaregnum];
		reg_free.pop_back();
		reg_used[ssaregnum] = phyregnum;
		srdef->l = std::unique_ptr<phyreg>(new phyreg(phyregnum));
	}
}

void nat_compiler::lower(bool regalloc)
{
	/* recursively lower setvar expressions to setreg tuples in SSA form */
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

	/* perform def use analysis */
	def_use_ssa_analysis();

	/* allocate physical registers */
	if (regalloc) {
		allocate_registers();
	}
}

size_t nat_compiler::lower_reg(node_list &l)
{
	size_t ssaregnum;
	node *n = static_cast<node*>(l.back());
	switch(n->opcode) {
		case op_ssareg:
			ssaregnum = static_cast<ssareg*>(n)->l;
			l.pop_back();
			break;
		case op_setreg:
			ssaregnum = static_cast<setreg*>(n)->l->l;
			break;
		default:
			ssaregnum = 0;
			error("expected reg or setreg node");
			break;
	}
	return ssaregnum;
}

void nat_compiler::run(op opcode)
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
				std::cout << " "
					<< static_cast<setreg*>(n)->l->to_string(this)
					<< " = " << num.to_string(10)
					<< " (" << num.to_string(16) << ")" << std::endl;
				break;
			default:
				break;
		}
	}
}

void nat_compiler::dump_ast(op opcode, bool regalloc)
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
				if (regalloc) {
					for (size_t j = 0; j < phyregcount; j++) {
						std::cout << def_use_phy[i * phyregcount + j];
					}
				} else {
					for (size_t j = 0; j < ssaregcount; j++) {
						std::cout << def_use_ssa[i * ssaregcount + j];
					}
				}
				std::cout << std::endl;
				break;
			}
			default:
				break;
		}
	}
}

void nat_compiler::emit_asm()
{
	for (size_t i = 0; i < nodes.size(); i++) {
		node *n = nodes[i];
		if (n->opcode != op_setreg) continue;
		setreg *sr = static_cast<setreg*>(n);
		node *sr_op = sr->r.get();
		std::cout
			<< "\t"
			<< op_name[sr_op->opcode]
			<< "\t"
			<< sr->l->to_string(this)
			<< ", ";
		if (typeid(*sr_op) == typeid(unaryop)) {
			std::cout
				<< static_cast<unaryop*>(sr_op)->l->to_string(this);
		} else if (typeid(*sr_op) == typeid(binaryop)) {
			std::cout
				<< static_cast<binaryop*>(sr_op)->l->to_string(this)
				<< ", "
				<< static_cast<binaryop*>(sr_op)->r->to_string(this);
		}
		std::cout << std::endl;
	}
}
