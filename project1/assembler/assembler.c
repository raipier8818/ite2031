/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

#define ADD  0
#define NOR  1
#define LW   2
#define SW   3
#define BEQ  4
#define JALR 5
#define HALT 6
#define NOOP 7

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	// if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
	// 	/* reached end of file */
	// }

	/* TODO: Phase-1 label calculation */

	int address = 0;
	char* labelTable[1000];
	int labelAddress[1000];
	int labelCount = 0;

	memset(labelTable, 0, sizeof(labelTable));
	memset(labelAddress, 0, sizeof(labelAddress));
	
	while(1) {
		label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
		if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
			break;
		}
		if (label[0] != '\0') {
			labelTable[labelCount] = strdup(label);
			labelAddress[labelCount] = address;
			labelCount++;
		}
		address++;
	}

	rewind(inFilePtr);

	while(1){
		label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
		if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
			break;
		}
		
		int instruction = 0;

		if (!strcmp(opcode, "add")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int destReg = atoi(arg2);

			instruction = (ADD << 22) | (regA << 19) | (regB << 16) | destReg;
		} else if (!strcmp(opcode, "nor")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int destReg = atoi(arg2);

			instruction = (NOR << 22) | (regA << 19) | (regB << 16) | destReg;
		} else if (!strcmp(opcode, "lw")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int offset = atoi(arg2);

			if(offset < -32768 || offset > 32767){
				printf("error: offset out of range\n");
				exit(1);
			}

			instruction = (LW << 22) | (regA << 19) | (regB << 16) | offset;
		} else if (!strcmp(opcode, "sw")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int offset = atoi(arg2);

			if(offset < -32768 || offset > 32767){
				printf("error: offset out of range\n");
				exit(1);
			}

			instruction = (SW << 22) | (regA << 19) | (regB << 16) | offset;
		} else if (!strcmp(opcode, "beq")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int offset = 0;
			for (int i = 0; i < labelCount; i++){
				if (!strcmp(arg2, labelTable[i])){
					offset = labelAddress[i] - address - 1;
					break;
				}
			}

			if (offset < -32768 || offset > 32767){
				printf("error: offset out of range\n");
				exit(1);
			}

			instruction = (BEQ << 22) | (regA << 19) | (regB << 16) | offset;
		} else if (!strcmp(opcode, "jalr")){
			int regA = atoi(arg0);
			int regB = atoi(arg1);

			instruction = (JALR << 22) | (regA << 19) | (regB << 16);
		} else if (!strcmp(opcode, "halt")){
			instruction = (HALT << 22);
		} else if (!strcmp(opcode, "noop")){
			instruction = (NOOP << 22);
		} else {
			printf("error: unrecognized opcode %s\n", opcode);
			exit(1);
		}

		fprintf(outFilePtr, "%d\n", instruction);
	}



	/* when done, close the files */



	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

