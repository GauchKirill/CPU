CXX_FLAGS:= -Wall -Wextra

all:
	@g++ $(CXX_FLAGS) asembler_main.cpp asembler.cpp -o asembler.out
	@g++ $(CXX_FLAGS) CPU_main.cpp CPU.cpp Stack.c -o CPU.out
	@g++ $(CXX_FLAGS) disasm_main.cpp disasm.cpp -o disasm.out 

