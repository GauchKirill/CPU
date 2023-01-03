#include "disasm.h"
#include "constants.h"

char disasm_text[max_lenght_name] = "disasm.txt";

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

int* get_code(FILE* CPU_file, size_t* cnt_cmd)
{
	fread (cnt_cmd, 1, sizeof(int), CPU_file);
	int* code = (int*) calloc (*cnt_cmd + 1, sizeof(int));
	if (code == nullptr)
	{
		printf ("Has not memory for code\n");
		return nullptr;
	} 
	if (*cnt_cmd != fread (code, *cnt_cmd, sizeof(int), CPU_file));
	{
		printf("Cann't read all file \"%s\"\n", CPU_name_file);
	}
	return code;
}

labels data_of_labels[max_cnt_labels];
funcs  data_of_funcs[max_cnt_funcs];

void passes(const int* code, size_t cnt_cmd)
{
	unsigned which_pass = 1;
	while(which_pass <= CNT_PASSES)
	{
		const int* start = code;
		switch(which_pass)
		{
		case FIRST_PASS:
			first_pass(code, cnt_cmd);
			break;
		case SECOND_PASS:
			second_pass(code, cnt_cmd);
			break;
		}	
		code = start;
		which_pass++;	
	}
}

void first_pass(const int* code, size_t cnt_cmd)
{
	unsigned 	cnt_labels = 0,
				cnt_funcs  = 0;
	for(size_t ip = 0; ip < cnt_cmd; ++ip)
	{
		if (code[ip] & TYPE_JMP)
		{
			data_of_labels[cnt_labels].num = cnt_labels+1;
			data_of_labels[cnt_labels].ip = code[++ip];
			cnt_labels++;
		}
		else if (code[ip] == CMD_CALL)
		{
			data_of_funcs[cnt_funcs].num = cnt_funcs+1;
			data_of_funcs[cnt_funcs].ip = code[++ip];
			cnt_funcs++;
		}
	}
}

#define DEF_CMD(name, num, arg, CPU_code, disasm_code) 	\
		case num:										\
			disasm_code									\
			break;

void second_pass(const int* code, size_t cnt_cmd)
{
	FILE* stream = fopen(disasm_text, "w");
	unsigned 	cnt_labels = 0,
				cnt_funcs  = 0;
	for (size_t ip = 0; ip < cnt_cmd; ip++)
	{
		find_label(ip, stream);
		find_func (ip, stream);
		switch (code[ip] & CMD_MASK)
		{
			#include "cmd.h"
			default:
				printf ("Not found command %d\n", code[ip]);
				break;
		}
	}
	fclose(stream);
}

#undef DEF_CMD

void find_label(size_t ip, FILE* stream)
{
	for (int i = 0; i < max_cnt_labels && data_of_labels[i].ip != -1; i++)
		if (ip == data_of_labels[i].ip)
		{
			fprintf (stream, ":l%d\n", data_of_labels[i].num);
			break;
		}
}

void find_func(size_t ip, FILE* stream)
{
	for (int i = 0; i < max_cnt_funcs  && data_of_funcs[i].ip  != -1; i++)
		if (ip == data_of_funcs[i].ip)
		{
			fprintf (stream, ":f%d\n", data_of_funcs[i].num);
			break;
		}		
}

void ConvertArg(const int* code, size_t* ip, FILE* stream)
{
	int plus = 0;
	if (code[*ip] & ARG_RAM) {
		fprintf(stream, "[");
		if (code[*ip] & ARG_IMMED)
		{
			fprintf(stream, "%d",code[++plus + (*ip)]);
			if (code[*ip] & ARG_REG) fprintf(stream, "+r%cx", code[++plus + (*ip)] + 'a' - 1);
		} else
		{
			if (code[*ip] & ARG_REG) fprintf(stream, "r%cx", code[++plus + (*ip)] + 'a' - 1);
		}
		fprintf(stream, "]\n");
	} else
	{
		if (code[*ip] & ARG_IMMED)
		{
			fprintf(stream, "%d",code[++plus + (*ip)]);
			if (code[*ip] & ARG_REG) fprintf(stream, "+r%cx", code[++plus + (*ip)] + 'a' - 1);
		} else
		{
			if (code[*ip] & ARG_REG) fprintf(stream, "r%cx", code[++plus + (*ip)] + 'a' - 1);
		}
		fprintf(stream, "\n");
	}
	*ip += plus;
}

void print_label(FILE* stream, const char* name_cmd, size_t label_ip)
{	
	fprintf(stream, "%s l%d\n", name_cmd, data_of_labels[label_ip].num);
}
