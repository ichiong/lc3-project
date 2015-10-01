// LC-3 Project Skeleton file
// 2013-2014
//

#include <stdio.h>
#include <string.h>

// Basic Declarations
//
	typedef short int Word;
	typedef unsigned short int Address;

	#define MEMLEN 65536
	#define NBR_REGS 8

	typedef struct {
		Word memory[MEMLEN];
		Word reg[NBR_REGS];  // Note: "register" is a reserved word
		int condition_code;  // 1=positive, 2=zero, 3=negative
		Word instr_reg;
		Address pgm_counter;
		int running;         // running = 1 iff CPU is executing instructions
	} CPU;


// Prototypes
//
	// CPU manipulation functions
	//
	void init_CPU(CPU *cpu);
	Word get_memory(CPU *cpu, Address addr);
	void set_memory(CPU *cpu, Address addr, Word value);
	void dump_memory(CPU *cpu, Address from, Address to, int nonzero_only);
	Word get_register(CPU *cpu, int regNbr);
	void set_register(CPU *cpu, int regNbr, Word newValue);
	void dump_registers(CPU *cpu);
	char get_condition_code(CPU *cpu);
	void set_condition_code(CPU *cpu, Word value);

	// Instruction cycle functions
	//
	void fetch_instr(CPU *cpu);
	void instruction_cycle(CPU *cpu);

	// Functions for executing each instruction.
	// TRAP uses readchar() to read a character (GETC, IN).
	//
	void instr_ADD (CPU *cpu);
	void instr_AND (CPU *cpu);
	void instr_BR  (CPU *cpu);
	void instr_err (CPU *cpu);
	void instr_JMP (CPU *cpu);
	void instr_JSR (CPU *cpu);
	void instr_LD  (CPU *cpu);
	void instr_LDI (CPU *cpu);
	void instr_LDR (CPU *cpu);
	void instr_LEA (CPU *cpu);
	void instr_NOT (CPU *cpu);
	void instr_RTI (CPU *cpu);
	void instr_ST  (CPU *cpu);
	void instr_STI (CPU *cpu);
	void instr_STR (CPU *cpu);
	void instr_TRAP(CPU *cpu);
	int  readchar();  // For GETC, IN

	/* *** STUB *** You may need other functions; depends on how you organize your code. */

// -------------------- MAIN PROGRAM --------------------
//
int main() {
	// Declare and initialize the CPU
	//
	CPU cpu;
	init_CPU(&cpu);

	/* *** STUB *** */
}

// -------------------- CPU ROUTINES --------------------
//
//
void init_CPU(CPU *cpu) {
	cpu->pgm_counter = 0;
	cpu->instr_reg = 0;
	cpu->condition_code = 0 /*** STUB ***/;  // Z condition code on power-up
	cpu->running = 1;               // CPU is running

	int regNbr = 0;
	while (regNbr < NBR_REGS) {
		set_register(cpu, regNbr++, 0);
	}

	Address addr = 0;
	do {
		cpu->memory[addr++] = 0;
	} while (addr > 0);
}

// -------------------- MEMORY ROUTINES --------------------
//

// get_memory(cpu, addr) returns the Word at the cpu's memory address
//
Word get_memory(CPU *cpu, Address addr) {
	return cpu->memory[addr];
}

// set_memory(cpu, addr, value) copies the value to the cpu's memory address
//
void set_memory(CPU *cpu, Address addr, Word value) {
	cpu->memory[addr] = value;
}


// Fetch instruction:
//   Copy instruction pointed to by program counter to instruction register
//   Increment program counter (wraparound addresses).
//
void fetch_instr(CPU *cpu) {
	cpu->instr_reg = cpu->memory[cpu->pgm_counter];
	cpu->pgm_counter = (cpu->pgm_counter + 1) % MEMLEN;
}

// Execute an instruction cycle
//   Fetch an instruction
//   Decode the instruction opcode
//   Execute the instruction
//
void instruction_cycle(CPU *cpu) {
	// Get current instruction to execute and its location,
	// echo them out.
	//
	Address old_pc = cpu->pgm_counter;
	fetch_instr(cpu);
	printf("x%04hX: x%04hX  ", old_pc, cpu->instr_reg);

	// Execute instruction; set halt = 1 if execution is to stop
	// (TRAP HALT or internal error).
	//
	switch (op(cpu->instr_reg)) {
	case  0: instr_BR(cpu);   break;
	/* *** STUB *** */

	default:
		printf("Bad opcode: %d; quitting\n", op(cpu->instr_reg));
		cpu->running = 0;
	}
}

// Execute branch instruction: Bitwise AND instruction's mask and
// cpu's condition code, branch iff result is nonzero.
//
// Echo kind of branch, current cc, whether or not branch happened,
// and if so, the target of the branch.
//
void instr_BR(CPU *cpu) {
	printf("BR instruction; *** STUB ***\n");
}

/* *** STUB *** */


// Read and return a character from standard input.  User must
// enter return after the char.  Just pressing return causes \n
// to be returned.  Any extra characters after the first are ignored.
//
int read_char() {
	char buffer[3] = "";
	fgets(buffer, sizeof buffer, stdin);
	return buffer[0];
}

/* *** STUB *** */
