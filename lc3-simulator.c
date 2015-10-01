// Ivaness Christle Chiong, 2014
// Project: LC-3 Simulator Project
//

#include <stdio.h>
#include <stdlib.h>	
#include <string.h>	//for error exit


// CPU Declarations -- a CPU is a structure with fields
// for the different parts of the CPU.

	typedef short int Word;		// A word of LC-3 memory
	typedef unsigned short int Address; 	// An LC-3 Address

	#define MEMLEN 65536
	#define NREG 8
	#define MAX_STRING_LEN 80

	typedef struct 
	{
		Word mem[MEMLEN];
		Word reg[NREG];
		Address pc;
		int cc;
		int running;
		Word ir;
		int pgm_len;
		int ORIG;
		Word set_mem[MEMLEN];
		int mem_trigger;
	} CPU;

	// Prototypes [note the functions are also declared in this order]
	//
		int atoi(const char *str);
		int main(int argc, char *argv[]);
		void initialize_control_unit(int argc, char *argv[], CPU *cpu);
		void initialize_memory(int argc, char *argv[], CPU *cpu);
		FILE *get_datafile(int argc, char *argv[]);

		void dump_control_unit(CPU *cpu);
		void dump_memory(CPU *cpu);
		void dump_registers(CPU *cpu);
		void help_message(void);
		int read_execute_command(CPU *cpu);
		int execute_command(char cmd_char, CPU *cpu);
		void many_instruction_cycles(int nbr_cycles, CPU *cpu);
		void one_instruction_cycle(CPU *cpu);
		
		void exec_HLT(CPU *cpu);
		void exec_TRAP(CPU *cpu, int trapcode);


	// Main program: Initialize the cpu, read in a program, and
	// execute it.
	//
	int main (int argc, char *argv[]) {
		printf("Ivaness Christle Chiong, 2014\n");
		printf("LC-3 Simulator \n");
		CPU cpu_value;
		CPU *cpu = &cpu_value;
		initialize_memory(argc, argv, cpu);
		initialize_control_unit(argc, argv, cpu);
		dump_memory(cpu);
		
		char *prompt = "> ";
		printf("Beginning execution; type h for help\n%s", prompt);


		int done = read_execute_command(cpu);
		while (!done) {
			printf("%s", prompt);
			done = read_execute_command(cpu);
		}
		return 0;
	}

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.hex.  If file opening fails, complain
// and terminate program execution with an error.
//
FILE *get_datafile(int argc, char *argv[]) {
	char *default_datafile_name = "default.hex";
	char *datafile_name;

	if(argc == 1) {
		datafile_name = "program.hex";
	} else if(argc > 1) {
		datafile_name = argv[1];
	}

	FILE *datafile = fopen(datafile_name, "r");

	if(datafile == NULL) {
	    printf("\nCouldn't load %s \n\n", datafile_name); // error message
	    exit(EXIT_FAILURE);
  	}
	
	return datafile;
}

// Read and dump initial values for memory
//
void initialize_memory(int argc, char *argv[], CPU *cpu) {
	FILE *datafile = get_datafile(argc, argv);

	// Will read the next line (words_read = 1 if it started
	// with a valid memory value). Will set memory location loc to
	// value_read
	//
	int value_read, words_read, loc = 0, done = 0;

	// Each getline automatically reallocates buffer and
	// updates buffer_len so that we can read in the whole line
	// of input. bytes_read is 0 at end-of-file.  Note we must
	// free the buffer once we're done with this file.
	//

	char *buffer = NULL;
	size_t buffer_len = 0, bytes_read = 0;
	Word temp[MEMLEN];
	// Read in first and succeeding memory values. 
	// Stop when we're done reading all of the values, or
	// fill up memory, or hit end-of-file.
	//

	bytes_read = getline(&buffer, &buffer_len, datafile);
	while (bytes_read != -1 && !done) {
		
		words_read = sscanf(buffer, "%X", &value_read);

		 if(loc > MEMLEN) {
			 	done = 1;
			 } 

			 if(words_read == 1) {
			 	temp[loc++] = value_read;
			 }

			 if(value_read > 65536 && value_read < -65536) {
			 	done = 1;
			 }

	
		// Set memory value at current location to
		// value_read and increment location.  Exceptions: If
		// loc is out of range, complain and quit the loop. If
		// value_read is outside -65536...65536, 
		// we should say so and quit the loop.
		
		// Get next line and continue the loop
		//
		bytes_read = getline(&buffer, &buffer_len, datafile);
	}
	free(buffer);  // return buffer to OS

	cpu->pgm_len = loc;					// save actual length of the program or number of values read
	cpu->ORIG = (Address) temp[0];		// set cpu->ORIG with the first value from fileread

	int i;
	int j = 1;
	
	if(temp[0] == -1) {
		j = 2;
		cpu->mem[cpu->ORIG] = temp[1];
		for(i=0; i<loc-2; i++)
			cpu->mem[i] = temp[j++];
		
	} else {
		for(i=cpu->ORIG; i<MEMLEN; i++)
			cpu->mem[i] = temp[j++];
	} 
			
}

// Initialize the control registers (pc, ir, condition code, 
// running flag) and the proper registers
//
void initialize_control_unit(int argc, char *argv[], CPU *cpu) {

	char *default_datafile_name = "default.hex";
	char *datafile_name;

	if(argc == 1) {
		 datafile_name = "program.hex";
		 printf("Loading %s\n", datafile_name);
	} else if(argc > 1) {
		 datafile_name = argv[1];
		 printf("Loading %s\n", datafile_name);
	}

	// Initializing PC, IR, CC, and Running flag
	// with proper values.
	
	cpu->pc = cpu->ORIG;	
	cpu->ir = (Address) 0;	
	cpu->running = 1;		
	cpu->cc = 0x5a;						// On boot up, initialize CC = Z
	
	cpu->mem_trigger = 0; 				// if there is a memory set that occurred

	int i = 0;

	for(i=0;i<NREG;i++) 
		cpu -> reg[i]= (Address) 0;		// initializing registers
	
	dump_control_unit(cpu);
	printf("\n");
}

// dump_control_unit(CPU *cpu): Print out the control and
// general-purpose registers
// 
void dump_control_unit(CPU *cpu) {
	printf("\nCONTROL UNIT:");
	printf("\nPC = x%04X\tIR = x%04X\tCC = %c\t   RUNNING = %d\n", 
						cpu->pc, (Address) cpu->ir, cpu->cc, cpu->running);
	dump_registers(cpu);
}

// dump_memory(CPU *cpu): Print memory values and 
// its appropriate memory addresses
//
void dump_memory(CPU *cpu) {
	int i, j;
	int len = cpu->pgm_len-1;
	Word tmp_memArr[len];
	Word tmp_instr[len];

	printf("MEMORY (addresses x0000 - xFFFF):\n");

	if (cpu->ORIG == 0xFFFF) {

		len = cpu->pgm_len-2;
		for (i = 0; i < len; i++) 
      		printf("x%04X: x%04X\t%d\n", i, (Address) cpu->mem[i], cpu->mem[i]);
      	if(cpu->mem_trigger != 0) {							// if memory trigger is triggered or true (mem_trigger = 1),
      		for (i = 0; i < cpu->mem_trigger; i++) 			// then print set memory location & values
      			printf("x%04X: x%04X\t%d\n", (Address) cpu->set_mem[i], 
      				(Address) cpu->mem[(Address) cpu->set_mem[i]], 
      				cpu->mem[(Address) cpu->set_mem[i]]);
      	}
      	printf("x%04X: x%04X\t%d\n", cpu->ORIG, (Address) cpu->mem[cpu->ORIG], 	// print value in xFFFF as the last 
      		cpu->mem[cpu->ORIG]);												// if cpu->ORIG is xFFFF

	} else {
		int ln = cpu->pgm_len-2;
		if (cpu->mem[cpu->ORIG+ln] == 0) {					// checks if last value read from file is x0000
			
			int k = cpu->pgm_len-2;
			for (i = cpu->ORIG; i < cpu->ORIG+k; i++) 
     			printf("x%04X: x%04X\t%d\n", i, (Address) cpu->mem[i], cpu->mem[i]);

		} else {

			int k = cpu->pgm_len-3;
			for (i = cpu->ORIG; i < cpu->ORIG+len; i++) 
     			printf("x%04X: x%04X\t%d\n", i, (Address) cpu->mem[i], cpu->mem[i]);

     	}

   	    if(cpu->mem_trigger != 0) {							// if memory trigger is triggered or true (mem_trigger = 1),
      		for (i = 0; i < cpu->mem_trigger; i++) 			// then print newly set memory location & values
      			printf("x%04X: x%04X\t%d\n", (Address) cpu->set_mem[i], 
      				(Address) cpu->mem[(Address) cpu->set_mem[i]], 
      				cpu->mem[(Address) cpu->set_mem[i]]);
  		}

    }

	printf("\n");
}

// dump_registers(CPU *cpu): Print register values in two rows of
// five.
//
void dump_registers(CPU *cpu) {
	int num_rows = 2;
	int num_cols = 4;
	int box[num_rows][num_cols];
	int row, col;
	int pos = 0;

        for(row = 0; row < num_rows; row++) {
            for(col = 0; col< num_cols; col++)
                box[row][col] = cpu->reg[pos++];
	}
        pos = 0;
        for(row = 0; row < num_rows; row++) {
            for(col=0; col<num_cols; col++) {
                printf("R%d x%04X  %d\t     ", pos++, 
                	(Address) box[row][col], (Word) box[row][col]);
            }
            printf("\n");
        }
}

// Read a simulator command from the keyboard ("h", "?", "d", number,
// or empty line) and execute it.  Return true if we hit end-of-input
// or execute_command told us to quit.  Otherwise return false.
//
int read_execute_command(CPU *cpu) {
	// Buffer for the command line from the keyboard, plus its size
	//
	char *cmd_buffer = NULL;
	size_t cmd_buffer_len = 0, bytes_read = 0;

	// Values read using sscanf of command line
	//
	int nbr_cycles;
	char cmd_char;
	char cmd_str[MAX_STRING_LEN];
	int nbytes = 100;
	size_t words_read;	// number of items read by sscanf call

	int done = 0;	// Should simulator stop?

	bytes_read = getline(&cmd_buffer, &cmd_buffer_len, stdin);
	if (bytes_read == -1) {
		done = 1;   // Hit end of file
	}
	
	words_read = sscanf(cmd_buffer, "%d", &nbr_cycles);
    if (words_read == 1)
    	many_instruction_cycles(nbr_cycles, cpu);
   	else{
    	int c_read = sscanf(cmd_buffer, "%c", &cmd_char);
    	execute_command(cmd_char, cpu);

   	}

    strncpy(cmd_str, cmd_buffer, sizeof(cmd_buffer) + 6);	// copy cmd_buffer and store to cmd_str
  	
    if(cmd_str[0] == 106) {			// if cmd_buffer starts with 'j', 
    								// this signals jump and execute true arm

  		char arr[4];
    	if(cmd_str[2] != 120) {		// if input is not in proper format
    		printf("Jump command should be j address (in xNNNN format)\n");
    	} else {
	    	printf("Jumped to ");
	  
	    	int x = 3;
	    	int y;
	    	for(y=0; y<4; y++)
	    		arr[y] = cmd_str[x++];

	    	int jump_loc;
			sscanf(arr, "%X", &jump_loc);  		// scan jump location
	    	cpu->pc = (Address) jump_loc;		// update PC to new jump location

	    	printf("x%04X\n", (Address) jump_loc);
	    }

    } else if (cmd_str[0] == 109) {				// if cmd_buffer starts with 'm', 
    											// this signals memory set location w/ new instructon
    	
    	char tmp_arr[50];
    	char arr2[4];
    	char arr3[4];

    	int m, n = 3;
    	for(m=0; m<sizeof(cmd_buffer)+5; m++)
    		tmp_arr[m] = cmd_str[n++];

   		int tr = 0;
   		while (tmp_arr[tr] != 32) {
   			tr++;
   		}

    	if(cmd_str[2] != 120) {			// if input is not in proper format
    		printf("Memory command should be m addr value (in xNNNN xMMMM format)\n");
    	} else if ( tmp_arr[tr+1] != 120 ) {
    		printf("Memory command should be m addr value (in xNNNN xMMMM format)\n");
    	} else {
	    	int y, x = 3;
	    	for(y=0; y<4; y++)
	    		arr2[y] = cmd_str[x++];
	    	int mem_loc;
	    	sscanf(arr2, "%X", &mem_loc);  	
	    	
	   		int z;
	   		int st = tr+2;
	   		for(z=0; z<4; z++)
	    		arr3[z] = tmp_arr[st++];
	    	int instr;
	    	sscanf(arr3, "%X", &instr);

	    	cpu->mem[mem_loc] = instr;					// set mem[xNNNN] = xMMMM
	    	cpu->set_mem[cpu->mem_trigger] = mem_loc;	// save to temporary memory array
	    	cpu->mem_trigger++;							// update trigger (trigger != 0 when a memory set is being done)
	 
	 		printf("Set mem[");
	    	printf("x%04X", (Address) mem_loc);
	    	printf("] = ");
	    	printf("x%04X\n", (Address) instr);

	    }

    } else if (cmd_str[0] == 114) {	
    	

    	if(cmd_str[5] != 120) {			// if input is not in proper format
    		printf("Register command should be r rN value (in xNNNN format)\n");
    	} else if(cmd_str[2] != 114) {
    		printf("Register command should be r rN value (in xNNNN format)\n");
    	} else {

	    	char arr4[4];
	    	char arr5[2];

	    	int i;
	    	for(i=0; i<2; i++)
	    		arr5[i] = 0;
	    	arr5[0] = cmd_str[3];

	    	int f = 6;
	    	int g;
	    	for(g=0; g<4; g++)
	    		arr4[g] = cmd_str[f++];

	    	int regn;
	    	int reg_val;
			sscanf(arr4, "%X", &reg_val);  			// register value
			sscanf(arr5, "%d", &regn); 				// scan register #
			
	    	printf("Set r%d = x%04X\n", regn, (Address) reg_val);

	    	cpu->reg[regn] = (Address) reg_val;		// do a register set
		}

    }

   	free(cmd_buffer);
   	return done;
}

// Execute a nonnumeric command; complain if it's not 'h', '?', 'd', 'q' or '\n'
// Return true for the q command, false otherwise
//
int execute_command(char cmd_char, CPU *cpu) {

	if (cmd_char == '?' || cmd_char == 'h') {	// if 'h' or '?', display help message
		help_message();
		
	} else if (cmd_char == 'q') {				// if 'q', exit program
        printf("Quitting...\n\n");
        exit(EXIT_FAILURE);
	} else if (cmd_char == '\n') {				// if '\n', execute 1 instruction cycle
        one_instruction_cycle(cpu);
        
	} else if (cmd_char == 'd') {				// if 'd', dump control unit & memory
        dump_control_unit(cpu);
        printf("\n");
        dump_memory(cpu);
	} 
	return 0;
}

// Print standard message for simulator help command ('h' or '?')
//
void help_message(void) {
	printf("Simulator commands:\n");
	printf("h or ? for help (prints this message)\n");
	printf("q to quit\n");
	printf("d to dump the control unit and memory\n");
	printf("r rN xNNNN to set register with value or location\n");
	printf("j xNNNN to jump to new location\n");
	printf("m xNNNN xMMMM to assign memory location xNNNN = value xMMMM\n");
	printf("An integer > 0 to execute that many instruction cycles\n");
	printf("Or just return, which executes one instruction cycle\n\n");

}

// Execute a number of instruction cycles.  Exceptions: If the
// number of cycles is <= 0, complain and return; if the CPU is
// not running, say so and return; if the number of cycles is
// insanely large, complain and substitute a saner limit.
//
// If, as we execute the many cycles, the CPU stops running,
// then return.
//
void many_instruction_cycles(int nbr_cycles, CPU *cpu) {
	if(nbr_cycles <= 0) {						// check if nbr_cycles is < or = 0
		printf("Cannot be executed.\n");
		
	} else if(nbr_cycles >= 100) {				// check if nbr_cycles is too big
		printf("Not applicable.\n");
		nbr_cycles = 1;
	
	} else {									// else, 1 instruction cycle many times
		int p = 0;
		while (p < nbr_cycles) {
	    	one_instruction_cycle(cpu);
	      	p++;
		}
	}

	if(cpu->running == 0) {						// check if CPU isn't running
		printf("CPU isn't running.\n");
		
	} 
}

// Execute one instruction cycle
//
void one_instruction_cycle(CPU *cpu) {
	// If the CPU isn't running, say so and return.
	// If the pc is out of range, complain and stop running the CPU.
	//
	int done = 0;

	if(cpu->running == 0){						// check if cpu isn't running
		printf("Halted\n");
		return;
	}

	if (cpu->pc > MEMLEN) {						// check if PC is out of range
		printf("PC out of range.\n");
		exit(EXIT_FAILURE);
	}

	// Get instruction and increment pc
	//
	int instr_loc = cpu -> pc;
    cpu->ir = (Address) cpu->mem[cpu -> pc++];

    // Extract opcode 
    //
	int opcode = (Address) cpu->ir & 0xF000;

	// Echo pc & instruction
	//
	printf("x%04X: x%04X", instr_loc, (Address) cpu -> ir);
	

	switch (opcode) {		// test instructions and implement
	case 0x0000: 			// check if BR or NOP
	{
		Address mask = (Address) cpu->ir & 0xE00;
		if (mask == 0x800) {										// check if BRN
			cpu->cc = 78;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRN if mask < 0, CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);

		} else if (mask == 0x400) {									// check if BRZ 
			cpu->cc = 90;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRZ if mask = 0; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);			
		} else if (mask == 0x200) {									// check if BRP 
			cpu->cc = 80;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRP if mask > 0; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);
		} else if (mask == 0xC00) {									// check if BRNZ 
			cpu->cc = 90;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRNZ if mask =< 0; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);
		} else if (mask == 0xA00) {									// check if BRNP 
			cpu->cc = 80;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRNP if mask != 0; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);
		} else if (mask == 0x600) {									// check if BRZP
			cpu->cc = 80;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BRZP if mask >= 0; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);
		} else if (mask == 0xE00) {									// check if BR / BRNZP = UNCONDITIONAL
			cpu->cc = 90;
			Address offset = (Address) cpu->ir & 0x1FF; 
			printf("  BR or BRNZP Unconditional; CC = %c\n", cpu->cc);
			cpu->pc = cpu->pc + offset;
			printf("Jumped to %04X \n", cpu->pc);
		} else {													// otherwise, NOP; no go to 
			cpu->cc = 90;
			printf("  NOP  %X, CC = %c, no go to\n", 
				cpu->mem[instr_loc], cpu->cc); 
		}

	} break;
	case 0x1000:		// ADD INSTR
	{
		Address dst = ((Address) cpu->ir & 0xE00) >> 9;
		Address flag = (Address) cpu->ir & 0x20;
		Address src1 = ((Address) cpu->ir & 0x1C0) >> 6;
		Address src2 = (Address) cpu->ir & 0x7;
		Address imm5 = (Address) cpu->ir & 0x1F;	

		if (flag != 0x20) {		// if flag is 0
			cpu->cc = 90;
			printf(" ADD R%X, R%X, R%X; R%X <- R%X + R%X; CC = %c\n",
				dst, src1, src2, dst, src1, src2, cpu->cc);
			cpu->reg[dst] = cpu->reg[src1] + cpu->reg[src2]; 
		} else { 
			cpu->cc = 80;
			printf(" ADD R%X, R%X, %X; R%X <- R%X + %X; CC = %c\n",
				dst, src1, imm5, dst, src1, imm5, cpu->cc);
			cpu->reg[dst] = cpu->reg[src1] + imm5; 
		}

	} break;
	case 0x2000:		// LD INSTR
	{
		Address dest = ((Address) cpu->ir & 0x0E00) >> 9;
		Address offset = (Address) cpu->ir & 0x01FF;
		Address d_reg = dest >> 8;
		printf(" LD R%X, %X;  R%X <- PC+%X = x%04X; CC = %c\n",
			d_reg, offset, d_reg, offset, cpu->pc+offset, cpu->cc);
		cpu->reg[d_reg] = cpu->pc+offset;

	} break;
	case 0x3000:		// ST INSTR
	{
		Address src = ((Address) cpu->ir & 0x0E00) >> 9;
		Address offset = (Address) cpu->ir & 0x01FF;
		Address src_reg = src >> 8;
		printf(" ST R%X, %X;  MEM[x%04X] <- R%X = x%04X; CC = %c\n",
			src_reg, offset, cpu->pc+offset, src_reg, (Address) cpu->reg[src_reg], 
			cpu->cc);
		cpu->mem[cpu->pc+offset] = cpu->reg[src_reg];

	} break;
	case 0x4000:		// JSR & JSRR INSTR
	{
		cpu->reg[7] = cpu->pc;
		Address flag = (Address) cpu->ir & 0x800;
		Address offset11 = (Address) cpu->ir & 0x7FF;
		Address base = ((Address) cpu->ir & 0x1C0) >> 6;

		if (flag == 0x800) {		// if flag is 1, then execute JSR; else, execute JSRR
			printf(" JSR %X; PC <- PC+%X; CC = %c\n",
				offset11, offset11, cpu->cc);
				cpu->pc = cpu->pc + offset11;
		} else {					
			printf(" JSRR R%X; PC <- R%X; CC = %c\n",
				base, base, cpu->cc);
			if ((Address) cpu->pc == cpu->reg[base]) {		// check if pc = base register
				printf(" INFINITE LOOP, exiting immediately\n");		
				exit(EXIT_FAILURE);							// choose to quit to prevent system crash
			} else {													
				cpu->pc = cpu->reg[base];
			}
		}

	} break;
	case 0x5000:		// AND INSTR
	{
		Address dst = ((Address) cpu->ir & 0xE00) >> 9;
		Address flag = (Address) cpu->ir & 0x20;
		Address src1 = ((Address) cpu->ir & 0x1C0) >> 6;
		Address src2 = (Address) cpu->ir & 0x7;
		Address imm5 = (Address) cpu->ir & 0x1F;

		if (flag != 0x20) {		// if flag is 0, do not consider immediate5
			cpu->cc = 90;
			printf(" AND R%X, R%X, R%X; R%X <- R%X AND R%X; CC = %c\n",
				dst, src1, src2, dst, src1, src2, cpu->cc);
			cpu->reg[dst] = cpu->reg[src1] * cpu->reg[src2]; 
		} else { 
			cpu->pc = 80;
			printf(" AND R%X, R%X, %X; R%X <- R%X AND %X; CC = %c\n",
				dst, src1, imm5, dst, src1, imm5, cpu->cc);
			cpu->reg[dst] = cpu->reg[src1] * imm5; 
		}

	} break;
	case 0x6000:		// LDR INSTR
	{ 
		Address dest = ((Address) cpu->ir & 0xE00) >> 9;
		Address base = ((Address) cpu->ir & 0x1C0) >> 6;
		Address offset6 = (Address) cpu->ir & 0x3F;
		cpu->cc = 80;
		printf(" LDR R%X, R%X, %X; R%X <- M[x%04X+%X] = M[x%04X] = x%04X; CC = %c\n",
				dest, base, offset6, dest, cpu->reg[base], offset6, base+offset6, 
				cpu->mem[base+offset6], cpu->cc); 

	} break;
	case 0x7000:		// STR INSTR
	{ 
		Address src = ((Address) cpu->ir & 0xE00) >> 9;
		Address base = ((Address) cpu->ir & 0x1C0) >> 6;
		Address offset6 = (Address) cpu->ir & 0x3F;
		cpu->cc = 80;
		printf(" STR R%X, R%X, %X; M[x%04X+%X] = M[x%04X] <- R%X = x%04X; CC = %c\n",
				src, base, offset6, (Address) cpu->reg[base], offset6, (Address) base+offset6, src,
				(Address) cpu->reg[src], cpu->cc); 

	} break;
	case 0x8000:		// RTI INSTR
	{
		cpu->cc = 80;
		printf(" Unsupported command, halts CPU; CC = %c\n", cpu->cc);
		cpu->running = 0;

	} break;
	case 0xA000:		// LDI INSTR
	{ 
		Address dest = ((Address) cpu->ir & 0x0E00) >> 9;
		Address offset = (Address) cpu->ir & 0x01FF;
		Address d_reg = dest >> 8;
		cpu->cc = 80;
		printf(" LDI R%X, %X;  R%X <- M[M[PC+%X]] = M[M[x%04X]] = M[x%04X] ; CC = %c\n",
			d_reg, offset, d_reg, (Address) offset, cpu->pc+offset, 
			cpu->mem[cpu->pc+offset], (Address) cpu->mem[cpu->mem[cpu->pc+offset]], 
			cpu->cc);
		cpu->mem[cpu->pc+offset];
		cpu->mem[cpu->mem[cpu->pc+offset]];
		cpu->reg[d_reg] = cpu->mem[cpu->mem[cpu->pc+offset]];

	} break;
	case 0x9000:		// NOT INSTR
	{ 
		Address dst = ((Address) cpu->ir & 0xE00) >> 9;
		Address src = ((Address) cpu->ir & 0x1C0) >> 6;
		Address not = 0x3F;
		cpu->pc = 78;
		printf(" NOT R%X, R%X; R%X <- NOT R%X = %d; CC = %c\n", 
			dst, src, dst, src, cpu->reg[src], cpu->cc);
		cpu->reg[dst] = (Address) ~(cpu->reg[src]) + not;

	} break;
	case 0xB000:		// STI INSTR
	{
		
		Address src = ((Address) cpu->ir & 0x0E00) >> 9;
		Address offset = (Address) cpu->ir & 0x01FF;
		Address s_reg = src >> 8;
		cpu->cc = 80;
		printf(" STI R%X, %X;  x%04X <- R%X = %X; CC = %c\n",
			s_reg, offset, cpu->pc+offset, s_reg, offset, cpu->cc);
		cpu->reg[s_reg] = cpu->pc+offset;

	} break;
	case 0xC000:		// JMP INSTR
	{
		
		Address base = (Address) cpu->ir & 0x1C0 >> 6;
		Address offset = (Address) cpu->ir & 0x3F;
		cpu->cc = 80;
		cpu->pc = cpu->reg[base];
		printf(" JMP R%X; PC <- R%X; CC = %c\n",
			base, base, cpu->cc);

	} break;
	case 0xD000:		// err INSTR
	{ 
		cpu->cc = 80;
		printf(" (Unused opcode) Halting CPU; CC = %c\n", cpu->cc);
		cpu->running = 0;

	} break;
	case 0xE000:		// LEA INSTR
	{ 
		Address dest = (Address) cpu->ir & 0x0F00;
		Address offset = (Address) cpu->ir & 0x00FF;
		Address d_reg = dest >> 8;
		cpu->cc = 80;
		printf(" LEA R%X, %X;  R%X <- PC+%X = x%04X; CC = %c\n",
			d_reg, offset, d_reg, offset, cpu->pc+offset, cpu->cc);
		cpu->reg[d_reg] = cpu->pc+offset;

	} break;
	case 0xF000: 		// TRAP INSTRS
	{
		int trap = (Address) cpu->ir & 0x00FF;
		switch (trap) {
			case 0x0020: 		// TRAP x20 GETC
			{
				printf(" TRAP x20 (GETC): ");
				char char_arr[10];
				fgets(char_arr, 10, stdin);
				cpu->reg[0] = char_arr[0];
				cpu->cc = 78;
				cpu->reg[7] = cpu->pc;
				printf("Read = %c; CC = %c \n", 
					cpu->reg[0], cpu->cc);
			} break;
			case 0x0021: 		// TRAP x21 OUT 
			{
				printf(" TRAP x21 (OUT): %d = %c; CC = N\n", 
					cpu->reg[0], cpu->reg[0]);
			} break;
			case 0x0022: 		// TRAP x22 PUTS
			{
				printf(" TRAP x22 (PUTS): ");
				cpu->cc = 80;
				cpu->reg[7] = cpu->pc;
				Address temp = cpu->reg[0];
				while(cpu->mem[temp] != 0) {
					printf("%c", cpu->mem[temp]);
					temp++;
				}
				printf("\nCC = %c\n", cpu->cc);

			} break;
			case 0x0023: 		// TRAP x23 IN
			{
				printf(" TRAP x21 (IN) Input a character> ");
				char char_arr[10];
				fgets(char_arr, 10, stdin);
				cpu->reg[0] = char_arr[0];
				cpu->cc = 78;
				cpu->reg[7] = cpu->pc;
				printf("Read = %c; CC = %c \n", 
					cpu->reg[0], cpu->cc);
			} break;
			case 0x0025: 		// TRAP x25 HALT
			{
				cpu->cc = 80;
				printf(" TRAP x25 Halting; CC = %c\n", cpu->cc);
				cpu->running = 0;
			} break;
			default: printf("  (instruction not yet supported)\n"); break;
		}
	} break;
	
    default: printf("  (instruction not yet supported)\n"); break;
   }

}


