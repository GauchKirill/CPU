#ifndef ASEMBLER_H
#define ASEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>

const size_t lenght_label = 20;
const size_t lenght_func  = 20;
const size_t max_lenght_cmd   = 20;

const size_t max_cnt_labels = 20;
const size_t max_cnt_funcs  = 20;

typedef struct _labels {
	size_t  ip = -1;
	char name[lenght_label]; 
} labels;

typedef struct _funcs {
	size_t ip = -1;
	char name[lenght_func];
} funcs;

enum passes
{
	FIRST_PASS  = 1,
	SECOND_PASS = 2,
	CNT_PASSES  = 2,
};

char* read_text(int argc, const char** argv);
char* get_name_asembler_file(int argc, const char** argv);
void passes(const char* asembler_code);
size_t first_pass(const char** asembler_code);
void add_label(const char* name_label, size_t ip);
void add_function(const char* name_func, size_t ip);
int* second_pass(const char** asembler_code, size_t cnt_cmd);
void GetArgs (const char** asembler_code, int* CPU_code);
int find_ip_label(char* name_label);
int find_ip_func(char* name_func);

#endif