#include "Lex.h"
#include "Syntax.h"

int main(int argc, char** argv) {
	//puts(argv[0]);
	//puts(argv[1]);
	Lex* lex = new Lex(argc >= 2 ? argv[1]: 0);
	std::cout << "//////////////////\n";
  Syntax* syntax = new Syntax(lex);

	std::cout << "good job\n";
 
}