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


using namespace nat::target;

backend* backend::get_default()
{
	static riscv::target riscv_target;
	return &riscv_target;
}
