#include <cstdio>
#include <unistd.h>

#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

#include <histedit.h>

#include "nat.h"
#include "nat-parser.hh"
#include "nat-compiler.h"


using namespace nat;

static bool continuation = false;

static char *nat_prompt(EditLine *el __attribute__((__unused__)))
{
	static char p[] = "Nat> ";
	static char c[] = "   | ";
	return continuation ? c : p;
}

void repl(int argc, char **argv)
{
	EditLine *el = NULL;
	int num;
	const char *buf;
	History *hist;
	HistEvent ev;

	hist = history_init();
	history(hist, &ev, H_SETSIZE, 100);
	el = el_init(*argv, stdin, stdout, stderr);
	el_set(el, EL_EDITOR, "emacs");
	el_set(el, EL_PROMPT_ESC, nat_prompt, '\1');
	el_set(el, EL_HIST, history, hist);

	std::stringstream in;
	while ((buf = el_gets(el, &num)) != NULL && num != 0)
	{
		const LineInfo *li = el_line(el);
		std::string line = li->buffer;
		bool empty = line.find("\n") == 0;
		size_t contp = line.find(";\n");
		if (empty) {
			if (continuation) continuation = false;
			else continue;
		}
		if ((continuation = (contp != std::string::npos))) {
			in << std::string(li->buffer, contp) << std::endl;
			continue;
		}
		if (!empty) {
			in << li->buffer;
		}
		compiler compiler;
		compiler.parse(in);
		compiler.run(op_setvar);
		history(hist, &ev, H_ENTER, in.str().c_str());
		in.str(std::string());
	}
	el_end(el);
	history_end(hist);
}

void print_usage(char **argv)
{
	fprintf(stderr, "usage: %s [(--interp|--tree|--ssa|--regalloc|--run) <filename>]\n", argv[0]);
}

void interp(int argc, char **argv)
{
	bool interp = strcmp(argv[1], "--interp") == 0;
	bool tree = strcmp(argv[1], "--tree") == 0;
	bool ssa = strcmp(argv[1], "--ssa") == 0;
	bool regalloc = strcmp(argv[1], "--regalloc") == 0;
	bool toyasm = strcmp(argv[1], "--asm") == 0;
	bool run = strcmp(argv[1], "--run") == 0;

	if (!(interp || tree || ssa || regalloc || toyasm || run)) {
		print_usage(argv);
		exit(1);
	}

	std::ifstream in(argv[2]);
	compiler compiler;
	compiler.parse(in);
	if (interp) {
		compiler.run(op_setvar);
	}
	else if (tree) {
		compiler.dump_ast(op_setvar);
	}
	else if (ssa || regalloc) {
		compiler.lower(regalloc);
		compiler.dump_ast(op_setreg, regalloc);
	}
	else if (toyasm) {
		compiler.lower(true);
		compiler.emit_asm();
	}
	else if (run) {
		compiler.lower(true);
		compiler.run(op_setreg);
	}
}

int main(int argc, char **argv)
{
	if ((argc == 1 && isatty(fileno(stdin))) ||
		(argc == 2 && strcmp(argv[1], "-") == 0))
	{
		repl(argc, argv);
	}
	else if (argc == 3) {
		interp(argc, argv);
	}
	else {
		print_usage(argv);
		exit(1);
	}
}
