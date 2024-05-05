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

int address = 0;
char *labelTable[1000];
int labelAddress[1000];
int labelCount = 0;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int encodeRType(char *, char *, char *, char *);
int encodeIType(char *, char *, char *, char *);
int encodeJType(char *, char *, char *, char *);
int encodeOType(char *, char *, char *, char *);

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

	
	/* TODO: Phase-1 label calculation */
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

	address = 0;
	while(1){
		label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
		if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
			break;
		}
		
		int instruction = 0;

		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")){
			instruction = encodeRType(opcode, arg0, arg1, arg2);
		} else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")){
			instruction = encodeIType(opcode, arg0, arg1, arg2);
		} else if (!strcmp(opcode, "jalr")){
			instruction = encodeJType(opcode, arg0, arg1, arg2);
		} else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")){
			instruction = encodeOType(opcode, arg0, arg1, arg2);
		} else if (!strcmp(opcode, ".fill")){
			if(isNumber(arg0)){
				instruction = atoi(arg0);
			} else {
				for(int i = 0; i < labelCount; i++){
					if(!strcmp(arg0, labelTable[i])){
						instruction = labelAddress[i];
						break;
					}
				}
			}
		} else {
			printf("error: unrecognized opcode %s\n", opcode);
			exit(1);
		}
		fprintf(outFilePtr, "%d\n", instruction);
		address++;
	}

	/* when done, close the files */

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	
	exit(0);
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

int encodeRType(char *opcode, char *arg0, char *arg1, char *arg2)
{
	/* opcode is add, nor */
	int instruction = 0;
	int regA = atoi(arg0);
	int regB = atoi(arg1);
	int destReg = atoi(arg2);

	// register integer check
	if (!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)){
		printf("error: non-integer register arguments\n");
		exit(1);
	}

	// register range check
	if(regA < 0 || regA > 7 || regB < 0 || regB > 7 || destReg < 0 || destReg > 7){
		printf("error: register out of range\n");
		exit(1);
	}

	if (!strcmp(opcode, "add")) {
		instruction = (ADD << 22) | (regA << 19) | (regB << 16) | destReg;
	} else if (!strcmp(opcode, "nor")) {
		instruction = (NOR << 22) | (regA << 19) | (regB << 16) | destReg;
	} else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	return instruction;
}

int encodeIType(char *opcode, char *arg0, char *arg1, char *arg2)
{
	/* opcode is lw, sw, beq */
	int instruction = 0;
	int regA = atoi(arg0);
	int regB = atoi(arg1);

	// register integer check
	if (!isNumber(arg0) || !isNumber(arg1)){
		printf("error: non-integer register arguments\n");
		exit(1);
	}

	// register range check
	if(regA < 0 || regA > 7 || regB < 0 || regB > 7){
		printf("error: register out of range\n");
		exit(1);
	}
	
	char* offsetLabel = arg2;
	int offset = -32769;
	
	if(isNumber(offsetLabel)){
		offset = atoi(offsetLabel);
	} else {
		// find label in label table
		for(int i = 0; i < labelCount; i++){
			if(!strcmp(offsetLabel, labelTable[i])){
				offset = labelAddress[i];
				break;
			}
		}

		if(offset == -32769){
			printf("error: use of undefined labels\n");
			exit(1);
		}

		if(offset < -32768 || offset > 32767){
			printf("error: offsetfield does not fit in 16 bits\n");
			exit(1);
		}

		if(!strcmp(opcode, "beq")){
			offset = offset - (address + 1);
		}

		offset = offset & 0xFFFF;
	}

	if (!strcmp(opcode, "lw")) {
		instruction = (LW << 22) | (regA << 19) | (regB << 16) | offset;
	} else if (!strcmp(opcode, "sw")) {
		instruction = (SW << 22) | (regA << 19) | (regB << 16) | offset;
	} else if (!strcmp(opcode, "beq")) {
		instruction = (BEQ << 22) | (regA << 19) | (regB << 16) | offset;
	} else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	return instruction;
}

int encodeJType(char *opcode, char *arg0, char *arg1, char *arg2)
{
	/* opcode is jalr */
	int instruction = 0;
	int regA = atoi(arg0);
	int regB = atoi(arg1);

	// register integer check
	if (!isNumber(arg0) || !isNumber(arg1)){
		printf("error: non-integer register arguments\n");
		exit(1);
	}

	// register range check
	if(regA < 0 || regA > 7 || regB < 0 || regB > 7){
		printf("error: register out of range\n");
		exit(1);
	}

	if (!strcmp(opcode, "jalr")) {
		instruction = (JALR << 22) | (regA << 19) | (regB << 16);
	} else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	return instruction;
}

int encodeOType(char *opcode, char *arg0, char *arg1, char *arg2)
{
	/* opcode is halt, noop */
	int instruction = 0;

	if (!strcmp(opcode, "halt")) {
		instruction = (HALT << 22);
	} else if (!strcmp(opcode, "noop")) {
		instruction = (NOOP << 22);
	} else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	return instruction;
}