#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DUMPLING
#define StackCtor(stk, size) Stack_Ctor((stk), (size), #stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef DUMPLING

	#define StackDump(stk, err) Stack_Dump((stk), (err), __FILE__, __PRETTY_FUNCTION__, __LINE__)

#endif

typedef int Elem_t;
extern const char* Elem_out;
extern const Elem_t POISON;
extern const int MIN_SIZE_DATA;

#ifdef DUMPLING

	extern const unsigned long long Canary;

#endif

typedef enum _Poisons {
	POISON_SIZE     = -1,
	POISON_CAPACITY = -1,
	POISON_DATA     = -1,
	POISON_NAME     =  0,
	POISON_LINE     = -1,
	POISON_HASH     = -1,
	POISON_CANARY   = -1,
} Poisons;

typedef enum _errors {
	STACK_OK         = 0x0,
	STACK_NULLPTR    = 0x1,
	STACK_NULL_DATA  = 0x2,
	INVALID_SIZE     = 0x4,
	INVALID_CAPACITY = 0x8,
	STACK_EMPTY      = 0x10,
	HAS_NOT_MEMORY   = 0x20,
	INVALID_DATA     = 0x40,
	STACK_IS_ATACKED = 0x80,
} errors;

typedef enum _Conditions {
	STACK_UP   = 1,
	STACK_DOWN = 0,
} Conditions;

#ifdef DUMPLING

	typedef struct 
	{
		const char* name;
		const char* func;
		const char* file;
		int         line;
	} stk_info;

#endif

typedef struct 
{
	#ifdef DUMPLING

		unsigned canary;

	#endif

	Elem_t* data;
	size_t  size;
	size_t  capacity;

	#ifdef DUMPLING

		stk_info           info;
		unsigned long long hash;

	#endif
} stack;

unsigned StackPush( stack* stk, Elem_t x);

unsigned StackPop( stack* stk, Elem_t* x);

unsigned Stack_Ctor( stack** stk, size_t n, const char* name_stk, const char* name_of_file, const char* name_of_func, int line);

unsigned StackDtor( stack* stk);

unsigned StackVerify( stack* stk);

unsigned StackResize( stack* stk, const int condition);

void Stack_Dump( stack* stk, const unsigned err, const char* name_wrong_file, const char* name_wrong_func, int line);

unsigned long long Stack_hash_FAQ6( const stack* stk);

unsigned Printf_stk( stack* stk);

#endif