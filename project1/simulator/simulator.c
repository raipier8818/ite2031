/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

// 24-22 bits
int getOpcode(int instruction) {
    return (instruction >> 22) & 0x7;
}

// 21-19 bits
int getRegA(int instruction) {
    return (instruction >> 19) & 0x7;
}

// 18-16 bits
int getRegB(int instruction) {
    return (instruction >> 16) & 0x7;
}

// 0-2 bits
int getDestReg(int instruction) {
    return (instruction & 0x7);
}

// 0-15 bits
int getOffsetField(int instruction) {
    return (instruction & 0xFFFF);
}

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    int lineNum = 0;
    int cnt = 0;

    // initialize registers
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }

    // initialize pc
    state.pc = 0;

    while(1) {
        printState(&state);
        int instruction = state.mem[state.pc];
        state.pc++;
        
        int opcode = getOpcode(instruction);
        cnt++;
        
        if (opcode == 0 || opcode == 1){
            // R-type
            int regA = getRegA(instruction);
            int regB = getRegB(instruction);
            int destReg = getDestReg(instruction);

            if (opcode == 0) {
                // add
                state.reg[destReg] = state.reg[regA] + state.reg[regB];
            } else {
                // nor
                state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
            }
        }
        else if(opcode == 2 || opcode == 3 || opcode == 4){
            // I-type
            int regA = getRegA(instruction);
            int regB = getRegB(instruction);
            int offsetField = getOffsetField(instruction);
            // offsetField to 16 bits signed
            offsetField = convertNum(offsetField);

            
            if (opcode == 2) {
                // lw
                if (state.reg[regA] + offsetField < 0 || state.reg[regA] + offsetField >= state.numMemory){
                    printf("Invalid address\n");
                    exit(1);
                }
                state.reg[regB] = state.mem[state.reg[regA] + offsetField];
            } else if (opcode == 3){
                // sw
                if (state.reg[regA] + offsetField < 0 || state.reg[regA] + offsetField >= state.numMemory)
                {
                    printf("Invalid address\n");
                    exit(1);
                }
                state.mem[state.reg[regA] + offsetField] = state.reg[regB];
            }else{
                // beg
                if (state.pc + offsetField < 0 || state.pc + offsetField >= state.numMemory){
                    printf("Invalid address %d\n", state.pc + offsetField);
                    exit(1);
                }

                if (state.reg[regA] == state.reg[regB]) {
                    state.pc += offsetField;
                }
            }
        }else if(opcode == 5){
            // jalr
            int regA = getRegA(instruction);
            int regB = getRegB(instruction);

            if (state.reg[regA] > state.numMemory || state.reg[regA] < 0){
                printf("Invalid address\n");
                exit(1);
            }

            state.reg[regB] = state.pc;
            state.pc = state.reg[regA];
        }
        else if(opcode == 6){
            // halt
            printf("machine halted\n");
            break;
        }
        else if(opcode == 7){
            // noop
        }
        else {
            printf("Invalid opcode\n");
            exit(1);
        }
        lineNum++;
    }

    printf("total of %d instructions executed\n", cnt);
    printf("final state of machine:\n");

    printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
