/* 
File name: assembler.c
Purpose  : A simple assembler that converts the assembly code written 
           for the simple microprocessor into machine code.

Author   : Alamgir Mohammed  

Created on: July 11, 2014
Last update: June 04, 2017
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// define the opcodes supported
enum OPCODES {
	NOP,	STOP,	DUMP,	LOAD,
	STOR,	XCHG,	JMP,	JMPE,
	ADD,	SUB,	INC,	DEC,
	AND,	OR,	NOT,	XOR,
	CMP,	COMP  // CMP is the same same as NOT, COMP is the same as SUB	
};

//machine codes corresponding to the opcodes above
char *MC_Opcodes[] = {
	"0", "1", "2", "3",
	"4", "5", "6", "7",
	"8", "9", "A", "B",
	"C", "D", "E", "F",
	"E", "9"
};


//types of argument/flags in the opcode
enum ARG_TYPE {
	IGNORE = 0,
	MEMORY = 1,
	DATA = 2,
	REGS  = 3  
};

//machine codes for the argument types flags
char *MC_Argtype [] = {
      "0", "1", "2", "3"	
};



//function prototypes
int OpcodeNametoCode (char *opcodeStr);
void Compile(char *fNameIn, char *fNameOut);
char *trim(char *str);
void mystrupr(char *str);


int main(int argc, char *argv[])
{
	char fNameIn[256], fNameOut[256];
	if(argc == 2) { // an input file name is given
		strcpy(fNameIn, argv[1]);
		strcpy(fNameOut, "stdout");
	} else if(argc == 3) { // both an input and output file name are given
		strcpy(fNameIn, argv[1]);
		strcpy(fNameOut,argv[2]);
	} else {
		printf("\nUsage: assembler inputfile outputfile");
		printf("\nif outputfile is not given, program.mc is assumed.\n");
		exit(1);
	}

	//compile the file	
	Compile(fNameIn, fNameOut);
	
	return 0;
}


void Compile(char *fNameIn, char *fNameOut)
{
	FILE *fpIn, *fpOut; //pointers for input and output files
	char buffer[1024];  //a buffer to hole one line of string
	int  lineNumber; 
	char *token;
	char mCode[10],  mData[5];
	//int  mData;

	lineNumber = 0;
	
	fpIn = fopen(fNameIn, "r+t");
	if(fpIn == NULL){
		printf("\nCant open file :%s", fNameIn);
		exit(0);
	}
	else {
		fpOut = fopen(fNameOut, "w+t");
		if(fpOut == NULL){
			printf("\nCant open file program.dat");
			exit(0);
		} else { 
			//file opening went well
			while(!feof(fpIn)) {
				lineNumber++;

				if(fgets(buffer, 1024, fpIn)){
					//printf(buffer);
					/* get the first token */
					token = strtok(buffer, " ");
					token = trim(token);
					mystrupr(token);
					sprintf(mCode,"%s", MC_Opcodes[OpcodeNametoCode(token)]);

					fprintf(fpOut, "%s", mCode );
					//printf(" -> %s", mCode );
					if(token) { // argument
						 token = strtok(NULL, " ");
						 if(token == NULL) { // no trailing space
						 	 sprintf(mCode,"%s00",MC_Argtype[IGNORE]);
						 }
						 else{ // argument given
						 	 token = trim(token);
							 mystrupr(token);

							 if(!strcmp(token, "")) // possibly a trailing space
							 {
								sprintf(mCode,"%s00",MC_Argtype[IGNORE]);
							 }
							 else if(token[0] == '$'){ // data 
								sprintf(mData, "%s",&token[1]);
								if(strlen(mData)==1)
								   sprintf(mCode,"%s0%s",MC_Argtype[DATA], mData);
								 else
									sprintf(mCode,"%s%s",MC_Argtype[DATA], mData);

							 }
							 else if(token[0] == '[' && token[1] == '$') { // memory address
								token[0] = ' ';token[1] = ' '; 
								if(token[3] == ']') 
									token[3] = ' '; 
								else   token[4]=' ';
							
								token = trim(token);
								sprintf(mData, "%s",token);
								
								if(strlen(mData)==1)
								   	sprintf(mCode,"%s0%s",MC_Argtype[MEMORY], mData);
								else
									sprintf(mCode,"%s%s",MC_Argtype[MEMORY], mData);
							}else if(token[1] == 'X' || token[1] == 'x'){ // handle registers 
								switch(token[0]){
									case 'A':
									case 'a':
											sprintf(mCode,"%s00",MC_Argtype[REGS]); break;
									
									case 'B':
									case 'b':
											sprintf(mCode,"%s01",MC_Argtype[REGS]); break;
											
									case 'R':
									case 'r':
											sprintf(mCode,"%s02",MC_Argtype[REGS]); break;
									
									case 'I':
									case 'i':
											sprintf(mCode,"%s03",MC_Argtype[REGS]); break;	
									default: //error
										;
								}
							}
							else  {
								sprintf(mCode,"%s000",MC_Argtype[IGNORE]);
								printf("\nLine %d: Error in argument type.", lineNumber);
							}
						}// argument given
						
							fprintf(fpOut, "%s\n", mCode);
							//printf("%s\n", mCode);
					}
				}
			}
			fclose(fpOut);
		}
		fclose(fpIn);
	}
}



/* returns the numeric opcode for a given mnemonic string */
int OpcodeNametoCode (char *opcodeStr)
{

switch(opcodeStr[0]) {
	case 'A': switch(opcodeStr[1]) { 
			case 'D': if(opcodeStr[2] == 'D') return ADD; 
					else return -1;
			case 'N': if(opcodeStr[2] == 'D') return AND; 
					else return -1;
			default:
				return -1;
		  }
		 break;
		
	case 'C': switch(opcodeStr[1]) { 
			case 'M': if(opcodeStr[2] == 'P') return CMP; 
					else return -1;
			case 'O': if(opcodeStr[2] == 'M' && opcodeStr[3] == 'P') 
					return COMP; else return -1;
			default:
				return -1;
		  }
		 break;

	case 'D': switch(opcodeStr[1]) { 
			case 'E':if(opcodeStr[2] == 'C') return DEC; 
					else return -1;
			case 'U':if(opcodeStr[2] == 'M' && opcodeStr[3] == 'P') 
					return DUMP; else return -1;
			default:
				return -1;
		  }
		 break;

	case 'I': if(opcodeStr[1] == 'N' && opcodeStr[2] == 'C') return INC; 
		  else return -1;

	case 'J': if(opcodeStr[1] == 'M' && opcodeStr[2] == 'P') {
				if(opcodeStr[3] == 'E') return JMPE;
				else if(opcodeStr[3] == '\0')return JMP; 
				else return -1;
			  }
			else return -1;

	case 'L':if(opcodeStr[1] == 'O') {
			if(opcodeStr[2] == 'A' && opcodeStr[3] == 'D' ) 
					return LOAD; 
			else return -1;
		  }
		  else return -1;

	case 'N':if(opcodeStr[1] == 'O') {
				if(opcodeStr[2] == 'P') return NOP; 
				else if(opcodeStr[2] == 'T') return NOT; 
				else return -1;
			 }
		  else return -1;

	case 'O':if(opcodeStr[1] == 'R') return OR;
		 else return -1;

	case 'S': switch(opcodeStr[1]) { 
			case 'T': 
					if(opcodeStr[2]=='O'){
						if(opcodeStr[3] == 'P')  
							return STOP; 
						else if(opcodeStr[3] == 'R')  return STOR; 
						else return -1;
					} else return -1;  

                        

			case 'U': if(opcodeStr[2] == 'B') 
					return SUB; else return -1;
			default:
				return -1;
		  }
		 break;


	case 'X': switch(opcodeStr[1]) { 
			case 'C': if(opcodeStr[2] == 'H' && opcodeStr[3] == 'G') 
					 return XCHG; else return -1;
			case 'O': if(opcodeStr[2] == 'R') 
					return XOR; else return -1;
			default:
				return -1;
		  }
		 break;

	default:
		return -1; // illegal opcode
	} // switch

}

/* this function is taken from
http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
*/

char *trim(char *str)
{
    size_t len = 0;
    char *frontp = str - 1;
    char *endp = NULL;

    if( str == NULL )
            return NULL;

    if( str[0] == '\0' )
            return str;

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address
     * the first non-whitespace characters from
     * each end.
     */
    while( isspace(*(++frontp)) );
    while( isspace(*(--endp)) && endp != frontp );

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so
     * that if it's dynamically allocated, we can
     * still free it on the returned pointer.  Note
     * the reuse of endp to mean the front of the
     * string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) *endp++ = *frontp++;
            *endp = '\0';
    }


    return str;
}

void mystrupr(char *str)
{

	int i=0;
	while(str[i]){
		str[i] = toupper(str[i]);
		i++;
	}
}
