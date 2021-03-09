#ifndef __LINE_H__
#define __LINE_H__

#include <string.h>
#include <iostream>
#include <vector>
#include "Enums.h"
using namespace std;


struct Operand {
	Tokens token;
	int reg_number = -1;
	string symbol="";
	int literal=-1;
	int byte_size = 0;
	int high_low = -1;
};

struct Section {
	string name="";
};

struct ListMember {
	string symbol="";
	short literal = -1;
	string op = "";
};
struct Line {
	string label="";
	Tokens type;
	Opcodes opcode;
	string spec_bw = "";
	string directive="";
	string accessor = "";
	string equ_symbol="";
	Section section;
	Operand op1;
	Operand op2;
	vector<ListMember> directive_members;
};

#endif
