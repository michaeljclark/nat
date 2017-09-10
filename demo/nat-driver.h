#pragma once

#undef YY_DECL
#define YY_DECL yy::nat_parser::symbol_type nat_scanner::yylex(nat_driver &driver)

struct node;
struct reg;
typedef std::vector<node*> node_list;
typedef std::map<std::string,node*> node_map;
typedef std::map<std::string,size_t> ssa_map;
typedef std::map<size_t,Nat> reg_map;
struct nat_driver;

enum op
{
	op_none,
	op_const_int,
	op_var,
	op_setvar,
	op_reg,
	op_imm,
	op_setreg,
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
	op_pow,
};

struct node
{
	op opcode;

	node(op opcode);
	virtual ~node() {}
	virtual Nat eval(nat_driver *) = 0;
	virtual node_list lower(nat_driver *) = 0;
	virtual std::string to_string(nat_driver *) = 0;
};

struct const_int : node
{
	std::unique_ptr<Nat> r;

	const_int(std::string r);
	const_int(Nat &n);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct unaryop : node
{
	std::unique_ptr<node> l;

	unaryop(op opcode, node *l);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct binaryop : node
{
	std::unique_ptr<node> l, r;

	binaryop(op opcode, node *l, node *r);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct var : node
{
	std::unique_ptr<std::string> l;

	var(std::string l);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct setvar : node
{
	std::unique_ptr<std::string> l;
	std::unique_ptr<node> r;

	setvar(std::string l, node *r);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct reg : node
{
	size_t l;

	reg(size_t l);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct imm : node
{
	int r;

	imm(int r);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct setreg : node
{
	size_t l;
	std::unique_ptr<node> r;
	std::unique_ptr<var> v;

	setreg(size_t l, node *r);
	virtual Nat eval(nat_driver *);
	virtual node_list lower(nat_driver *);
	virtual std::string to_string(nat_driver *);
};

struct nat_driver
{
	node_list nodes;
	node_map variables;
	ssa_map varssa;
	reg_map registers;
	size_t regnum;
	std::unique_ptr<char[]> def_use;

	nat_driver();

	node* new_unary(op opcode, node *l);
	node* new_binary(op opcode, node *l, node *r);
	node* new_const_int(std::string num);
	node* set_variable(std::string name, node *r);
	node* get_variable(std::string name);
	void add_toplevel(node *n);
	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);

	int parse(std::istream &in);
	void use_scan(std::unique_ptr<node> &nr, size_t i, size_t j, size_t defreg);
	void usedef_analysis();
	void lower();
	size_t lower_reg(node_list &l);
	void run(op opcode);
	void dump(op opcode);
};
