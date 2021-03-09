#include "Lexers.h"
#include "Compiler.h"
#include <sstream>
#include <map>

vector<string> Compiler::split(const string &s, char delim) {
	vector<string> result;
	string new_s = "";
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == delim) {
			if (new_s != "")result.push_back(new_s);
			new_s = "";
		}
		else new_s += s[i];
	}
	if (new_s != "")result.push_back(new_s);
	return result;
}

void Compiler::AnalyseTheFile() {
	string name;
	int i = 0;
	while (getline(f1, name)) {
		char c = ' ';
		vector<string> result = split(name, c);
		if (result.size() == 0) continue;
		string first = result[0];
		vector<string> new_vec;
		new_vec.push_back(first);
		for (int j = 1; j < result.size(); j++) {
			vector<string> nr = split(result[j], ',');
			for (int k = 0; k < nr.size(); k++) {
				new_vec.push_back(nr[k]);
			}
		}
		pair<int, vector<string>> p;
		p.first = i;
		p.second = new_vec;
		lines.push_back(p);
		if (!regex_match(first, regex("^" + symbol + ":$")) || new_vec.size()>1) {
			i++;
		}


	}

	for (int i = 0; i < lines.size(); i++) {
		pair<int, vector<string>> p = lines[i];
		cout << p.first << " ";
		for (int j = 0; j < p.second.size(); j++) {
			cout << p.second[j] << " ";
		}
		cout << endl;
	}
	i = 0;
	string label = "";
	int previous_num = -1;
	while (i < lines.size()) {
		Line line;
		int num = lines[i].first;
		vector<string> vec = lines[i].second;
		string type = vec[0];
		if (regex_match(type, regex("^(\\.global|\\.extern)$"))) {
			if (vec.size() < 2) {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
			if (previous_num != -1 || label != "") {
				cout << "Greska na liniji:" << previous_num << endl;
				
				exit(-1);
			}
			for (int j = 1; j < vec.size(); j++) {
				string name = vec[j];
				ListMember lm;
				if (regex_match(name, regex("^" + symbol + "$"))) {
					lm.symbol = name;
					line.directive_members.push_back(lm);
				}
				else {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
			}
			line.type = Tokens::ACCESSOR;
			line.accessor = type.substr(1);
			true_lines.push_back(line);
			i++;
		}
		else if (regex_match(type, regex("^\\.section$"))) {
			if (vec.size() != 2) {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
			if (previous_num != -1 || label != "") {
				cout << "Greska na liniji:" << previous_num << endl;
				
				exit(-1);
			}
			string name = vec[1];
			if (regex_match(name, regex("^" + symbol + "$"))) {
				line.type = Tokens::SECTION;
				line.section.name = name;
				true_lines.push_back(line);
				i++;
			}
			else {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
		}
		else if (regex_match(type, regex("^" + symbol + ":$"))) {
			label = type.substr(0, type.length() - 1);
			if (vec.size() == 1){
				previous_num = num;
				i++;
            }
			else {
				string type2 = vec[1];
				if (regex_match(type2, regex("^\\.(word|byte|skip|equ)$"))) {
					vector<string> new_vec;
					for (int j = 1; j < vec.size(); j++) {
						new_vec.push_back(vec[j]);
					}
					line = process_directive(new_vec, num);
					if (end == true) {
						break;
					}
					line.label = label;
					if (type2 == ".equ") {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
					label = "";
					true_lines.push_back(line);
					i++;
				}
				else if (regex_match(type2, regex("^(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)?$"))) {
					vector<string> new_vec;
					for (int j = 1; j < vec.size(); j++) {
						new_vec.push_back(vec[j]);
					}
					line = process_instr(new_vec, num);
					line.label = label;
					label = "";
					true_lines.push_back(line);
					i++;

				}
				else {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
			}

		}
		else if (regex_match(type, regex("^\\.(word|byte|skip|equ|end)$"))) {
			line = process_directive(vec, num);
			if (end == true) {
				break;
			}
			if (label != "") {
				if (previous_num != -1 && previous_num == num) {
					line.label = label;
					label = "";
					previous_num = -1;
				}
				else {
					cout << "Greska na liniji:" << previous_num << endl;
					
					exit(-1);
				}
				
			}
			true_lines.push_back(line);
			i++;
		}
		else if (regex_match(type, regex("^(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)?$"))) {
			line = process_instr(vec, num);
			if (label != "") {
				if (previous_num != -1 && previous_num == num) {
					line.label = label;
					label = "";
					previous_num = -1;
				}
				else {
					cout << "Greska na liniji:" << previous_num << endl;
					
					exit(-1);
				}

			}
			true_lines.push_back(line);
			i++;
		}
		else {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}
	}

	if (end == false) {
		cout << "Nema kraja:" << endl;
		
		exit(-1);
	}

	/*
	cout << "=========================" << endl;

	for (i = 0; i < true_lines.size(); i++) {
		Line l = true_lines[i];
		switch (l.type) {
		case Tokens::ACCESSOR:
			cout << l.accessor << " ";
			for (int j = 0; j < l.directive_members.size(); j++) {
				cout << l.directive_members[j].symbol << " ";
			}
			cout << endl;
			break;
		case Tokens::SECTION:
			cout << "section " << l.section.name << endl;
			break;
		case Tokens::DIRECTIVE:
			if (l.label != "") cout << "label " << l.label << ":";
			cout << l.directive << " ";
			if (l.directive == "equ") cout << l.equ_symbol << ", ";
			for (int j = 0; j < l.directive_members.size(); j++) {
				if (l.directive_members[j].symbol == "") {
					cout << l.directive_members[j].literal;
				}
				else {
					cout << l.directive_members[j].symbol;
				}
				cout << l.directive_members[j].op << " ";
			}
			cout << endl;
			break;
		case Tokens::OPCODE:
			if (l.label != "") cout << "label " << l.label << ":";
			int num_of_ops = -1;
			switch (l.opcode) {
			case Opcodes::HALT:
				cout << "halt" << " ";
				num_of_ops = 0;
				break;
			case Opcodes::IRET:
				cout << "iret" << " ";
				num_of_ops = 0;
				break;
			case Opcodes::RET:
				cout << "ret" << " ";
				num_of_ops = 0;
				break;
			case Opcodes::INT:
				cout << "int" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::CALL:
				cout << "call" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::JMP:
				cout << "jmp" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::JEQ:
				cout << "jeq" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::JNE:
				cout << "jne" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::JGT:
				cout << "jgt" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::PUSH:
				cout << "push" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::POP:
				cout << "pop" << " ";
				num_of_ops = 1;
				break;
			case Opcodes::XCHG:
				cout << "xchg" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::MOV:
				cout << "mov" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::ADD:
				cout << "add" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::SUB:
				cout << "sub" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::MUL:
				cout << "mul" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::DIV:
				cout << "div" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::CMP:
				cout << "cmp" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::NOT:
				cout << "not" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::AND:
				cout << "and" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::OR:
				cout << "or" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::XOR:
				cout << "xor" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::TEST:
				cout << "test" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::SHL:
				cout << "shl" << " ";
				num_of_ops = 2;
				break;
			case Opcodes::SHR:
				cout << "shr" << " ";
				num_of_ops = 2;
				break;
			
			}
			cout << l.spec_bw<<" ";
			if (num_of_ops > 0) {
				switch (l.op1.token) {
				case Tokens::REG_IMM_DEC_OPERAND:
					cout << "REG_IMM_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_IMM_HEX_OPERAND:
					cout << "REG_IMM_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_SYM_OPERAND:
					cout << "REG_SYM_OPERAND" << " ";
					break;
				case Tokens::REG_REG_OPERAND:
					cout << "REG_REG_OPERAND" << " ";
					break;
				case Tokens::REG_REG_IND_OPERAND:
					cout << "REG_REG_IND_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDIMM_DEC_OPERAND:
					cout << "REG_REGINDIMM_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDIMM_HEX_OPERAND:
					cout << "REG_REGINDIMM_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDSYM_OPERAND:
					cout << "REG_REGINDSYM_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_DEC_OPERAND:
					cout << "REG_MEMDIR_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_HEX_OPERAND:
					cout << "REG_MEMDIR_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_SYM_OPERAND:
					cout << "REG_MEMDIR_SYM_OPERAND" << " ";
					break;
				case Tokens::JMP_IMM_DEC_OPERAND:
					cout << "JMP_IMM_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_IMM_HEX_OPERAND:
					cout << "JMP_IMM_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_SYM_OPERAND:
					cout << "JMP_SYM_OPERAND" << " ";
					break;
				case Tokens::JMP_REG_OPERAND:
					cout << "JMP_REG_OPERAND" << " ";
					break;
				case Tokens::JMP_REG_IND_OPERAND:
					cout << "JMP_REG_IND_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDIMM_DEC_OPERAND:
					cout << "JMP_REGINDIMM_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDIMM_HEX_OPERAND:
					cout << "JMP_REGINDIMM_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDSYM_OPERAND:
					cout << "JMP_REGINDSYM_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_DEC_OPERAND:
					cout << "JMP_MEMDIR_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_HEX_OPERAND:
					cout << "JMP_MEMDIR_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_SYM_OPERAND:
					cout << "JMP_MEMDIR_SYM_OPERAND" << " ";
					break;
				}
				if (l.op1.reg_number != -1) cout << "r" << l.op1.reg_number;
				if (l.op1.high_low != -1) {
					if (l.op1.high_low == 1) {
						cout << "h";
					}
					else cout << "l";
				}
				if (l.op1.reg_number != -1) cout << " ";
				if (l.op1.symbol != "") cout << l.op1.symbol << " ";
				if (l.op1.literal != -1) cout << l.op1.literal << " ";
			}

			if (num_of_ops == 2) {
				switch (l.op2.token) {
				case Tokens::REG_IMM_DEC_OPERAND:
					cout << "REG_IMM_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_IMM_HEX_OPERAND:
					cout << "REG_IMM_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_SYM_OPERAND:
					cout << "REG_SYM_OPERAND" << " ";
					break;
				case Tokens::REG_REG_OPERAND:
					cout << "REG_REG_OPERAND" << " ";
					break;
				case Tokens::REG_REG_IND_OPERAND:
					cout << "REG_REG_IND_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDIMM_DEC_OPERAND:
					cout << "REG_REGINDIMM_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDIMM_HEX_OPERAND:
					cout << "REG_REGINDIMM_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_REGINDSYM_OPERAND:
					cout << "REG_REGINDSYM_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_DEC_OPERAND:
					cout << "REG_MEMDIR_DEC_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_HEX_OPERAND:
					cout << "REG_MEMDIR_HEX_OPERAND" << " ";
					break;
				case Tokens::REG_MEMDIR_SYM_OPERAND:
					cout << "REG_MEMDIR_SYM_OPERAND" << " ";
					break;
				case Tokens::JMP_IMM_DEC_OPERAND:
					cout << "JMP_IMM_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_IMM_HEX_OPERAND:
					cout << "JMP_IMM_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_SYM_OPERAND:
					cout << "JMP_SYM_OPERAND" << " ";
					break;
				case Tokens::JMP_REG_OPERAND:
					cout << "JMP_REG_OPERAND" << " ";
					break;
				case Tokens::JMP_REG_IND_OPERAND:
					cout << "JMP_REG_IND_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDIMM_DEC_OPERAND:
					cout << "JMP_REGINDIMM_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDIMM_HEX_OPERAND:
					cout << "JMP_REGINDIMM_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_REGINDSYM_OPERAND:
					cout << "JMP_REGINDSYM_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_DEC_OPERAND:
					cout << "JMP_MEMDIR_DEC_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_HEX_OPERAND:
					cout << "JMP_MEMDIR_HEX_OPERAND" << " ";
					break;
				case Tokens::JMP_MEMDIR_SYM_OPERAND:
					cout << "JMP_MEMDIR_SYM_OPERAND" << " ";
					break;
				}
				if (l.op2.reg_number != -1) cout << "r" << l.op2.reg_number;
				if (l.op2.high_low != -1) {
					if (l.op2.high_low == 1) {
						cout << "h";
					}
					else cout << "l";
				}
				if (l.op2.reg_number != -1) cout << " ";
				if (l.op2.symbol != "") cout << l.op2.symbol << " ";
				if (l.op2.literal != -1) cout << l.op2.literal << " ";
			}

			cout << endl;
		
		    
			break;
		}
	}
	*/
}
Line Compiler::process_directive(vector<string> vec, int num) {
	Line line;
	string type = vec[0].substr(1);
	line.type = Tokens::DIRECTIVE;
	line.directive = type;
	if (type == "word" || type=="byte") {
		if (vec.size() < 2) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}
		for (int i = 1; i < vec.size(); i++) {
			ListMember lm;
			string s = vec[i];
			
			if (regex_match(s, regex("^" + symbol + "$"))) {
				lm.symbol = s;
				line.directive_members.push_back(lm);
			}
			else if (regex_match(s, regex("^" + dec_literal + "$"))) {
				lm.literal = stoi(s);
				if (type == "byte") {
					if (!(lm.literal<128 && lm.literal>=-128)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
				}
				else {
					if (!(lm.literal<32768 && lm.literal>=-32768)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
				}
				line.directive_members.push_back(lm);
			}
			else if (regex_match(s, regex("^" + hex_literal + "$"))) {
				lm.literal = stoi(s,nullptr,16);
				if (type == "byte") {
					if (!(lm.literal < 128 && lm.literal >= -128)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
				}
				line.directive_members.push_back(lm);
			}
			else {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
		}
	}
	else if (type == "skip") {
		if (vec.size() != 2) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}
		else {
			string s = vec[1];
			ListMember lm;
	        if (regex_match(s, regex("^" + dec_literal + "$"))) {
		         lm.literal = stoi(s);
				 if (!(lm.literal < 32768 && lm.literal >= -32768)) {
					 cout << "Greska na liniji:" << num << endl;
					 
					 exit(-1);
				 }
		         line.directive_members.push_back(lm);
	        }
	        else if (regex_match(s, regex("^" + hex_literal + "$"))) {
		         lm.literal = stoi(s, nullptr, 16);
		         line.directive_members.push_back(lm);
	        }
	        else {
		         cout << "Greska na liniji:" << num << endl;
		         
		         exit(-1);
	        }
		}
	}
	else if (type == "equ") {
		if (vec.size() !=3) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}
		if (regex_match(vec[1], regex("^"+symbol+"$"))) {
			line.equ_symbol = vec[1];
		}
		else {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}
		string s = vec[2];
		size_t current = 0,previous=0;
		while (current < s.size()) {
			ListMember lm;
			size_t found_plus = s.find("+",previous);
			size_t found_minus = s.find("-",previous);
			if(found_plus!= string::npos && found_plus<=found_minus){
				current = found_plus - 1;
				string nx = s.substr(previous, current-previous+1);
				previous = current + 2;
				if (regex_match(nx, regex("^" + symbol + "$"))) {
					lm.symbol = nx;
					lm.op = "+";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + dec_literal + "$"))) {
					lm.literal = stoi(nx);
					if (!(lm.literal < 32768 && lm.literal >= -32768)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
					lm.op = "+";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + hex_literal + "$"))) {
					lm.literal = stoi(nx, nullptr, 16);
					lm.op = "+";
					line.directive_members.push_back(lm);
				}
				else {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				} 
				

			}
			else if(found_minus!=string::npos){
				current = found_minus - 1;
				string nx = s.substr(previous, current - previous+1);
				previous = current + 2;
				if (regex_match(nx, regex("^" + symbol + "$"))) {
					lm.symbol = nx;
					lm.op = "-";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + dec_literal + "$"))) {
					lm.literal = stoi(nx);
					if (!(lm.literal < 32768 && lm.literal >= -32768)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
					lm.op = "-";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + hex_literal + "$"))) {
					lm.literal = stoi(nx, nullptr, 16);
					lm.op = "-";
					line.directive_members.push_back(lm);
				}
				else {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
			}
			else {
				current = s.length() - 1;
				string nx = s.substr(previous, current - previous+1);

				if (regex_match(nx, regex("^" + symbol + "$"))) {
					lm.symbol = nx;
					lm.op = "";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + dec_literal + "$"))) {
					lm.literal = stoi(nx);
					if (!(lm.literal < 32768 && lm.literal >= -32768)) {
						cout << "Greska na liniji:" << num << endl;
						
						exit(-1);
					}
					lm.op = "";
					line.directive_members.push_back(lm);
				}
				else if (regex_match(nx, regex("^" + hex_literal + "$"))) {
					lm.literal = stoi(nx, nullptr, 16);
					lm.op = "";
					line.directive_members.push_back(lm);
				}
				else {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				current = s.length() + 1;
				
			}
		}
		
	}
	else if (type == "end") {
	   if (vec.size() != 1) {
		   cout << "Greska na liniji:" << num << endl;
		   
		   exit(-1);
	   }
	   end = true;
	}
	return line;
}

Line Compiler::process_instr(vector<string> vec,int num) {
	Line line;
	line.type = Tokens::OPCODE;
	string type = "";
	if (vec[0][vec[0].length() - 1] == 'w' || (vec[0][vec[0].length() - 1] == 'b' && vec[0]!="sub")) {
		type = vec[0].substr(0, vec[0].length() - 1);
		line.spec_bw = vec[0].substr(vec[0].length() - 1, 1);
	}
	else type = vec[0];
	if (type == "int" || type == "call" || type == "jmp" || type == "jeq" || type == "jne" || type == "jgt") {
		if (vec.size() < 2 || vec.size()>=3) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}

		if (type == "int") {
			line.opcode = Opcodes::INT;
		}
		if (type == "call") {
			line.opcode = Opcodes::CALL;
		}
		if (type == "jmp") {
			line.opcode = Opcodes::JMP;
		}
		if (type == "jeq") {
			line.opcode = Opcodes::JEQ;
		}
		if (type == "jne") {
			line.opcode = Opcodes::JNE;
		}
		if (type == "jgt") {
			line.opcode = Opcodes::JGT;
		}
		
		string op1 = vec[1];
		if(regex_match(op1,regex("^" + dec_literal + "$"))){
			line.op1.literal = stoi(op1);
			if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
			if(line.op1.literal<128 && line.op1.literal>=-128)line.op1.byte_size = 1;
			else line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_IMM_DEC_OPERAND;
		}
		else if (regex_match(op1, regex("^" + hex_literal + "$"))) {
			line.op1.literal = stoi(op1,nullptr,16);
			if (line.op1.literal<128 && line.op1.literal>=-128)line.op1.byte_size = 1;
			else line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_IMM_HEX_OPERAND;
		}
		else if (regex_match(op1, regex("^" + symbol + "$"))) {
			line.op1.symbol = op1;
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_SYM_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*%" + register_op + "(h|l)?$"))) {

			line.op1.reg_number = stoi("0x"+op1.substr(3,1),nullptr,16);
			line.op1.byte_size = 2;
			if (op1.length() == 5) {
				string hl = op1.substr(4, 1);
				if (hl == "h")line.op1.high_low = 1;
				else line.op1.high_low = 0;
				line.op1.byte_size = 1;
			}
			line.op1.token = Tokens::JMP_REG_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*\\(%" + register_op + "\\)$"))) {
			line.op1.reg_number = stoi("0x" + op1.substr(4, 1), nullptr, 16);
			line.op1.byte_size = 2;
			if (op1.length() == 7) {
				string hl = op1.substr(5, 1);
				if (hl == "h")line.op1.high_low = 1;
				else line.op1.high_low = 0;
			}
			line.op1.token = Tokens::JMP_REG_IND_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + dec_literal + "\\(%" + register_op + "\\)$"))) {
			size_t parenth_found = op1.find("(");
			line.op1.literal = stoi(op1.substr(1, parenth_found-1));
			if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
			line.op1.reg_number = stoi("0x"+op1.substr(parenth_found+3, 1),nullptr,16);
			if (op1[parenth_found+4] == 'h' || op1[parenth_found+4]=='l') {
				string hl = op1.substr(parenth_found+4, 1);
				if (hl == "h")line.op1.high_low = 1;
				else line.op1.high_low = 0;
			}
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_REGINDIMM_DEC_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + hex_literal + "\\(%" + register_op + "\\)$"))) {
			size_t parenth_found = op1.find("(");
			line.op1.literal = stoi(op1.substr(1, parenth_found - 1),nullptr,16);
			line.op1.reg_number = stoi("0x"+op1.substr(parenth_found + 3, 1),nullptr,16);
			if (op1[parenth_found + 4] == 'h' || op1[parenth_found + 4] == 'l') {
				string hl = op1.substr(parenth_found + 4, 1);
				if (hl == "h")line.op1.high_low = 1;
				else line.op1.high_low = 0;
			}
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_REGINDIMM_HEX_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + symbol + "\\(%" + register_op + "\\)$"))) {
			size_t parenth_found = op1.find("(");
			line.op1.symbol = op1.substr(1, parenth_found-1);
			line.op1.reg_number = stoi("0x"+op1.substr(parenth_found + 3, 1),nullptr,16);
			if (op1[parenth_found + 4] == 'h' || op1[parenth_found + 4] == 'l') {
				string hl = op1.substr(parenth_found + 4, 1);
				if (hl == "h")line.op1.high_low = 1;
				else line.op1.high_low = 0;
			}
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_REGINDSYM_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + symbol + "\\(%pc\\)$"))) {
			size_t parenth_found = op1.find("(");
			line.op1.symbol = op1.substr(1, parenth_found-1);
			line.op1.reg_number = 7;
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_REGINDSYM_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + dec_literal + "$"))) {
			line.op1.literal = stoi(op1.substr(1));
			if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_MEMDIR_DEC_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + hex_literal + "$"))) {
			line.op1.literal = stoi(op1.substr(1), nullptr, 16);
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_MEMDIR_HEX_OPERAND;
		}
		else if (regex_match(op1, regex("^\\*" + symbol + "$"))) {
			line.op1.symbol = op1.substr(1);
			line.op1.byte_size = 2;
			line.op1.token = Tokens::JMP_MEMDIR_SYM_OPERAND;
		}
		else {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}


		if (line.spec_bw == "w") {
			if (line.op1.token == Tokens::JMP_REG_OPERAND && line.op1.byte_size == 1) {
				cout << "Lose specificirana velicina operanada na liniji:" << num << endl;
				
				exit(-1);
			}
		}

		if (line.spec_bw == "") {
			int m = line.op1.byte_size;
			if (m == 0)line.spec_bw = "w";
			if (m == 1)line.spec_bw = "b";
			if (m == 2) line.spec_bw = "w";
		}
	}
	else if (type == "halt" || type == "iret" || type == "ret" || type == "push" || type == "pop" || type == "xchg" || type=="mov" ||type=="add" || type=="sub" || type=="mul" || type=="div" || type=="cmp" || type=="not" || type=="and" || type=="or" || type=="xor" || type=="test" || type=="shl" || type=="shr") {

		if (type == "halt") {
			line.opcode = Opcodes::HALT;
		}
		if (type == "iret") {
			line.opcode = Opcodes::IRET;
		}
		if (type == "ret") {
			line.opcode = Opcodes::RET;
		}
		if (type == "push") {
			line.opcode = Opcodes::PUSH;
		}
		if (type == "pop") {
			line.opcode = Opcodes::POP;
		}
		if (type == "xchg") {
			line.opcode = Opcodes::XCHG;
		}
		if (type == "mov") {
			line.opcode = Opcodes::MOV;
		}
		if (type == "add") {
			line.opcode = Opcodes::ADD;
		}
		if (type == "sub") {
			line.opcode = Opcodes::SUB;
		}
		if (type == "mul") {
			line.opcode = Opcodes::MUL;
		}
		if (type == "cmp") {
			line.opcode = Opcodes::CMP;
		}
		if (type == "not") {
			line.opcode = Opcodes::NOT;
		}
		if (type == "and") {
			line.opcode = Opcodes::AND;
		}
		if (type == "or") {
			line.opcode = Opcodes::OR;
		}
		if (type == "xor") {
			line.opcode = Opcodes::XOR;
		}
		if (type == "test") {
			line.opcode = Opcodes::TEST;
		}
		if (type == "shl") {
			line.opcode = Opcodes::SHL;
		}
		if (type == "shr") {
			line.opcode = Opcodes::SHR;
		}



		if ((type=="halt" || type=="iret"  || type=="ret" ) && vec.size() >1) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}

		if (vec.size() > 1) {
			string op1 = vec[1];
			if (regex_match(op1, regex("^\\$" + dec_literal + "$"))) {
				line.op1.literal = stoi(op1.substr(1));
				if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				if (line.op1.literal<128 && line.op1.literal>=-128)line.op1.byte_size = 1;
				else line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_IMM_DEC_OPERAND;
			}
			else if (regex_match(op1, regex("^\\$" + hex_literal + "$"))) {
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op1.literal = stoi(op1.substr(1), nullptr, 16);
				if (line.op1.literal<128 && line.op1.literal>=-128)line.op1.byte_size = 1;
				else line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_IMM_HEX_OPERAND;
			}
			else if (regex_match(op1, regex("^\\$" + symbol + "$"))) {
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op1.symbol = op1.substr(1);
				line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_SYM_OPERAND;
			}
			else if (regex_match(op1, regex("^%" + register_op + "(h|l)?$"))) {
				line.op1.reg_number = stoi("0x"+op1.substr(2),nullptr,16);
				line.op1.byte_size = 2;
				if (op1.length() == 4) {
					string hl = op1.substr(3, 1);
					if (hl == "h")line.op1.high_low = 1;
					else line.op1.high_low = 0;
					line.op1.byte_size = 1;
				}
				
				line.op1.token = Tokens::REG_REG_OPERAND;
			}
			else if (regex_match(op1, regex("^\\(%" + register_op + "\\)$"))) {
				line.op1.reg_number = stoi("0x"+op1.substr(3, 1),nullptr,16);
				line.op1.byte_size = 2;
				if (op1.length() == 6) {
					string hl = op1.substr(4, 1);
					if (hl == "h")line.op1.high_low = 1;
					else line.op1.high_low = 0;
				}
				line.op1.token = Tokens::REG_REG_IND_OPERAND;
			}
			else if (regex_match(op1, regex("^" + dec_literal + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op1.find("(");
				line.op1.literal = stoi(op1.substr(0, parenth_found));
				if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op1.reg_number = stoi("0x"+op1.substr(parenth_found + 3, 1),nullptr,16);
				line.op1.byte_size = 2;
				if (op1[parenth_found + 4] == 'h' || op1[parenth_found + 4] == 'l') {
					string hl = op1.substr(parenth_found + 4, 1);
					if (hl == "h")line.op1.high_low = 1;
					else line.op1.high_low = 0;
				}
				line.op1.token = Tokens::REG_REGINDIMM_DEC_OPERAND;
			}
			else if (regex_match(op1, regex("^" + hex_literal + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op1.find("(");
				line.op1.literal = stoi(op1.substr(0, parenth_found), nullptr, 16);
				line.op1.reg_number = stoi("0x"+op1.substr(parenth_found + 3, 1),nullptr,16);
				line.op1.byte_size = 2;
				if (op1[parenth_found + 4] == 'h' || op1[parenth_found + 4] == 'l') {
					string hl = op1.substr(parenth_found + 4, 1);
					if (hl == "h")line.op1.high_low = 1;
					else line.op1.high_low = 0;
				}
				line.op1.token = Tokens::REG_REGINDIMM_HEX_OPERAND;
			}
			else if (regex_match(op1, regex("^" + symbol + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op1.find("(");
				line.op1.symbol = op1.substr(0, parenth_found);
				line.op1.reg_number = stoi("0x"+op1.substr(parenth_found + 3, 1),nullptr,16);
				line.op1.byte_size = 2;
				if (op1[parenth_found + 4] == 'h' || op1[parenth_found + 4] == 'l') {
					string hl = op1.substr(parenth_found + 4, 1);
					if (hl == "h")line.op1.high_low = 1;
					else line.op1.high_low = 0;
				}
				line.op1.token = Tokens::REG_REGINDSYM_OPERAND;
			}
			else if (regex_match(op1, regex("^" + symbol + "\\(%pc\\)$"))) {
				size_t parenth_found = op1.find("(");
				line.op1.symbol = op1.substr(0, parenth_found);
				line.op1.reg_number = 7;
				line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_REGINDSYM_OPERAND;
			}
			else if (regex_match(op1, regex("^" + dec_literal + "$"))) {
				line.op1.literal = stoi(op1);
				if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_MEMDIR_DEC_OPERAND;
			}
			else if (regex_match(op1, regex("^" + hex_literal + "$"))) {
				line.op1.literal = stoi(op1, nullptr, 16);
				line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_MEMDIR_HEX_OPERAND;
			}
			else if (regex_match(op1, regex("^" + symbol + "$"))) {
				line.op1.symbol = op1;
				line.op1.byte_size = 2;
				line.op1.token = Tokens::REG_MEMDIR_SYM_OPERAND;
			}
			else {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
		}
		

		if ((type == "push" || type=="pop") && vec.size() > 2) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}

		if ((type == "pop") && (line.op1.token == Tokens::REG_IMM_DEC_OPERAND || line.op1.token == Tokens::REG_IMM_HEX_OPERAND || line.op1.token == Tokens::REG_SYM_OPERAND)) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}

		if (vec.size() > 2) {
			string op2 = vec[2];
			if (regex_match(op2, regex("^\\$" + dec_literal + "$")) && type=="shr") {
				line.op2.literal = stoi(op2.substr(1));
				if (!(line.op2.literal < 32768 && line.op2.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				if (line.op2.literal<128 && line.op2.literal>=-128)line.op2.byte_size = 1;
				else line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_IMM_DEC_OPERAND;
			}
			else if (regex_match(op2, regex("^\\$" + hex_literal + "$")) && type=="shr") {
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op2.literal = stoi(op2.substr(1), nullptr, 16);
				if (line.op2.literal<128 && line.op1.literal>=-128)line.op2.byte_size = 1;
				else line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_IMM_HEX_OPERAND;
			}
			else if (regex_match(op2, regex("^\\$" + symbol + "$")) && type=="shr") {
				if(type=="xchg"){
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op2.symbol = op2.substr(1);
				line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_SYM_OPERAND;
			}
			else if (regex_match(op2, regex("^%" + register_op +  "(h|l)?$"))) {
				line.op2.reg_number = stoi("0x"+op2.substr(2),nullptr,16);
				line.op2.byte_size = 2;
				if (op2.length() == 4) {
					string hl = op2.substr(3, 1);
					if (hl == "h")line.op2.high_low = 1;
					else line.op2.high_low = 0;
					line.op2.byte_size = 1;
				}
				line.op2.token = Tokens::REG_REG_OPERAND;
			}
			else if (regex_match(op2, regex("^\\(%" + register_op + "\\)$"))) {
				line.op2.reg_number = stoi("0x"+op2.substr(3, 1),nullptr,16);
				line.op2.byte_size = 2;
				if (op2.length() == 6) {
					string hl = op2.substr(4, 1);
					if (hl == "h")line.op2.high_low = 1;
					else line.op2.high_low = 0;
				}
				line.op2.token = Tokens::REG_REG_IND_OPERAND;
			}
			else if (regex_match(op2, regex("^" + dec_literal + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op2.find("(");
				line.op2.literal = stoi(op2.substr(0, parenth_found));
				if (!(line.op2.literal < 32768 && line.op2.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op2.reg_number = stoi("0x"+op2.substr(parenth_found + 3, 1),nullptr,16);
				line.op2.byte_size = 2;
				if (op2[parenth_found + 4] == 'h' || op2[parenth_found + 4] == 'l') {
					string hl = op2.substr(parenth_found + 4, 1);
					if (hl == "h")line.op2.high_low = 1;
					else line.op2.high_low = 0;
				}
				line.op2.token = Tokens::REG_REGINDIMM_DEC_OPERAND;
			}
			else if (regex_match(op2, regex("^" + hex_literal + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op2.find("(");
				line.op2.literal = stoi(op2.substr(0, parenth_found), nullptr, 16);
				line.op2.reg_number = stoi("0x"+op2.substr(parenth_found + 3, 1),nullptr,16);
				line.op2.byte_size = 2;
				if (op2[parenth_found + 4] == 'h' || op2[parenth_found + 4] == 'l') {
					string hl = op2.substr(parenth_found + 4, 1);
					if (hl == "h")line.op2.high_low = 1;
					else line.op2.high_low = 0;
				}
				line.op2.token = Tokens::REG_REGINDIMM_HEX_OPERAND;
			}
			else if (regex_match(op2, regex("^" + symbol + "\\(%" + register_op + "\\)$"))) {
				size_t parenth_found = op2.find("(");
				line.op2.symbol = op2.substr(0, parenth_found);
				line.op2.reg_number = stoi("0x"+op2.substr(parenth_found + 3, 1),nullptr,16);
				line.op2.byte_size = 2;
				if (op2[parenth_found + 4] == 'h' || op2[parenth_found + 4] == 'l') {
					string hl = op2.substr(parenth_found + 4, 1);
					if (hl == "h")line.op2.high_low = 1;
					else line.op2.high_low = 0;
				}
				line.op2.token = Tokens::REG_REGINDSYM_OPERAND;
			}
			else if (regex_match(op2, regex("^" + symbol + "\\(%pc\\)$"))) {
				size_t parenth_found = op2.find("(");
				line.op2.symbol = op2.substr(0, parenth_found);
				line.op2.reg_number = 7;
				line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_REGINDSYM_OPERAND;
			}
			else if (regex_match(op2, regex("^" + dec_literal + "$"))) {
				line.op2.literal = stoi(op2);
				if (!(line.op1.literal < 32768 && line.op1.literal >= -32768)) {
					cout << "Greska na liniji:" << num << endl;
					
					exit(-1);
				}
				line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_MEMDIR_DEC_OPERAND;
			}
			else if (regex_match(op2, regex("^" + hex_literal + "$"))) {
				line.op2.literal = stoi(op2, nullptr, 16);
				line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_MEMDIR_HEX_OPERAND;
			}
			else if (regex_match(op2, regex("^" + symbol + "$"))) {
				line.op2.symbol = op2;
				line.op2.byte_size = 2;
				line.op2.token = Tokens::REG_MEMDIR_SYM_OPERAND;
			}
			else {
				cout << "Greska na liniji:" << num << endl;
				
				exit(-1);
			}
		}
		

		if (vec.size() > 3) {
			cout << "Greska na liniji:" << num << endl;
			
			exit(-1);
		}

		if(type=="shr"){
           if(line.op1.token==Tokens::REG_IMM_DEC_OPERAND || line.op1.token==Tokens::REG_IMM_HEX_OPERAND || line.op1.token==Tokens::REG_SYM_OPERAND){
			   cout << "Greska na liniji:" << num << endl;
			   
			   exit(-1);
		   }
		}

		bool m1_concludable = false;
		bool m2_concludable = false;

		
		if (line.op1.byte_size > 0) {
			if (line.op1.token == Tokens::REG_IMM_DEC_OPERAND || line.op1.token == Tokens::REG_IMM_HEX_OPERAND || line.op1.token == Tokens::REG_SYM_OPERAND || line.op1.token == Tokens::REG_REG_OPERAND) m1_concludable = true;
		}

		if (line.op2.byte_size > 0) {
			if (line.op2.token == Tokens::REG_IMM_DEC_OPERAND || line.op2.token == Tokens::REG_IMM_HEX_OPERAND || line.op2.token == Tokens::REG_SYM_OPERAND || line.op2.token == Tokens::REG_REG_OPERAND) m2_concludable = true;
		}

		if (line.spec_bw == "") {
			if (line.op1.byte_size==2 && line.op2.byte_size==2 && !m1_concludable && !m2_concludable) {
				cout << "Specificirati vel. operanada na liniji:" << num << endl;
				
				exit(-1);
			}
		}

		if (line.spec_bw == "w") {
			if ((line.op1.byte_size == 1 &&  line.op1.token==Tokens::REG_REG_OPERAND) || (line.op2.byte_size==1 && line.op1.token == Tokens::REG_REG_OPERAND)) {
				cout << "Losa vel. operanda ili operanada na liniji:" << num << endl;
				
				exit(-1);
			}
		}

		

		if (line.spec_bw == "") {
			int m = line.op1.byte_size;
			if (line.op2.byte_size > m) m = line.op2.byte_size;
			if (m == 0)line.spec_bw = "w";
			if (m == 1)line.spec_bw = "b";
			if (m == 2) line.spec_bw = "w";
		}

		

		


	}
	else {
	    cout << "Greska na liniji:" << num << endl;
	    
	    exit(-1);
    }
	return line;
}

void Compiler::Transcribe(string file1, string file2) {
	f1.open(file1,ifstream::in);
	AnalyseTheFile();
	RunThroughTheFile();
	EquTable();
	SolveRelBlocks();
	f2.open(file2, ofstream::out);
	WriteFile();
	f1.close();
	f2.close();
}

void Compiler::RunThroughTheFile() {
	int counter = 0;
	string section="";
	int section_num = -1;
	SectionMaterial sm;
	for (int i = 0; i < true_lines.size(); i++) {
		if (true_lines[i].label != "") {
			if (section_num == -1) {
				cout << "Greska u obradi u liniji:" << i << endl;
				
				exit(-1);
			}
			int idx = -1;
			for (int k = 0; k < symbol_table.size(); k++) {
				
				if (symbol_table[k].name == true_lines[i].label) {
					idx = k;
					break;
				}
				
			}
			if (idx != -1) {
				symbol_table[idx].val = counter;
				if (symbol_table[idx].section != "") {
					cout << "Greska u obradi u liniji:" << i << endl;
					
					exit(-1);
				}
				symbol_table[idx].section = section;
				if (symbol_table[idx].glob_loc == -1) {
					symbol_table[idx].glob_loc = 0;
				}
			}
			else {
				Entry entry;
				entry.name = true_lines[i].label;
				entry.section = section;
				entry.val = counter;
				entry.section = section;
				entry.glob_loc = 0;
				symbol_table.push_back(entry);
			}

		}
		switch (true_lines[i].type) {
		case Tokens::SECTION: {
			if (section_num != -1) {
				symbol_table[section_num].size = counter;
				sm.sec_name = section;
				end_code.push_back(sm);
				sm.instructions.clear();
				sm.code.clear();
				sm.rel.clear();
				counter = 0;
			}
			for (int t = 0; t < end_code.size(); t++) {
				if (end_code[t].sec_name == true_lines[i].section.name) {
					cout << "Nemoguce je imati dve sekcije istog imena." << endl;
					
					exit(-1);
				}
			}
			section = true_lines[i].section.name;
			section_num = symbol_table.size();
			Entry sec_entry;
			sec_entry.name = section;
			sec_entry.section = section;
			sec_entry.val = 0;
			sec_entry.glob_loc = 1;
			symbol_table.push_back(sec_entry);
			break;
		}
		case Tokens::ACCESSOR: {
			if (true_lines[i].accessor == "global") {
				for (int j = 0; j < true_lines[i].directive_members.size(); j++) {
					int idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].directive_members[j].symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						symbol_table[idx].glob_loc = 1;
					}
					else {
						Entry entry;
						entry.name = true_lines[i].directive_members[j].symbol;
						entry.glob_loc = 1;
						symbol_table.push_back(entry);
					}
				}
			}
			else {
				for (int j = 0; j < true_lines[i].directive_members.size(); j++) {
					int idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].directive_members[j].symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						symbol_table[idx].glob_loc = 1;
						symbol_table[idx].val = 0;
						symbol_table[idx].section = "UNDEFINED";
					}
					else {
						Entry entry;
						entry.name = true_lines[i].directive_members[j].symbol;
						entry.section = "UNDEFINED";
						entry.val = 0;
						entry.glob_loc = 1;
						symbol_table.push_back(entry);
					}
				}
			}
		}
		break;
		case Tokens::DIRECTIVE: {
			if (section_num == -1) {
				cout << "Greska u obradi u liniji:" << i << endl;
				
				exit(-1);
			}

			if (true_lines[i].directive == "equ") {
				pair<int, Line> p;
				p.first = section_num;
				p.second = true_lines[i];
				equ_table.push_back(p);
			}
			else if (true_lines[i].directive == "skip") {
				for (int j = 0; j < true_lines[i].directive_members[0].literal; j++)sm.code.push_back(0x00);
				counter += true_lines[i].directive_members[0].literal;
			}
			else if (true_lines[i].directive == "byte") {
				for (int j = 0; j < true_lines[i].directive_members.size(); j++) {
					if (true_lines[i].directive_members[j].symbol == "") {
						sm.code.push_back(true_lines[i].directive_members[j].literal);
						counter++;
					}
					else {
						int idx = -1;
						for (int k = 0; k < symbol_table.size(); k++) {

							if (symbol_table[k].name == true_lines[i].directive_members[j].symbol) {
								idx = k;
								break;
							}

						}
						if (idx != -1) {
							RelBlock rb;
							rb.address = counter;
							rb.type = 1;
							rb.byte_size = 1;
							rb.table_num = section_num;
							symbol_table[idx].flink.push_back(rb);
						}
						else {
							Entry entry;
							entry.name = true_lines[i].directive_members[j].symbol;
							RelBlock rb;
							rb.address = counter;
							rb.type = 1;
							rb.byte_size = 1;
							rb.table_num = section_num;
							entry.flink.push_back(rb);
							symbol_table.push_back(entry);
						}
						sm.code.push_back(0x00);
						counter++;
					}
				}
			}
			else {
				for (int j = 0; j < true_lines[i].directive_members.size(); j++) {
					if (true_lines[i].directive_members[j].symbol == "") {
						short lit = true_lines[i].directive_members[j].literal;
						char bit = 0x00;
						bit = (lit & 0xff);
						sm.code.push_back(bit);
						counter++;
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;

					}
					else {
						int idx = -1;
						for (int k = 0; k < symbol_table.size(); k++) {

							if (symbol_table[k].name == true_lines[i].directive_members[j].symbol) {
								idx = k;
								break;
							}

						}
						if (idx != -1) {
							RelBlock rb;
							rb.address = counter;
							rb.type = 1;
							rb.byte_size = 2;
							rb.table_num = section_num;
							symbol_table[idx].flink.push_back(rb);
						}
						else {
							Entry entry;
							entry.name = true_lines[i].directive_members[j].symbol;
							RelBlock rb;
							rb.address = counter;
							rb.type = 1;
							rb.byte_size = 2;
							rb.table_num = section_num;
							entry.flink.push_back(rb);
							symbol_table.push_back(entry);
						}
						sm.code.push_back(0x00);
						counter++;
						sm.code.push_back(0x00);
						counter++;
					}
				}
			}
		}
		break;
		case Tokens::OPCODE: {
			if (section_num == -1) {
				cout << "Greska u obradi u liniji:" << i << endl;
				
				exit(-1);
			}
			char instr = 0x00;
			int opcode = true_lines[i].opcode;
			instr = (opcode & 0xff);
			instr <<= 1;
			int one_two = 1;
			if (true_lines[i].spec_bw != "b") {
				instr |= 1;
				one_two = 2;
			}
			instr <<= 2;
			sm.code.push_back(instr);
			counter++;
			char bit = 0x0;
			short lit = 0x0;
			int idx = -1;
			if (true_lines[i].opcode > 2) {
				switch (true_lines[i].op1.token) {
				case Tokens::REG_IMM_DEC_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
					
				}
				break;
				case Tokens::REG_IMM_HEX_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
				}
				break;
				case Tokens::REG_SYM_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < one_two; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-one_two;
						rb.type = 1;
						rb.byte_size = one_two;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-one_two;
						rb.byte_size = one_two;
						rb.type = 1;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::REG_REG_OPERAND: {
					bit = 0x1 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REG_IND_OPERAND: {
					bit = 0x2 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDIMM_DEC_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDIMM_HEX_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDSYM_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < 2; k++) {

						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::REG_MEMDIR_DEC_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_MEMDIR_HEX_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_MEMDIR_SYM_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < 2; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::JMP_IMM_DEC_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
				}
				break;
				case Tokens::JMP_IMM_HEX_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
				}
				break;
				case Tokens::JMP_SYM_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < one_two; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					int idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-one_two;
						rb.type = 2;
						rb.byte_size = one_two;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-one_two;
						rb.type = 2;
						rb.byte_size = one_two;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::JMP_REG_OPERAND: {
					bit = 0x1 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_REG_IND_OPERAND: {
					bit = 0x2 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_REGINDIMM_DEC_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_REGINDIMM_HEX_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_REGINDSYM_OPERAND: {
					char bit = 0x3 << 5;
					bit |= (true_lines[i].op1.reg_number << 1);
					if (true_lines[i].op1.high_low > -1)bit |= true_lines[i].op1.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < 2; k++) {

						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 2;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 2;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::JMP_MEMDIR_DEC_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_MEMDIR_HEX_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::JMP_MEMDIR_SYM_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op1.literal;
					for (int k = 0; k < 2; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op1.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 2;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op1.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 2;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				}
			}
			else {
			   sm.instructions.push_back(counter);
			   continue;
            }

			if (true_lines[i].opcode > 10) {
				switch (true_lines[i].op2.token) {
				case Tokens::REG_IMM_DEC_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
					
				}
				break;
				case Tokens::REG_IMM_HEX_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					if (one_two > 1) {
						bit = ((lit >> 8) & 0xff);
						sm.code.push_back(bit);
						counter++;
					}
				}
				break;
				case Tokens::REG_SYM_OPERAND: {
					bit = 0x0;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					for (int k = 0; k < one_two; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op2.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-one_two;
						rb.type = 1;
						rb.byte_size = one_two;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op2.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-one_two;
						rb.byte_size = one_two;
						rb.type = 1;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::REG_REG_OPERAND: {
					bit = 0x1 << 5;
					bit |= (true_lines[i].op2.reg_number << 1);
					if (true_lines[i].op2.high_low > -1)bit |= true_lines[i].op2.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REG_IND_OPERAND: {
					bit = 0x2 << 5;
					bit |= (true_lines[i].op2.reg_number << 1);
					if (true_lines[i].op2.high_low > -1)bit |= true_lines[i].op2.high_low;
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDIMM_DEC_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op2.reg_number << 1);
					if (true_lines[i].op2.high_low > -1)bit |= true_lines[i].op2.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDIMM_HEX_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op2.reg_number << 1);
					if (true_lines[i].op2.high_low > -1)bit |= true_lines[i].op2.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_REGINDSYM_OPERAND: {
					bit = 0x3 << 5;
					bit |= (true_lines[i].op2.reg_number << 1);
					if (true_lines[i].op2.high_low > -1)bit |= true_lines[i].op2.high_low;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					for (int k = 0; k < 2; k++) {

						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op2.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op2.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				case Tokens::REG_MEMDIR_DEC_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_MEMDIR_HEX_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					bit = (lit & 0xff);
					sm.code.push_back(bit);
					counter++;
					bit = ((lit >> 8) & 0xff);
					sm.code.push_back(bit);
					counter++;
				}
				break;
				case Tokens::REG_MEMDIR_SYM_OPERAND: {
					bit = 0x4 << 5;
					sm.code.push_back(bit);
					counter++;
					lit = true_lines[i].op2.literal;
					for (int k = 0; k < 2; k++) {
						sm.code.push_back(0x00);
						counter++;
					}
					idx = -1;
					for (int k = 0; k < symbol_table.size(); k++) {

						if (symbol_table[k].name == true_lines[i].op2.symbol) {
							idx = k;
							break;
						}

					}
					if (idx != -1) {
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						symbol_table[idx].flink.push_back(rb);
					}
					else {
						Entry entry;
						entry.name = true_lines[i].op2.symbol;
						entry.glob_loc = 0;
						RelBlock rb;
						rb.address = counter-2;
						rb.type = 1;
						rb.byte_size = 2;
						rb.table_num = section_num;
						entry.flink.push_back(rb);
						symbol_table.push_back(entry);
					}
				}
				break;
				}
			}
			else {
			    sm.instructions.push_back(counter);
				continue;
            }

			sm.instructions.push_back(counter);
		}
		break;
		}
	}
	if (sm.sec_name != "") {
		symbol_table[section_num].size = counter;
		sm.sec_name = section;
		end_code.push_back(sm);
	}

	/*
	cout << "==================" << endl;
	cout << "Tabela simbola:" << endl;
	for (int i = 0; i < symbol_table.size(); i++) {
		cout << symbol_table[i].name << " " << symbol_table[i].section << " " << symbol_table[i].val << " " << symbol_table[i].glob_loc << " "<<symbol_table[i].size<<endl;
		cout << "xxxx" << endl;
		for (int j = 0; j < symbol_table[i].flink.size(); j++) {
			cout << symbol_table[i].flink[j].address << " " << symbol_table[i].flink[j].type << " " << symbol_table[i].flink[j].table_num << " "<<symbol_table[i].flink[j].byte_size<< endl;
		}
	}
	for (int i = 0; i < end_code.size(); i++) {
		cout << end_code[i].sec_name << endl;
		for (int j = 0; j < end_code[i].code.size(); j++) {
			cout << hex << (0xFF & end_code[i].code[j]) << " ";
		}
		cout <<endl<< "||||||||" << endl;
		for (int j = 0; j < end_code[i].instructions.size(); j++) {
			cout << dec << end_code[i].instructions[j] << " ";
		}
		cout << endl;
	}
	*/

}

void Compiler::EquTable() {
	bool solved = false;
	map<string, pair<string,int>> m;
	
	if (equ_table.size()>0) {
		int s = equ_table.size();
		int c = 0;
		while (c<s) {
			pair<int, Line> p = equ_table[0];
			equ_table.erase(equ_table.begin());
			Line l = p.second;
			int expr = 0;
			string sym = l.equ_symbol;
			char oper = '0';
			bool done = false;
			for (int i = 0; i < l.directive_members.size(); i++) {
				ListMember lm = l.directive_members[i];
				if (lm.symbol == ""){
					if(oper=='0' || oper=='+'){
						expr += lm.literal;
					}
					else{
						expr -= lm.literal;
					}
					if (lm.op == "")oper = '0';
					if (lm.op == "+")oper = '+';
					if (lm.op == "-")oper = '-';
				}
				else {
					int idx = -1;
					for (int j = 0; j < symbol_table.size(); j++) {
						if (symbol_table[j].name == lm.symbol) {
							idx = j;
							break;
						}
					}
					if (idx == -1) {
						done = true;
						break;
					}
					else {
						if (symbol_table[idx].val == -1) {
							done = true;
							break;
						}
						else {
							if (oper == '0' || oper == '+') {
								expr += symbol_table[idx].val;
							}
							else {
								expr -= symbol_table[idx].val;
							}
							

							if (m.find(symbol_table[idx].section) != m.end()) {
								pair<string, int> p = m[symbol_table[idx].section];
								if (oper == '0' || oper == '+' || symbol_table[idx].section=="UNDEFINED") {
									p.second++;
								}
								else {
									p.second--;
								}
								m[symbol_table[idx].section] = p;
							}
							else {
								pair<string, int> p;
								p.first = symbol_table[idx].name;
								p.second = 1;
								m[symbol_table[idx].section] = p;
							}

							if (lm.op == "")oper = '0';
							if (lm.op == "+")oper = '+';
							if (lm.op == "-")oper = '-';
						}
					}
				}

			}
			c++;
			if (!done) {
				if(m.size()==0){
                    solved=true;
					int idx = -1;
					for (int j = 0; j < symbol_table.size(); j++) {
						if (symbol_table[j].name == l.equ_symbol) {
							idx = j;
							break;
						}
					}
					if(idx!=-1){
						if (symbol_table[idx].section == "UNDEFINED") {
							cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
							
							exit(-1);
						}
						symbol_table[idx].val = expr;
						symbol_table[idx].section = "ABSOLUTE";
						if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;

					}
					else{
						Entry entry;
						entry.name = l.equ_symbol;
						entry.glob_loc = 0;
						entry.val = expr;
						entry.section = "ABSOLUTE";
						symbol_table.push_back(entry);

					}

				}
				else{
					string sym = "";
				    int x = 0;
					bool undef=false;
					for (map<string, pair<string, int>>::iterator it = m.begin(); it != m.end(); ++it) {
						pair<string, int> p = it->second;
						if (p.second == 1 && it->first!="ABSOLUTE") {
							if (x > 0) {
								cout << "Greska u equ izrazu za simbol:"<<l.equ_symbol<<endl;
								
								exit(-1);
							}

							x = 1;
							sym = p.first;
							if(it->first=="UNDEFINED"){
								undef=true;
							}
						}
						else if (p.second == 0) {
							continue;
						}
						else {
							if(it->first!="ABSOLUTE"){
								cout << "Greska u equ izrazu za simbol:" <<l.equ_symbol<< endl;
								exit(-1);

							}
							
						}
					}

					if(sym==""){
						int idx = -1;
						for (int j = 0; j < symbol_table.size(); j++) {
							if (symbol_table[j].name == l.equ_symbol) {
								idx = j;
								break;
							}
						}
						if(idx!=-1){
							if (symbol_table[idx].section == "UNDEFINED") {
								cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
								
								exit(-1);
							}
							symbol_table[idx].val = expr;
							symbol_table[idx].section = "ABSOLUTE";
							if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;

						}
						else{
							Entry entry;
							entry.name = l.equ_symbol;
							entry.glob_loc = 0;
							entry.val = expr;
							entry.section = "ABSOLUTE";
							symbol_table.push_back(entry);

						}



					}
					else{
						int idx = -1;
						for (int j = 0; j < symbol_table.size(); j++) {
							if (symbol_table[j].name == l.equ_symbol) {
								
								idx = j;
								break;
							}
						}
						if (idx != -1) {
							if (symbol_table[idx].section == "UNDEFINED") {
								cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
								
								exit(-1);
							}
							symbol_table[idx].val = expr;
							for(int k=0;k<(int)symbol_table.size();k++){
								if(symbol_table[k].name==sym){
									symbol_table[idx].section = symbol_table[k].section;
									break;
								}
							}
							
							if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;
						}
						else {
							Entry entry;
							entry.name = l.equ_symbol;
							entry.glob_loc = 0;
							entry.val = expr;
							for(int k=0;k<(int)symbol_table.size();k++){
								if(symbol_table[k].name==sym){
									entry.section = symbol_table[k].section;
									break;
								}
							}
							symbol_table.push_back(entry);
						}

						if(undef){
							Equ_Member em;
							em.symbol=l.equ_symbol;
							em.dependent_symbol=sym;
							for(int k=0;k<(int)l.directive_members.size();k++){
								if(l.directive_members[k].symbol==sym){
									if(l.directive_members[k].op=="0" || l.directive_members[k].op=="+"){
										em.plus_minus="plus";

									}
									else{
										em.plus_minus="minus";
									}
									break;
								}

							}
							equ_rel_table.push_back(em);

						}

					}
					solved = true;
				}
			}
			else {
				equ_table.push_back(p);
			}
			m.clear();

		}
	}

	while (equ_table.size() > 0) {
		int s = equ_table.size();
		int c = 0;
		if (!solved) {
			cout << "Neizracunljivi equ izrazi." << endl;
			
			exit(-1);
		}
		solved = false;
		while (c < s) {
			pair<int, Line> p = equ_table[0];
			equ_table.erase(equ_table.begin());
			int sec_num = p.first;
			Line l = p.second;
			int expr = 0;
			string sym = l.equ_symbol;
			char oper = '0';
			bool done = false;
			for (int i = 0; i < l.directive_members.size(); i++) {
				ListMember lm = l.directive_members[i];
				if (lm.symbol == "") {
					if (oper == '0' || oper == '+') {
						expr += lm.literal;
					}
					else {
						expr -= lm.literal;
					}
					if (lm.op == "")oper = '0';
					if (lm.op == "+")oper = '+';
					if (lm.op == "-")oper = '-';
				}
				else {
					int idx = -1;
					for (int j = 0; j < symbol_table.size(); j++) {
						if (symbol_table[j].name == lm.symbol) {
							idx = j;
							break;
						}
					}
					if (idx == -1) {
						done = true;
						break;
					}
					else {
						if (symbol_table[idx].val == -1) {
							done = true;
							break;
						}
						else {
							if (oper == '0' || oper == '+') {
								expr += symbol_table[i].val;
							}
							else {
								expr -= symbol_table[i].val;
							}


							if (m.find(symbol_table[idx].section) != m.end()) {
								pair<string, int> p = m[symbol_table[idx].section];
								if (oper == '0' || oper == '+' || symbol_table[idx].section=="UNDEFINED") {
									p.second++;
								}
								else {
									p.second--;
								}
								m[symbol_table[idx].section] = p;
							}
							else {
								pair<string, int> p;
								p.first = symbol_table[idx].name;
								p.second = 1;
								m[symbol_table[idx].section] = p;
							}

							if (lm.op == "")oper = '0';
							if (lm.op == "+")oper = '+';
							if (lm.op == "-")oper = '-';
						}
					}
				}

			}
			c++;
			if (!done) {
				if(m.size()==0){
                    solved=true;
					int idx = -1;
					for (int j = 0; j < symbol_table.size(); j++) {
						if (symbol_table[j].name == l.equ_symbol) {
							idx = j;
							break;
						}
					}
					if(idx!=-1){
						if (symbol_table[idx].section == "UNDEFINED") {
							cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
							
							exit(-1);
						}
						symbol_table[idx].val = expr;
						symbol_table[idx].section = "ABSOLUTE";
						if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;

					}
					else{
						Entry entry;
						entry.name = l.equ_symbol;
						entry.glob_loc = 0;
						entry.val = expr;
						entry.section = "ABSOLUTE";
						symbol_table.push_back(entry);

					}

				}
				else{
					string sym = "";
				    int x = 0;
					bool undef=false;
					for (map<string, pair<string, int>>::iterator it = m.begin(); it != m.end(); ++it) {
						pair<string, int> p = it->second;
						if (p.second == 1 && it->first!="ABSOLUTE") {
							if (x > 0) {
								cout << "Greska u equ izrazu za simbol:"<<l.equ_symbol<<endl;
								
								exit(-1);
							}

							x = 1;
							sym = p.first;
							if(it->first=="UNDEFINED") undef=true;
						}
						else if (p.second == 0) {
							continue;
						}
						else {
							if(it->first!="ABSOLUTE"){
								cout << "Greska u equ izrazu za simbol:" <<l.equ_symbol<< endl;
								exit(-1);

							}
							
						}
					}

					if(sym==""){
						int idx = -1;
						for (int j = 0; j < symbol_table.size(); j++) {
							if (symbol_table[j].name == l.equ_symbol) {
								idx = j;
								break;
							}
						}
						if(idx!=-1){
							if (symbol_table[idx].section == "UNDEFINED") {
								cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
								
								exit(-1);
							}
							symbol_table[idx].val = expr;
							symbol_table[idx].section = "ABSOLUTE";
							if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;

						}
						else{
							Entry entry;
							entry.name = l.equ_symbol;
							entry.glob_loc = 0;
							entry.val = expr;
							entry.section = "ABSOLUTE";
							symbol_table.push_back(entry);

						}

						



					}
					else{
						int idx = -1;
						for (int j = 0; j < symbol_table.size(); j++) {
							if (symbol_table[j].name == l.equ_symbol) {
								
								idx = j;
								break;
							}
						}
						if (idx != -1) {
							if (symbol_table[idx].section == "UNDEFINED") {
								cout << "Pokusaj definicije .extern simbola:" << symbol_table[idx].name << endl;
								
								exit(-1);
							}
							symbol_table[idx].val = expr;
							for(int k=0;k<(int)symbol_table.size();k++){
								if(symbol_table[k].name==sym){
									symbol_table[idx].section = symbol_table[k].section;
									break;
								}
							}
							
							if (symbol_table[idx].glob_loc == -1) symbol_table[idx].glob_loc = 0;
						}
						else {
							Entry entry;
							entry.name = l.equ_symbol;
							entry.glob_loc = 0;
							entry.val = expr;
							for(int k=0;k<(int)symbol_table.size();k++){
								if(symbol_table[k].name==sym){
									entry.section = symbol_table[k].section;
									break;
								}
							}
							symbol_table.push_back(entry);
						}

						if(undef){
							Equ_Member em;
							em.symbol=l.equ_symbol;
							em.dependent_symbol=sym;
							for(int k=0;k<(int)l.directive_members.size();k++){
								if(l.directive_members[k].symbol==sym){
									if(l.directive_members[k].op=="0" || l.directive_members[k].op=="+"){
										em.plus_minus="plus";

									}
									else{
										em.plus_minus="minus";
									}
									break;
								}

							}
							equ_rel_table.push_back(em);

						}

					}
					solved = true;
				}
			}
			else {
				equ_table.push_back(p);
			}
			m.clear();
		}
	}

	/*cout << "=========" << endl;
	cout << "nova tabela simbola" << endl;
	for (int i = 0; i < symbol_table.size(); i++) {
		cout <<dec<< symbol_table[i].name << " " << symbol_table[i].section << " " << symbol_table[i].val << " " << symbol_table[i].glob_loc << " " << symbol_table[i].size << endl;
		cout << "xxxx" << endl;
		for (int j = 0; j < symbol_table[i].flink.size(); j++) {
			cout << symbol_table[i].flink[j].address << " " << symbol_table[i].flink[j].type << " " << symbol_table[i].flink[j].table_num << endl;
		}
	}
	cout << "========="<<endl;
	cout << "equ tabela" << endl;

	for (int i = 0; i < equ_rel_table.size();i++) {
		cout << equ_rel_table[i].first << " " << equ_rel_table[i].second << endl;
	}*/

	

}

void Compiler::SolveRelBlocks() {
	for(int i=0;i<(int)symbol_table.size();i++){
		if(symbol_table[i].section==""){
			cout<<"Nedefinisan simbol!"<<endl;
			exit(-1);
		}
	}
	for (int i = 0; i < symbol_table.size(); i++) {
		for (int j = 0; j < symbol_table[i].flink.size(); j++) {
			RelBlock rb = symbol_table[i].flink[j];
			int idx = -1;
			for (int k = 0; k < end_code.size(); k++) {
				if (end_code[k].sec_name == symbol_table[rb.table_num].name) {
					idx = k;
					break;
				}
			}
			if(symbol_table[i].section=="ABSOLUTE"){
				short s = symbol_table[i].val;
				for (int k = 0; k < rb.byte_size; k++) {
					end_code[idx].code[rb.address+k] = (s & 0xFF);
					s >>= 8;
				}
				continue;

			}
			else if (symbol_table[i].glob_loc == 0) {
				short s = symbol_table[i].val;
				for (int k = 0; k < rb.byte_size; k++) {
					end_code[idx].code[rb.address+k] = (s & 0xFF);
					s >>= 8;
				}

				bool found=false;

				for(int k=0;k<(int)symbol_table.size();k++){
					if(symbol_table[i].section==symbol_table[k].name){
                        found=true;
						rb.table_num=k;
						break;
					}
				}

				if(!found) rb.table_num=i;

				
			}
			else{
				rb.table_num=i;
			}
			end_code[idx].rel.push_back(rb);
		}
	}
	/*
	cout << "=========" << endl;
	cout << "tabela simbola" << endl;
	for (int i = 0; i < symbol_table.size(); i++) {
		cout << dec << symbol_table[i].name << " " << symbol_table[i].section << " " << symbol_table[i].val << " " << symbol_table[i].glob_loc << " " << symbol_table[i].size << endl;
	}
	cout << "=========" << endl;
	cout << "equ tabela" << endl;
	for (int i = 0; i < equ_rel_table.size(); i++) {
		cout << equ_rel_table[i].first << " " << equ_rel_table[i].second << endl;
	}
	cout << "========" << endl;
	cout << "sekcije:" << endl;
	for (int i = 0; i < end_code.size(); i++) {
		cout << end_code[i].sec_name << endl;
		for (int j = 0; j < end_code[i].code.size(); j++) {
			cout << hex << (0xFF & end_code[i].code[j]) << " ";
		}
		cout << endl << "||||||||" << endl;
		for (int j = 0; j < end_code[i].rel.size(); j++) {
			cout << dec << end_code[i].rel[j].address << " " << end_code[i].rel[j].type << " " << end_code[i].rel[j].table_num << " " << end_code[i].rel[j].byte_size << endl;
		}
		cout << endl;
	}
	*/
	
}

void Compiler::WriteFile() {
	//cout << "=========" << endl;
	f2 << "tabela simbola" << endl;
	for (int i = 0; i < symbol_table.size(); i++) {
		f2 << dec << symbol_table[i].name << " " << symbol_table[i].section << " " << symbol_table[i].val << " " << symbol_table[i].glob_loc << " " << symbol_table[i].size << endl;
	}
	f2 << "kraj tabele simbola" << endl;
	f2 << "equ tabela" << endl;
	for (int i = 0; i < equ_rel_table.size(); i++) {
		f2 << equ_rel_table[i].symbol << " " << equ_rel_table[i].dependent_symbol << " "<<equ_rel_table[i].plus_minus<< endl;
	}
	f2 <<"kraj equ tabele"<< endl;
	//cout << "sekcije:" << endl;
	for (int i = 0; i < end_code.size(); i++) {
		f2 << end_code[i].sec_name << endl;
		f2 << "kod" << endl;
		for (int j = 0; j < end_code[i].code.size(); j++) {
			f2 << hex << (0xFF & end_code[i].code[j]) << " ";
		}
		if (end_code[i].code.size() > 0)f2 << endl;
		f2 <<"kraj koda" << endl;
		f2 <<  "rel.zapisi" << endl;
		for (int j = 0; j < end_code[i].rel.size(); j++) {
			f2 << dec << end_code[i].rel[j].address << " " << end_code[i].rel[j].type << " " << end_code[i].rel[j].table_num << " " << end_code[i].rel[j].byte_size << endl;
		}
		f2 <<"kraj rel.zapisa"<< endl;
	}
}
