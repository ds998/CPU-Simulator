#include "Compiler.h"



int main(int argc,char** argv) {
	if (argc != 4) {
		
		return -1;
	}
	string sef = argv[1];
	if (sef!="-o") {
		
		return -1;
	}
	Compiler c;
	c.Transcribe(argv[2], argv[3]);
	return 0;
}