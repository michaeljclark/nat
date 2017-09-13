#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <iomanip>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-compiler.h"
#include "FlexLexer.h"
#include "nat-scanner.h"
#include "nat-target.h"
#include "nat-target-riscv.h"


using namespace nat;
using namespace nat::target;

using nat::target::reg_class::cr;
using nat::target::reg_class::ce;
using nat::target::reg_class::rs;

const reg_class riscv::target::reg_class[] = {
	cr, cr, cr, cr, cr, cr, cr, cr,
	cr, cr, cr, cr, cr, cr, cr, ce,
	ce, ce, ce, ce, ce, ce, ce, ce,
	ce, ce, ce, rs, rs, rs, rs, rs
};

const size_t riscv::target::reg_order[] = {
	10, 11, 12, 13, 14, 15, 16, 17,
	5,  6,  7,  28, 29, 30, 31, 8,
	9,  18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 1,  2,  3,  4,  0
};

const char* riscv::target::reg_name[] = {
	"zero",
	"ra",
	"sp",
	"gp",
	"tp",
	"t0",
	"t1",
	"t2",
	"s0",
	"s1",
	"a0",
	"a1",
	"a2",
	"a3",
	"a4",
	"a5",
	"a6",
	"a7",
	"s2",
	"s3",
	"s4",
	"s5",
	"s6",
	"s7",
	"s8",
	"s9",
	"s10",
	"s11",
	"t3",
	"t4",
	"t5",
	"t6"
};

const char* riscv::target::inst_name[] = {
	"illegal",
	"li",
	"and",
	"andi",
	"or",
	"ori",
	"xor",
	"xori",
	"slt",
	"slti",
	"sltu",
	"sltiu",
	"srl",
	"srli",
	"sll",
	"slli",
	"add",
	"addi",
	"sub",
	"mul",
	"div",
	"rem"
};

void riscv::machineinst::execute(nat::target::backend *target)
{
	riscv::target *t = static_cast<riscv::target*>(target);
	switch (instcode) {
		case inst_li:    t->reg[rd] = simm;                              break;
		case inst_and:   t->reg[rd] = t->reg[rs1] & t->reg[rs2];         break;
		case inst_andi:  t->reg[rd] = t->reg[rs1] & simm;                break;
		case inst_or:    t->reg[rd] = t->reg[rs1] | t->reg[rs2];         break;
		case inst_ori:   t->reg[rd] = t->reg[rs1] | simm;                break;
		case inst_xor:   t->reg[rd] = t->reg[rs1] ^ t->reg[rs2];         break;
		case inst_xori:  t->reg[rd] = t->reg[rs1] ^ simm;                break;
		case inst_slt:   t->reg[rd] = t->reg[rs1] < t->reg[rs2];         break;
		case inst_slti:  t->reg[rd] = t->reg[rs1] < simm;                break;
		case inst_sltu:  t->reg[rd] = ux(t->reg[rs1]) < ux(t->reg[rs2]); break;
		case inst_sltiu: t->reg[rd] = ux(t->reg[rs1]) < ux(simm);        break;
		case inst_srl:   t->reg[rd] = t->reg[rs1] >> t->reg[rs2];        break;
		case inst_srli:  t->reg[rd] = t->reg[rs1] >> simm;               break;
		case inst_sll:   t->reg[rd] = t->reg[rs1] << t->reg[rs2];        break;
		case inst_slli:  t->reg[rd] = t->reg[rs1] << simm;               break;
		case inst_add:   t->reg[rd] = t->reg[rs1] + t->reg[rs2];         break;
		case inst_addi:  t->reg[rd] = t->reg[rs1] + simm;                break;
		case inst_sub:   t->reg[rd] = t->reg[rs1] - t->reg[rs2];         break;
		case inst_mul:   t->reg[rd] = t->reg[rs1] * t->reg[rs2];         break;
		case inst_div:   t->reg[rd] = t->reg[rs1] / t->reg[rs2];         break;
		case inst_rem:   t->reg[rd] = t->reg[rs1] % t->reg[rs2];         break;
		default: break;
	}
}

std::string riscv::machineinst::to_string(nat::compiler *driver)
{
	std::string s = riscv::target::inst_name[instcode];
	s += "\t";
	switch (typecode) {
		case type_none:
			break;
		case type_r:
			s += riscv::target::reg_name[rd];
			s += ", ";
			s += riscv::target::reg_name[rs1];
			s += ", ";
			s += riscv::target::reg_name[rs2];
			break;
		case type_i:
			s += riscv::target::reg_name[rd];
			s += ", ";
			s += riscv::target::reg_name[rs1];
			s += ", ";
			s += Nat(simm).to_string(16);
			break;
		case type_u:
			s += riscv::target::reg_name[rd];
			s += ", ";
			s += Nat(simm).to_string(16);
			break;
	}
	return s;
}

machineinst* riscv::machineinst::make_r(inst instcode, reg_s rd, reg_s rs1, reg_s rs2)
{
	machineinst *insn = new machineinst();
	insn->instcode = instcode;
	insn->typecode = type_r;
	insn->rd = rd;
	insn->rs1 = rs1;
	insn->rs2 = rs2;
	return insn;
}

machineinst* riscv::machineinst::make_i(inst instcode, reg_s rd, reg_s rs1, int simm)
{
	machineinst *insn = new machineinst();
	insn->instcode = instcode;
	insn->typecode = type_i;
	insn->rd = rd;
	insn->rs1 = rs1;
	insn->simm = simm;
	return insn;
}

machineinst* riscv::machineinst::make_u(inst instcode, reg_s rd, int simm)
{
	machineinst *insn = new machineinst();
	insn->instcode = instcode;
	insn->typecode = type_u;
	insn->rd = rd;
	insn->simm = simm;
	return insn;
}

size_t riscv::target::get_reg_count() { return reg_count; }

const char** riscv::target::get_reg_name() { return reg_name; }

const reg_class* riscv::target::get_reg_class() { return reg_class; }

const size_t* riscv::target::get_reg_order() { return reg_order; }

nat::node_list riscv::target::emit(nat::compiler *driver, nat::node *n)
{
	nat::node_list l;

	if (n->opcode != op_setreg) return l;
	setreg *sr = static_cast<setreg*>(n);
	node *sr_op = sr->r.get();
	op opcode = sr_op->opcode;

	switch (opcode)
	{
		case op_li: {
			auto op = static_cast<unaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_imm = static_cast<imm*>(op->l.get());
			l.push_back(riscv::machineinst::make_u(inst_li, op_rd->l, op_imm->r));
			break;
		}
		case op_not: {
			auto op = static_cast<unaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_rs1 = static_cast<phyreg*>(op->l.get());
			l.push_back(riscv::machineinst::make_i(inst_xori, op_rd->l, op_rs1->l, -1));
			break;
		}
		case op_neg: {
			auto op = static_cast<unaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_rs1 = static_cast<phyreg*>(op->l.get());
			l.push_back(riscv::machineinst::make_r(inst_sub, op_rd->l, 0, op_rs1->l));
			break;
		}
		case op_and:
		case op_or:
		case op_xor:
		case op_slt:
		case op_srl:
		case op_sll:
		case op_add:
		case op_sub:
		case op_mul:
		case op_div:
		case op_rem: {
			auto op = static_cast<binaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_rs1 = static_cast<phyreg*>(op->l.get());
			auto op_rs2 = static_cast<phyreg*>(op->r.get());
			inst instcode;
			switch (opcode) {
				case op_and: instcode = inst_and; break;
				case op_or:  instcode = inst_or;  break;
				case op_xor: instcode = inst_xor; break;
				case op_slt: instcode = inst_slt; break;
				case op_srl: instcode = inst_srl; break;
				case op_sll: instcode = inst_sll; break;
				case op_add: instcode = inst_add; break;
				case op_sub: instcode = inst_sub; break;
				case op_mul: instcode = inst_mul; break;
				case op_div: instcode = inst_div; break;
				case op_rem: instcode = inst_rem; break;
				default: instcode = inst_illegal; break;
			}
			l.push_back(riscv::machineinst::make_r(instcode, op_rd->l, op_rs1->l, op_rs2->l));
			break;
		}
		case op_srli:
		case op_slli:
		case op_addi: {
			auto op = static_cast<binaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_rs1 = static_cast<phyreg*>(op->l.get());
			auto op_imm = static_cast<imm*>(op->r.get());
			inst instcode;
			switch (opcode) {
				case op_srli: instcode = inst_srli; break;
				case op_slli: instcode = inst_slli; break;
				case op_addi: instcode = inst_addi; break;
				default: instcode = inst_illegal; break;
			}
			l.push_back(riscv::machineinst::make_i(instcode, op_rd->l, op_rs1->l, op_imm->r));
			break;
		}
		case op_seq:
		case op_sne:
		case op_slte:
		case op_sgt:
		case op_sgte:
		{
			auto op = static_cast<binaryop*>(sr_op);
			auto op_rd = static_cast<phyreg*>(sr->l.get());
			auto op_rs1 = static_cast<phyreg*>(op->l.get());
			auto op_rs2 = static_cast<phyreg*>(op->r.get());
			switch (opcode) {
				case op_seq:
					l.push_back(riscv::machineinst::make_r(inst_sub, op_rd->l, op_rs1->l, op_rs2->l));
					l.push_back(riscv::machineinst::make_i(inst_sltiu, op_rd->l, op_rd->l, 1));
					break;
				case op_sne:
					l.push_back(riscv::machineinst::make_r(inst_sub, op_rd->l, op_rs1->l, op_rs2->l));
					l.push_back(riscv::machineinst::make_r(inst_sltu, op_rd->l, 0, op_rd->l));
					break;
				case op_slte:
					l.push_back(riscv::machineinst::make_r(inst_slt, op_rd->l, op_rs2->l, op_rs1->l));
					l.push_back(riscv::machineinst::make_i(inst_xori, op_rd->l, op_rd->l, 1));
					break;
				case op_sgt:
					l.push_back(riscv::machineinst::make_r(inst_slt, op_rd->l, op_rs2->l, op_rs1->l));
					break;
				case op_sgte:
					l.push_back(riscv::machineinst::make_r(inst_slt, op_rd->l, op_rs1->l, op_rs2->l));
					l.push_back(riscv::machineinst::make_i(inst_xori, op_rd->l, op_rd->l, 1));
					break;
				default:
					break;
			}
			break;
		}
		default: {
			std::string msg = "unimplemented op: ";
			msg += op_name[sr_op->opcode];
			driver->error(msg);
		}
	}

	return l;
}
