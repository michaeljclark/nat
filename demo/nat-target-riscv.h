#pragma once

namespace riscv {

	typedef signed long long   sx;
	typedef unsigned long long ux;
	typedef unsigned char      reg_s;

	enum inst : char
	{
		inst_illegal,
		inst_li,
		inst_and,
		inst_andi,
		inst_or,
		inst_ori,
		inst_xor,
		inst_xori,
		inst_slt,
		inst_slti,
		inst_sltu,
		inst_sltiu,
		inst_srl,
		inst_srli,
		inst_sll,
		inst_slli,
		inst_add,
		inst_addi,
		inst_sub,
		inst_mul,
		inst_div,
		inst_rem,
		inst_not
	};

	enum type : char
	{
		type_none,
		type_u,
		type_r,
		type_i,
	};

	struct machineinst : nat::target::machineinst
	{
		inst instcode;
		type typecode;
		reg_s rd;
		reg_s rs1;
		reg_s rs2;
		reg_s rs3;
		int simm;

		machineinst() {}

		virtual void execute(nat::target::backend *);
		virtual std::string to_string(nat::compiler *);

		static nat::target::machineinst* make_r(inst instcode, reg_s rd, reg_s rs1, reg_s rs2);
		static nat::target::machineinst* make_i(inst instcode, reg_s rd, reg_s rs1, int imm);
		static nat::target::machineinst* make_u(inst instcode, reg_s rd, int imm);
	};

	struct target : nat::target::backend
	{
		enum { reg_count = 32};

		sx reg[reg_count];

		static const nat::target::reg_class reg_class[];
		static const size_t reg_order[];
		static const char* reg_name[];
		static const char* inst_name[];

		size_t get_reg_count();
		const char** get_reg_name();
		const nat::target::reg_class* get_reg_class();
		const size_t* get_reg_order();
		nat::node_list emit(nat::compiler*, nat::node*);
	};
}
