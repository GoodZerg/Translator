#include "Lex.h"

int main(int argc, char** argv) {
	//puts(argv[0]);
	//puts(argv[1]);
	Lex* lex = new Lex(argc >= 2 ? argv[1]: 0);
}