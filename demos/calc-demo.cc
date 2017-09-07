#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <map>

#include <histedit.h>

#include "nat.h"
#include "calc-parser.hh"
#include "calc-driver.h"


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
		if (line.find("\n") == 0) continue;
		size_t contp = line.find(";\n");
		continuation = contp != std::string::npos;
		if (continuation) {
			in << std::string(li->buffer, contp) << std::endl;
			continue;
		}
		in << li->buffer;
		calc_driver driver;
		driver.parse(in);
		history(hist, &ev, H_ENTER, in.str().c_str());
		in.str(std::string());
	}
	el_end(el);
	history_end(hist);
}

void interp(int argc, char **argv)
{
	std::ifstream in(argv[1]);
	calc_driver driver;
	driver.parse(in);
}

int main(int argc, char **argv)
{
	if ((argc == 1 && isatty(fileno(stdin))) ||
		(argc == 2 && strcmp(argv[1], "-") == 0))
	{
		repl(argc, argv);
	} else if (argc == 2) {
		interp(argc, argv);
	} else {
		fprintf(stderr, "usage: %s [<filename>|-]\n", argv[0]);
		exit(1);
	}
}
