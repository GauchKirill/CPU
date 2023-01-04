#ifndef DISASM_H
#define DISASM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const size_t max_cnt_labels = 20;
const size_t max_cnt_funcs  = 20;	

enum poisons
{
	IP_POISSON = SIZE_MAX,
};

typedef struct _labels
{
	size_t ip = IP_POISSON;
	int num;
} labels;

typedef struct _funcs
{
	size_t ip = IP_POISSON;
	int num;
} funcs;

enum passes
{
	FIRST_PASS  = 1,
	SECOND_PASS = 2,
	CNT_PASSES  = 2,
};

FILE* get_CPU_file();
int* get_code(FILE* CPU_file, size_t* cnt_cmd);
void passes(const int* code, size_t cnt_cmd);
void first_pass(const int* code, size_t cnt_cmd);
void second_pass(const int* code, size_t cnt_cmd);
void find_label(size_t ip, FILE* stream);
void find_func(size_t ip, FILE* stream);
void ConvertArg (const int* code, size_t* ip, FILE* stream);
void print_label(FILE* stream, const char* name_cmd, size_t label_ip);

#endif