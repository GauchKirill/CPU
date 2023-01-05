#include "assembler.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

char* read_text(int argc, const char** argv)
{
	char assembler_name_file[MAX_LENGHT_NAME] = "assembler.txt";
	get_name_assembler_file(argc, argv, assembler_name_file);

	FILE* assembler_file = fopen(assembler_name_file,	"r");

	if (assembler_file == nullptr)
	{
		printf ("Cann't open files \"%s\"\n", assembler_name_file);
		return nullptr;
	}

	char* assembler_code = nullptr;
	struct stat stbuf 	= {};

	if (stat (assembler_name_file, &stbuf) == -1)
	{
		printf ("Can not find file \"%s\"\n", assembler_name_file);
		fclose(assembler_file);
		return nullptr;
	}

	size_t cnt_bite = stbuf.st_size;
	assembler_code = (char*) calloc (cnt_bite, sizeof (char));
	if (assembler_code == nullptr)
	{
		printf ("Has not memory to scanf the text\n");
		fclose(assembler_file);
		return nullptr;
	}
	if (cnt_bite != fread(assembler_code, sizeof(char), cnt_bite, assembler_file))
	{
		printf ("Cann't read all text in file %s\n", assembler_name_file);
		fclose(assembler_file);
		free(assembler_code);
		return nullptr;
	}

	fclose (assembler_file);
	return assembler_code;
}

void get_name_assembler_file(int argc, const char** argv, char* assembler_name_file)
{
	char name_file[MAX_LENGHT_NAME] = {};
	if (argc == 1)
	{
		printf("Enter the name of the file with the code in \"assembler\"\n or \"-\" if you want to continue with the standard file\n");
		scanf("%s", name_file);

		if (strncmp(name_file, "-", 2))
			strncpy(assembler_name_file, name_file, MAX_LENGHT_NAME);
	}
	else
		strncpy(assembler_name_file, argv[1], MAX_LENGHT_NAME);
}

void passes(const char* assembler_code)
{	
	CHECK_ERR(!assembler_code, "Invalid ptr with assembler_code\n", )

	labels data_of_labels[MAX_CNT_LABELS] = {};
	funcs  data_of_funcs [MAX_CNT_FUNCS]  = {};

	size_t cnt_cmd = 0;
	int* CPU_code  = nullptr;
	
	for(unsigned which_pass = 1; which_pass <= CNT_PASSES; which_pass++)
	{
		const char* start = assembler_code;
		if(which_pass == FIRST_PASS)
			cnt_cmd = first_pass(&assembler_code, data_of_labels, data_of_funcs);
		else
		if (which_pass == SECOND_PASS)
			CPU_code =  second_pass(&assembler_code, cnt_cmd, data_of_labels, data_of_funcs);

		assembler_code = start;
	}
	FILE* CPU_file = fopen(CPU_name_file, "wb");
	fwrite(CPU_code, sizeof(int), cnt_cmd+1, CPU_file);
	free(CPU_code);
	fclose(CPU_file);
}

size_t first_pass(const char** assembler_code, labels* data_of_labels, funcs* data_of_funcs)
{
	size_t cnt_cmd = 0;
	char name_cmd[MAX_LENGHT_CMD] = {};
	int length_cmd = 0;
	while(sscanf(*assembler_code, "%s%n", name_cmd, &length_cmd) == 1)
	{
		(*assembler_code) += length_cmd;
		if (strchr(name_cmd, ':'))
			add_label(name_cmd+1, cnt_cmd, data_of_labels);
		else if (strchr(name_cmd, '*'))
			add_function(name_cmd+1, cnt_cmd, data_of_funcs);
		else
			cnt_cmd++;
	}
	return cnt_cmd;
}

void add_label(const char* name_label, size_t ip, labels* data_of_labels)
{
	static size_t cnt_labels = 0;
	CHECK_ERR(cnt_labels >= MAX_CNT_LABELS, "Label limit exceeded\n", )
	data_of_labels[cnt_labels].ip = ip;
	strncpy(data_of_labels[cnt_labels].name, name_label, LENGHT_LABEL);
	cnt_labels++;
}

void add_function(const char* name_func, size_t ip, funcs* data_of_funcs)
{
	static size_t cnt_funcs = 0;
	CHECK_ERR(cnt_funcs >= MAX_CNT_FUNCS, "Label limit exceeded\n", )
	data_of_funcs[cnt_funcs].ip = ip;
	strncpy(data_of_funcs[cnt_funcs].name, name_func, LENGHT_FUNC);
	cnt_funcs++;
}

#define DEF_CMD(name, num, arg, CPU, disasm)			\
	if (strncmp(#name, name_cmd, MAX_LENGHT_CMD) == 0)	\
	{													\
		CPU_code[++ip] = num;							\
		if(arg)											\
			get_args(assembler_code, CPU_code + (ip++),	\
					data_of_labels, data_of_funcs);		\
	} else

int* second_pass(const char** assembler_code, size_t cnt_cmd, labels* data_of_labels, funcs* data_of_funcs)
{
	int* CPU_code = (int*) calloc(cnt_cmd+1, sizeof(int));
	CPU_code[0] = cnt_cmd;
	size_t ip = 0;
	char name_cmd[MAX_LENGHT_CMD] = {};
	int length_cmd = 0;
	while(sscanf(*assembler_code, "%s%n", name_cmd, &length_cmd) == 1)
	{
		(*assembler_code) += length_cmd;
		#include "cmd.h"
		if (strchr(name_cmd, ':') || strchr(name_cmd, '*'))
			continue;
		else
			CHECK_ERR(1, "Not found command", nullptr)
	}
	return CPU_code;
}

#undef DEF_CMD

void get_args (const char** assembler_code, int* CPU_code, labels* data_of_labels, funcs* data_of_funcs)
{
	unsigned length_cmd = 0;
	if (*CPU_code == CMD_PUSH || *CPU_code == CMD_POP)
	{	
		char arg[MAX_LENGHT_CMD] = {};
		sscanf (*assembler_code, "%s%n", arg, &length_cmd);
		*assembler_code += length_cmd;

		int num = 0;
		char reg[MAX_LENGHT_CMD] = {};
		if (sscanf(arg, "[%d]", &num))
		{
			*CPU_code |= (ARG_RAM + ARG_IMMED);
			*(CPU_code+1) = num;
		}
		else if (sscanf(arg, "[%s]", reg))
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
		char name_label[LENGHT_LABEL] = {};
		sscanf (*assembler_code, "%s%n", name_label, &length_cmd);
		*assembler_code += length_cmd;
		*(CPU_code+1) = find_ip_label(name_label, data_of_labels);
	} else
	if (*CPU_code == CMD_CALL)
	{
		char name_func[LENGHT_FUNC] = {};
		sscanf (*assembler_code, "%s%n", name_func, &length_cmd);
		*assembler_code += length_cmd; 
		*(CPU_code+1) = find_ip_func(name_func, data_of_funcs);
	}
}

size_t find_ip_label(char* name_label, labels* data_of_labels)
{
	for (size_t i = 0; i < MAX_CNT_LABELS; i++)
	{
		if (data_of_labels[i].ip == IP_POISSON)
			return IP_POISSON;	
		if (strncmp(data_of_labels[i].name, name_label, LENGHT_LABEL) == 0)
			return data_of_labels[i].ip;		
	}
	return IP_POISSON;
}

size_t find_ip_func(char* name_func, funcs* data_of_funcs)
{
	for (size_t i = 0; i < MAX_CNT_FUNCS; i++)
	{
		if (data_of_funcs[i].ip == IP_POISSON)
			return IP_POISSON;
		if (strncmp(data_of_funcs[i].name, name_func, LENGHT_FUNC) == 0)
			return data_of_funcs[i].ip;		
	}
	return IP_POISSON;
}