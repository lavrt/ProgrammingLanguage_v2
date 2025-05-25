#ifndef NASM_GEN
#define NASM_GEN

#include "node.h"

#include <stdio.h>

const size_t kMaxLengthOfSymbol = 32;
const size_t kMaxSymbols = 128;
const size_t kMaxScopes = 16;

struct TSymbol {
    char name[kMaxLengthOfSymbol];
    bool isGlobal;
    int offset; // for local variables
    int initialValue; // for global variables
};

struct TSymbolTable {
    TSymbol symbols[kMaxSymbols];
    int count;
    int currentOffset; // for local variables
};

struct TScopeTable {
    TSymbolTable scopes[kMaxScopes];
    TSymbolTable globalScope;
    int currentScope;
};

void RunGenerator(tNode* root);

#endif // NASM_GEN
