#include "CPU.h"

int main()
{
	FILE* CPU_file = get_CPU_file();

	int cnt_cmd = 0;
	int* code = get_code(CPU_file, &cnt_cmd);
	run (code, cnt_cmd);

	fclose (CPU_file);
	free (code);

	return 0;
}