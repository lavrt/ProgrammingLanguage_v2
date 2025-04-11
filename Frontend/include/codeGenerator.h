#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <stdio.h>

#include "vector.h"
#include "node.h"

enum FUNCTION
{
    nonFunction,
    functionСall,
    functionDefinition,
    paramsTransmission,
    noParamsTransmission,
    defaultVector,
    funcVector,
};

struct codeGenerator
{
    FILE* codeFile;
    tNode* node;
    size_t ifCounter;
    size_t whileCounter;
    size_t comparsionCounter;
    size_t returnCounter;

    Vector scopeTable;
    size_t nestingLevel;
    size_t freeIndex;

    FUNCTION workingWith;
    FUNCTION isParamsTransmitting;

    Vector funcScopeTable;
    size_t funcNestingLevel;

    FUNCTION typeOfVector;
    const char* nameOfFunc;

};

struct symbol
{
    const char* ID;
    size_t IDAddress;
    size_t numberOfIDsInScope;
};

const size_t SCOPE_SIZE = 64;
const size_t INITIAL_NUMBER_OF_SCOPES = 5;

void runCodeGenerator(tNode* root);

#endif // CODEGENERATOR_H
