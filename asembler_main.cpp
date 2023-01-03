#include "asembler.h"

int main(int argc, const char** argv)
{
	char* asembler_code = read_text(argc, argv);
	assert (asembler_code != nullptr);
	passes(asembler_code);
}