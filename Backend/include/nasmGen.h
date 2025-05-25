#ifndef NASM_GEN
#define NASM_GEN

#include "node.h"

#include <stdio.h>

const size_t kMaxLengthOfSymbol = 32;
const size_t kMaxLengthOfNumber = 32;
const size_t kMaxSymbols = 128;
const int kMaxScopes = 16;

struct TSymbol {
    char name[kMaxLengthOfSymbol];
    char initialValue[kMaxLengthOfNumber]; 
};

struct TSymbolTable {
    TSymbol symbols[kMaxSymbols];
    int count;
    int currentOffset; // for local variables
};

void RunGenerator(tNode* root);

#endif // NASM_GEN
