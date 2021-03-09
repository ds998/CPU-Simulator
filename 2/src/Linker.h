#ifndef __LINKER_H__
#define __LINKER_H__

#include <vector>
#include <string.h>
#include <iostream>
using namespace std;

struct RelBlock {
	int address;
	int type;
	int table_num;
	int byte_size;
};

struct Entry {
	string name = "";
	string section = "";
	int val = -1;
	int glob_loc = -1;
	int size = 0;

};

struct SectionMaterial {
	string sec_name;
	vector<char> code;
	vector<RelBlock> rel;
};
struct Equ_Member{
	string symbol;
	string dependent_symbol;
	string plus_minus;

};
struct File{
    vector<Equ_Member> equ_table;
    vector<Entry> symbol_table;
    vector<SectionMaterial> sections;
};

struct End_Instruction{
    int address;
    string section;
    vector<char> instructions;
};

struct Program{
    vector<Entry> exit_symbol_table;
    vector<RelBlock> exit_rel_blocks;
    vector<End_Instruction> end_instructions;
    vector<File> files;
};

class Linker{
private:
vector<pair<string,int>> sec_placement;
vector<string> filenames;
Program program;

vector<string> split(const string &s,char delim);
void read_input(vector<string> args);
void read_files();
void resize_sections();
void redefine_symbols();
void do_rel_blocks();
void combine_code();
public:
Program link(vector<string> args);



};
#endif