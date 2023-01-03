
all:
	@g++ asembler_main.cpp asembler.cpp -o asembler.out
	@g++ CPU_main.cpp CPU.cpp Stack.c -o CPU.out
	@g++ disasm_main.cpp disasm.cpp -o disasm.out 

