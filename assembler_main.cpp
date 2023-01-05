#include "assembler.h"

int main(int argc, const char** argv)
{
	char* assembler_code = read_text(argc, argv);
	passes(assembler_code);
}