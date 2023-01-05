#ifndef assembler_H
#define assembler_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

const size_t LENGHT_LABEL = 20;
const size_t LENGHT_FUNC  = 20;
const size_t MAX_LENGHT_CMD   = 20;

const size_t MAX_CNT_LABELS = 20;
const size_t MAX_CNT_FUNCS  = 20;

enum poisons
{
	IP_POISSON = SIZE_MAX,
};

typedef struct _labels {
	size_t  ip = IP_POISSON;
	char name[LENGHT_LABEL]; 
} labels;

typedef struct _funcs {
	size_t ip = IP_POISSON;
	char name[LENGHT_FUNC];
} funcs;

enum passes
{
	FIRST_PASS  = 1,
	SECOND_PASS = 2,
	CNT_PASSES  = 2,
};

char* 	read_text(int argc, const char** argv);
void 	get_name_assembler_file(int argc, const char** argv, char* assembler_name_file);
void 	passes(const char* assembler_code);
size_t 	first_pass(const char** assembler_code, labels* data_of_labels, funcs* data_of_funcs);
void 	add_label(const char* name_label, size_t ip, labels* data_of_labels);
void 	add_function(const char* name_func, size_t ip, funcs* data_of_funcs);
int* 	second_pass(const char** assembler_code, size_t cnt_cmd, labels* data_of_labels, funcs* data_of_funcs);
void 	get_args (const char** assembler_code, int* CPU_code, labels* data_of_labels, funcs* data_of_funcs);
size_t 	find_ip_label(char* name_label, labels* data_of_labels);
size_t 	find_ip_func(char* name_func, funcs* data_of_funcs);

#endif