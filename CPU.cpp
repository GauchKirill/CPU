#include "CPU.h"
#include "constants.h"
#include <string.h>

const size_t SIZE_RAM = 200;
Elem_t RAM[SIZE_RAM] = {};

const int MAX_REGS = 4;
Elem_t Regs[MAX_REGS+1] = {};

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

int* get_code(FILE* CPU_file, int* cnt_cmd)
{
	fread (cnt_cmd, 1, sizeof(int), CPU_file);

	int* code = (int*) calloc ((*cnt_cmd) + 1, sizeof(int));
	CHECK_ERR(code == nullptr, "Has not memory for code\n", nullptr)
	fread (code, *cnt_cmd, sizeof(int), CPU_file);
	
	return code;
}

#define DEF_CMD(name, num, arg, CPU, disasm)\
		case num:							\
			CPU								\
			break;

void run (int* code, int cmd)
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

				printf ("Not found command %x\n", code[ip]);
				break;
		}
	}

	free (stk);
}

void get_arg_for_push (int* code, int* ip, Elem_t* arg)
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

void get_arg_for_pop (int* code, int* ip, Elem_t** ptr_arg)
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