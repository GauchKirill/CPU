#include "asembler.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

labels data_of_labels[max_cnt_labels] = {};
funcs  data_of_funcs [max_cnt_funcs]  = {};

char* read_text(int argc, const char** argv)
{
	strncpy(asembler_name_file, get_name_asembler_file(argc, argv), max_lenght_name);

	FILE* asembler_file = fopen(asembler_name_file,	"r");

	if (asembler_file == nullptr)
	{
		printf ("Cann't open files \"%s\"\n", asembler_name_file);
		return nullptr;
	}

	char* asembler_code = nullptr;
	struct stat stbuf 	= {};

	if (stat (asembler_name_file, &stbuf) == -1)
	{
		printf ("Can not find file \"%s\"\n", asembler_name_file);
		return 0;
	}
	else
	{
		size_t cnt_bite = stbuf.st_size;
		asembler_code = (char*) calloc (cnt_bite, sizeof (char));
		if (asembler_code == nullptr)
		{
			printf ("Has not memory to scanf the text\n");
			return nullptr;
		}
		if (cnt_bite != fread(asembler_code, sizeof(char), cnt_bite, asembler_file))
		{
			printf ("Cann't read all text in file %s\n", asembler_name_file);
			return nullptr;
		}
	}
	fclose (asembler_file);
	return asembler_code;
}

char* get_name_asembler_file(int argc, const char** argv)
{
	char name_file[max_lenght_name] = {};
	if (argc == 1)
	{
		printf("Введите название файла с кодом на языке \"асемблер\"\nили \"-\", если хотите продолжить с системным файлом\n");
		scanf("%s", name_file);

		if (strncmp(name_file, "-", 2))
			strncpy(asembler_name_file, name_file, max_lenght_name);
	}
	else
		strncpy(asembler_name_file, argv[1], max_lenght_name);

	return asembler_name_file;
}

void passes(const char* asembler_code)
{	
	size_t cnt_cmd = 0;
	int* CPU_code  = nullptr;
	unsigned which_pass = 1;
	while(which_pass <= CNT_PASSES)
	{
		const char* start = asembler_code;
		switch(which_pass++)
		{
		case FIRST_PASS:
			cnt_cmd = first_pass(&asembler_code);
			break;
		case SECOND_PASS:
			CPU_code =  second_pass(&asembler_code, cnt_cmd);
			break;
		}
		asembler_code = start;
	}
	FILE* CPU_file = fopen(CPU_name_file, "wb");
	fwrite(CPU_code, sizeof(int), cnt_cmd+1, CPU_file);
	free(CPU_code);
	fclose(CPU_file);
}

size_t first_pass(const char** asembler_code)
{
	size_t cnt_cmd = 0;
	char name_cmd[max_lenght_cmd] = {};
	int length_cmd = 0;
	while(sscanf(*asembler_code, "%s%n", name_cmd, &length_cmd) == 1)
	{
		(*asembler_code) += length_cmd;
		if (strchr(name_cmd, ':'))
			add_label(name_cmd+1, cnt_cmd);
		else if (strchr(name_cmd, '*'))
			add_function(name_cmd+1, cnt_cmd);
		else
			cnt_cmd++;
	}
	return cnt_cmd;
}

void add_label(const char* name_label, size_t ip)
{
	for (size_t i = 0; i < max_cnt_labels; i++)
	{
		if (data_of_labels[i].ip == IP_POISSON)
		{
			data_of_labels[i].ip = ip;
			strncpy(data_of_labels[i].name, name_label, lenght_label);
			break;
		}
	}
}

void add_function(const char* name_func, size_t ip)
{
	for (size_t i = 0; i < max_cnt_funcs; i++)
	{
		if (data_of_funcs[i].ip == IP_POISSON)
		{
			data_of_funcs[i].ip = ip;
			strncpy(data_of_funcs[i].name, name_func, lenght_func);
			break;
		}
	}
}

#define DEF_CMD(name, num, arg, CPU, disasm)			\
	if (strncmp(#name, name_cmd, max_lenght_cmd) == 0)	\
	{													\
		CPU_code[++ip] = num;							\
		if(arg)											\
			GetArgs(asembler_code, CPU_code + (ip++));	\
	} else

int* second_pass(const char** asembler_code, size_t cnt_cmd)
{
	int* CPU_code = (int*) calloc(cnt_cmd+1, sizeof(int));
	CPU_code[0] = cnt_cmd;
	size_t ip = 0;
	char name_cmd[max_lenght_cmd] = {};
	int length_cmd = 0;
	while(sscanf(*asembler_code, "%s%n", name_cmd, &length_cmd) == 1)
	{
		(*asembler_code) += length_cmd;
		#include "cmd.h"
		if (strchr(name_cmd, ':') || strchr(name_cmd, '*'))
			continue;
		else
			CHECK_ERR(1, "Not found command", nullptr)
	}
	return CPU_code;
}

#undef DEF_CMD

void GetArgs (const char** asembler_code, int* CPU_code)
{
	unsigned length_cmd = 0;
	if (*CPU_code == CMD_PUSH || *CPU_code == CMD_POP)
	{	
		char arg[max_lenght_cmd] = {};
		sscanf (*asembler_code, "%s%n", arg, &length_cmd);
		*asembler_code += length_cmd;

		int num = 0;
		char reg[max_lenght_cmd] = {};
		if (sscanf(arg, "[%d]", &num))
		{
			*CPU_code |= (ARG_RAM + ARG_IMMED);
			*(CPU_code+1) = num;
		}
		else if (sscanf(arg, "[%s", reg))
		{
			*CPU_code |= (ARG_RAM + ARG_REG);
			*(CPU_code+1) = reg[1] - 'a' + 1;
		}
		else if (sscanf(arg, "%d", &num))
		{
			*CPU_code |= ARG_IMMED;
			*(CPU_code+1) = num;
		}
		else
		{
			*CPU_code |= ARG_REG;
			*(CPU_code+1) = arg[1] - 'a' + 1;
		}
	} else if (*CPU_code & TYPE_JMP)
	{
		char name_label[lenght_label] = {};
		sscanf (*asembler_code, "%s%n", name_label, &length_cmd);
		*asembler_code += length_cmd;
		*(CPU_code+1) = find_ip_label(name_label);
	} else
	if (*CPU_code == CMD_CALL)
	{
		char name_func[lenght_func] = {};
		sscanf (*asembler_code, "%s%n", name_func, &length_cmd);
		*asembler_code += length_cmd; 
		*(CPU_code+1) = find_ip_func(name_func);
	}
}

size_t find_ip_label(char* name_label)
{
	for (size_t i = 0; i < max_cnt_labels; i++)
	{
		if (data_of_labels[i].ip == IP_POISSON)
			return IP_POISSON;	
		if (strncmp(data_of_labels[i].name, name_label, lenght_label) == 0)
			return data_of_labels[i].ip;		
	}
	return IP_POISSON;
}

size_t find_ip_func(char* name_func)
{
	for (size_t i = 0; i < max_cnt_funcs; i++)
	{
		if (data_of_funcs[i].ip == IP_POISSON)
			return IP_POISSON;
		if (strncmp(data_of_funcs[i].name, name_func, lenght_func) == 0)
			return data_of_funcs[i].ip;		
	}
	return IP_POISSON;
}