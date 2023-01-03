#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

extern const char* log_file_name;

FILE* get_CPU_file();
int* get_code(FILE* CPU_file, size_t cnt_cmd);

void Run (int* code, int cmd);
void GetArg (int* code, int* ip, Elem_t* arg);
void GetPtrArg (int* code, int* ip, Elem_t** ptr_arg);
void print_error(const char* name_cmd, size_t ip);

#endif