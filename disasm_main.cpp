#include "disasm.h"

int main()
{
	FILE* CPU_file = get_CPU_file();
	size_t cnt_cmd = 0;
	int* code = get_code(CPU_file, &cnt_cmd);
	fclose (CPU_file);
	passes(code, cnt_cmd);
	free (code);

	return 0;	
}