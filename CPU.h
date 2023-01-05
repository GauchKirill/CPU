#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

extern const char* log_file_name;

FILE* get_CPU_file();
int* get_code(FILE* CPU_file, int* cnt_cmd);

void run (int* code, int cnt_cmd);
void get_arg_for_push (int* code, int* ip, Elem_t* arg);
void get_arg_for_pop (int* code, int* ip, Elem_t** ptr_arg);
void print_error(const char* name_cmd, size_t ip);

#endif