#include <stdio.h>
#include <histedit.h>
#include <sstream>
#include <map>

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

int main(int argc, char **argv)
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

	std::stringstream ss;
	while ((buf = el_gets(el, &num)) != NULL && num != 0)
	{
		const LineInfo *li = el_line(el);
		std::string line = li->buffer;
		size_t contp = line.find("\\\n");
		continuation = contp != std::string::npos;
		if (continuation) {
			ss << std::string(li->buffer, contp) << std::endl;
			continue;
		}
		ss << li->buffer;
		calc_driver driver;
		driver.parse(ss);
		history(hist, &ev, H_ENTER, ss.str().c_str());
		ss.str(std::string());
	}
	el_end(el);
	history_end(hist);
}
