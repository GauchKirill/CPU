#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CHECK_ERR(err, msg, returnCode) \
		{								\
 		if (err)						\
 		{								\
 			printf(msg);				\
			return returnCode;			\
		}								\
		}

const unsigned 	MAX_LENGHT_NAME = 30;
const unsigned  LENGHT_INSTRUCTION = 50;
const char		CPU_name_file[MAX_LENGHT_NAME] 		= "CPU.txt";


#define DEF_CMD(name, num, arg, CPU_code, disasm_code) \
	CMD_##name = num,


enum CMD_ERR
{
	#include "cmd.h"
};

#undef DEF_CMD

enum masks{
	ARG_IMMED 	= 1<<5, // 0010 0000
	ARG_REG   	= 1<<6, // 0100 0000
	ARG_RAM   	= 1<<7, // 1000 0000
	CMD_MASK	= 0x1f, // 0001 1111
	TYPE_JMP	= 1<<4, // 0001 0000
};

#endif