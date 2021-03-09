#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <vector>
#include <iostream>
#include <fstream>
#include "Line.h"
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
	vector<RelBlock> flink;

};
struct SectionMaterial {
	string sec_name;
	vector<char> code;
	vector<RelBlock> rel;
	vector<int> instructions;
};
class Compiler{
private:
  vector<pair<int,vector<string>>> lines;
  vector<Line> true_lines;
  vector<Entry> symbol_table;
  vector<SectionMaterial> end_code;
  vector<pair<int,Line>> equ_table;
  vector<pair<string, int>> equ_rel_table;
  void AnalyseTheFile();
  void RunThroughTheFile();
  void EquTable();
  void SolveRelBlocks();
  void WriteFile();
  Line process_instr(vector<string>, int num);
  Line process_directive(vector<string>, int num);
  vector<string> split(const string &s, char delim);
  ifstream f1;
  ofstream f2;
  bool end = false;
public:
	void Transcribe(string first_file, string second_file);
};

#endif
