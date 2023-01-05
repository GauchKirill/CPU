CXX_FLAGS:= -Wall -Wextra

all:
	@g++ $(CXX_FLAGS) assembler_main.cpp assembler.cpp -o assembler.out
	@g++ $(CXX_FLAGS) CPU_main.cpp CPU.cpp Stack.c -o CPU.out
	@g++ $(CXX_FLAGS) disasm_main.cpp disasm.cpp -o disasm.out 

