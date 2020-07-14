#ifndef ARM11_14_STRUCTS_AND_CONSTANTS_H
#define ARM11_14_STRUCTS_AND_CONSTANTS_H

// Constants
#define MAX_LINE_LEN 511
#define MAX_TOKEN_NUM 6
#define MAX_CHAR_PER_TOKEN 10
#define MAX_LINE_NUM (65536 / 4)
#define MAX_LABEL_LEN 50
#define MAX_LINE_LEN 511
#define BYTE_SIZE 8
#define INSTRUCTION_SIZE 32
#define BYTE_PER_INSTRUCTION 4
#define MEMORY_SIZE 65536
#define PROGRAM_COUNTER_NUM 15
#define CPSR_NUM 16

// The indices of the flags in the CSPR register
#define N 31
#define Z 30
#define C 29
#define V 28

// The positions of certain bits
#define IMMEDIATE_POS 25
#define SET_CONDITION_POS 20
#define ACCUMULATE_POS 21
#define PRE_INDEXING_POS 24
#define LOAD_POS 20
#define UP_POS 23


// structs and enums
enum InstructionType {
  DATA_PROCESSING = 1,
  MULTIPLY = 2,
  SINGLE_DATA_TRANSFER = 3,
  BRANCH = 4,
  SPECIAL = 5
};

typedef enum InstructionType InstructionType;

struct Instruction {
  uint16_t address;
  InstructionType type;
  bool cond;
  bool A;
  bool U;
  bool I;
  bool S;
  bool P;
  bool L;
  bool opcode[4];
  bool Rd[4];
  bool Rn[4];
  bool Rs[4];
  bool Rm[4];
  bool operand2[12];
  bool offset[24];
};

typedef struct Instruction Instruction;

struct ARM {
  bool registers[17][INSTRUCTION_SIZE];
  bool memory[MEMORY_SIZE][BYTE_SIZE];
  bool fetched[INSTRUCTION_SIZE];
  bool stop;
  // A boolean which indicates whether the instruction should be decoded in the
  // next cycle
  bool decode;
  uint16_t addressLastFetched;
  Instruction decodedInstr;
  Instruction cache[8];
};

typedef struct ARM ARM;


struct Tuple {
  char label[MAX_LABEL_LEN];
  uint16_t memoryAddress;
};
typedef struct Tuple Tuple;


struct SymbolTable {
  int size;
  Tuple map[MAX_LABEL_LEN];
};
typedef struct SymbolTable SymbolTable;

/*
* STRUCT representation of ARM instruction file provided
* numOfLines: number of lines of instructions in file
* sourceCode: array of strings with ahc string being 1 INSTRUCTION
*/

typedef struct {
  int numOfLines;
  char sourceCode[MAX_LINE_NUM][MAX_LINE_LEN];
} InstrLines;


typedef struct {
  int outputSize;
  int currentLine;
  bool outputBinary[MAX_LINE_NUM][INSTRUCTION_SIZE];
} Output;



#endif //ARM11_14_STRUCTS_AND_CONSTANTS_H
