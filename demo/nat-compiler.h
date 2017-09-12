#pragma once

/*
 * forward declarations
 */

#undef YY_DECL
#define YY_DECL yy::nat_parser::symbol_type nat_scanner::yylex(nat_compiler &driver)

struct node;
typedef std::vector<node*> node_list;
typedef std::map<std::string,node*> node_map;
typedef std::map<std::string,size_t> var_map;
typedef std::deque<size_t> reg_free_list;
typedef std::map<size_t,size_t> reg_alloc_map;
typedef std::map<size_t,Nat> reg_value_map;
struct nat_compiler;


/*
 * tree node typecodes and opcodes
 */

enum type : char
{
	type_none,
	type_const_int,
	type_unaryop,
	type_binaryop,
	type_var,
	type_setvar,
	type_ssareg,
	type_phyreg,
	type_setreg,
	type_imm,
};

enum op : char
{
	op_none,
	op_const_int,
	op_var,
	op_setvar,
	op_ssareg,
	op_phyreg,
	op_setreg,
	op_imm,
	op_li,
	op_and,
	op_or,
	op_xor,
	op_seq,
	op_sne,
	op_slt,
	op_slte,
	op_sgt,
	op_sgte,
	op_srl,
	op_srli,
	op_sll,
	op_slli,
	op_add,
	op_addi,
	op_sub,
	op_mul,
	op_div,
	op_rem,
	op_not,
	op_neg,
};


/*
 * tree node types
 */

struct node
{
	type typecode;
	op opcode;

	node(type typecode, op opcode);

	virtual ~node() {}
	virtual Nat eval(nat_compiler *) = 0;
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *) = 0;
};

struct const_int : node
{
	std::unique_ptr<Nat> r;

	const_int(std::string r);
	const_int(Nat &n);

	virtual Nat eval(nat_compiler *);
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct unaryop : node
{
	std::unique_ptr<node> l;

	unaryop(op opcode, node *l);

	virtual Nat eval(nat_compiler *);
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct binaryop : node
{
	std::unique_ptr<node> l, r;

	binaryop(op opcode, node *l, node *r);

	virtual Nat eval(nat_compiler *);
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct var : node
{
	std::unique_ptr<std::string> l;

	var(std::string l);

	virtual Nat eval(nat_compiler *);
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct setvar : node
{
	std::unique_ptr<std::string> l;
	std::unique_ptr<node> r;

	setvar(std::string l, node *r);

	virtual Nat eval(nat_compiler *);
	virtual node_list lower(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct reg : node
{
	size_t l;

	reg(type typecode, op opcode, size_t l);
};

struct ssareg : reg
{
	ssareg(size_t l);

	virtual Nat eval(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct phyreg : reg
{
	phyreg(size_t l);

	virtual Nat eval(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct setreg : node
{
	std::unique_ptr<reg> l;
	std::unique_ptr<node> r;
	std::unique_ptr<var> v;

	setreg(reg *l, node *r);

	virtual Nat eval(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};

struct imm : node
{
	int r;

	imm(int r);

	virtual Nat eval(nat_compiler *);
	virtual std::string to_string(nat_compiler *);
};


/*
 * compiler
 */

struct nat_compiler
{
	node_list nodes;
	node_map variables;
	var_map varssa;
	reg_free_list reg_free;
	reg_alloc_map reg_used;
	reg_value_map reg_values;
	size_t ssaregcount;
	size_t phyregcount;
	std::unique_ptr<char[]> def_use_ssa;
	std::unique_ptr<char[]> def_use_phy;

	nat_compiler();

	node* new_unary(op opcode, node *l);
	node* new_binary(op opcode, node *l, node *r);
	node* new_const_int(std::string num);
	node* set_variable(std::string name, node *r);
	node* get_variable(std::string name);
	void add_toplevel(node *n);
	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);

	int parse(std::istream &in);

	void use_ssa_scan(std::unique_ptr<node> &nr,
		size_t i, size_t j, size_t defreg);
	void def_use_ssa_analysis();
	void allocate_registers();
	void lower(bool regalloc);

	size_t lower_reg(node_list &l);
	void run(op opcode);
	void dump_ast(op opcode, bool regalloc = false);
	void emit_asm();
};
