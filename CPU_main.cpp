#include "CPU.h"

int main()
{
	FILE* CPU_file = get_CPU_file();
	size_t cnt_cmd = 0;
	fread (&cnt_cmd, 1, sizeof(int), CPU_file);

	int* code = get_code(CPU_file, cnt_cmd);
	Run (code, cnt_cmd);

	fclose (CPU_file);
	free (code);

	return 0;
}