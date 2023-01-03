#include "CPU.h"
#include "constants.h"
#include <string.h>

const size_t SIZE_RAM = 200;
Elem_t RAM[SIZE_RAM] = {0};

const int max_regs = 4;
Elem_t Regs[max_regs+1] = {0};

FILE* get_CPU_file()
{
	FILE* CPU_file = fopen (CPU_name_file, "rb");

	if (CPU_file == nullptr)
	{
		printf ("CPU: don't open file \"%s\"\n", CPU_name_file);
		return nullptr;
	}
	return CPU_file;
}

int* get_code(FILE* CPU_file, size_t cnt_cmd)
{
	int* code = (int*) calloc (cnt_cmd + 1, sizeof(int));
	if (code == nullptr)
	{
		printf ("Has not memory for code\n");
		return nullptr;
	} 
	if (cnt_cmd != fread (code, cnt_cmd, sizeof(int), CPU_file));
	{
		//printf("Cann't read all file \"%s\"\n", CPU_name_file);
	}
	return code;
}

#define DEF_CMD(name, num, arg, CPU, disasm)\
		case num:							\
			CPU								\
			break;

void Run (int* code, int cmd)
{

	stack* stk     = nullptr;
	stack* stk_ret = nullptr;
	StackCtor (&stk,     1);
	StackCtor (&stk_ret, 1);

	for (int ip = 0; ip < cmd; ip++)
	{
		Elem_t lhs = 0, rhs = 0;
		Elem_t arg = 0;
		Elem_t* ptr_arg = nullptr;
		switch (code[ip] & CMD_MASK)
		{
			#include "cmd.h"
			default:

				printf ("Not found command %x\n",code[ip]);
				break;
		}
	}

	free (stk);
}

void GetArg (int* code, int* ip, Elem_t* arg)
{
	unsigned shift = 0;
	if (code[*ip] & ARG_RAM) {
		if (code[*ip] & ARG_IMMED) *arg += code[++shift + (*ip)];
		if (code[*ip] & ARG_REG)   *arg += Regs[code[++shift + (*ip)]];
		*arg =  RAM[(int) *arg];
	} else
	{
		if (code[*ip] & ARG_IMMED) *arg += code[++shift + (*ip)];
		if (code[*ip] & ARG_REG)   *arg += Regs[code[++shift + (*ip)]];
	}
	*ip += shift;
}

void GetPtrArg (int* code, int* ip, Elem_t** ptr_arg)
{
	unsigned shift = 0;
	if (code[*ip] & ARG_RAM)
	{
		int arg = 0;
		if (code[*ip] & ARG_IMMED) arg += code[++shift + (*ip)];
		if (code[*ip] & ARG_REG)   arg += Regs[code[++shift + (*ip)]];
		*ptr_arg = &(RAM[arg]);
	} else
	{
		if (code[*ip] & ARG_IMMED) *ptr_arg += code[++shift + (*ip)];
		if (code[*ip] & ARG_REG)   *ptr_arg = &(Regs[code[++shift + (*ip)]]);
	}
	*ip += shift;
}

void print_error(const char* name_cmd, size_t ip)
{
	printf ("Error in \"%s\" command (command_ip = %lu). Please, open the \"%s\"\n", name_cmd, ip, log_file_name);
}