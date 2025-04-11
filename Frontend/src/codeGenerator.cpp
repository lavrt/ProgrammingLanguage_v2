#include "codeGenerator.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "tree.h"
#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static const char* const ASM_CODE_FILE = "../Backend/Assembler/asm_code_in.txt";

static Operations returnNodeValue(const char* const word);
static void generateCode(codeGenerator* cGen);

static void emitIf(codeGenerator* cGen);
static void emitID(codeGenerator* cGen);
static void emitAdd(codeGenerator* cGen);
static void emitSub(codeGenerator* cGen);
static void emitMul(codeGenerator* cGen);
static void emitDiv(codeGenerator* cGen);
static void emitSin(codeGenerator* cGen);
static void emitCos(codeGenerator* cGen);
static void emitDef(codeGenerator* cGen);
static void emitCall(codeGenerator* cGen);
static void emitSqrt(codeGenerator* cGen);
static void emitEqual(codeGenerator* cGen);
static void emitPrint(codeGenerator* cGen);
static void emitWhile(codeGenerator* cGen);
static void emitNumber(codeGenerator* cGen);
static void emitReturn(codeGenerator* cGen);
static void emitSemicolon(codeGenerator* cGen);
static void emitComparsion(codeGenerator* cGen);

// global --------------------------------------------------------------------------------------------------------------

void runCodeGenerator(tNode* root)
{
    assert(root);

    codeGenerator cGen =
    {
        .node = root,
        .ifCounter = 0,
        .whileCounter = 0,
        .comparsionCounter = 0,
        .scopeTable = {},
        .nestingLevel = 0,
        .freeIndex = 0,
        .workingWith = nonFunction,
        .isParamsTransmitting = noParamsTransmission,
        .funcNestingLevel = 0,
    };

    vectorInit(&cGen.scopeTable, INITIAL_NUMBER_OF_SCOPES);
    symbol* symbolTable = (symbol*)calloc(SCOPE_SIZE, sizeof(symbol));
    assert(symbolTable);
    vectorPush(&cGen.scopeTable, symbolTable);

    vectorInit(&cGen.funcScopeTable, INITIAL_NUMBER_OF_SCOPES);
    symbol* funcSymbolTable = (symbol*)calloc(SCOPE_SIZE, sizeof(symbol));
    assert(funcSymbolTable);
    vectorPush(&cGen.funcScopeTable, funcSymbolTable);

    cGen.codeFile = fopen(ASM_CODE_FILE, "wb");
    assert(cGen.codeFile);

    generateCode(&cGen);

    fprintf(cGen.codeFile, "hlt");

    FCLOSE(cGen.codeFile);

    freeAllocatedVectorCells(&cGen.scopeTable);
    vectorFree(&cGen.scopeTable);

    freeAllocatedVectorCells(&cGen.funcScopeTable);
    vectorFree(&cGen.funcScopeTable);
}

// static --------------------------------------------------------------------------------------------------------------

static void generateCode(codeGenerator* cGen)
{
    assert(cGen);

    if (!cGen->node)
    {
        return;
    }

    switch (cGen->node->type)
    {
        case Identifier:             emitID(cGen);         break;
        case Number:                 emitNumber(cGen);     break;
        case Function:               emitDef(cGen);        break;
        case Operation:
        {
            switch (returnNodeValue(cGen->node->value))
            {
                case NoOperation:    emitCall(cGen);       break;
                case Semicolon:      emitSemicolon(cGen);  break;
                case Return:         emitReturn(cGen);     break;
                case Equal:          emitEqual(cGen);      break;
                case Print:          emitPrint(cGen);      break;
                case While:          emitWhile(cGen);      break;
                case Sqrt:           emitSqrt(cGen);       break;
                case Add:            emitAdd(cGen);        break;
                case Sub:            emitSub(cGen);        break;
                case Mul:            emitMul(cGen);        break;
                case Div:            emitDiv(cGen);        break;
                case If:             emitIf(cGen);         break;
                case Sin:            emitSin(cGen);        break;
                case Cos:            emitCos(cGen);        break;
                case Less:
                case Greater:
                case Identical:
                case LessOrEqual:
                case NotIdentical:
                case GreaterOrEqual: emitComparsion(cGen); break;

                default: assert(0);
            }
            break;
        }
        default: assert(0);
    }
}

static Operations returnNodeValue(const char* const word)
{
    assert(word);

         if (!strcmp(word, "if"    )) return If;
    else if (!strcmp(word, "+"     )) return Add;
    else if (!strcmp(word, "-"     )) return Sub;
    else if (!strcmp(word, "*"     )) return Mul;
    else if (!strcmp(word, "/"     )) return Div;
    else if (!strcmp(word, "sin"   )) return Sin;
    else if (!strcmp(word, "cos"   )) return Cos;
    else if (!strcmp(word, "sqrt"  )) return Sqrt;
    else if (!strcmp(word, "<"     )) return Less;
    else if (!strcmp(word, "while" )) return While;
    else if (!strcmp(word, "="     )) return Equal;
    else if (!strcmp(word, "print" )) return Print;
    else if (!strcmp(word, "return")) return Return;
    else if (!strcmp(word, ">"     )) return Greater;
    else if (!strcmp(word, ";"     )) return Semicolon;
    else if (!strcmp(word, "=="    )) return Identical;
    else if (!strcmp(word, "<="    )) return LessOrEqual;
    else if (!strcmp(word, "!="    )) return NotIdentical;
    else if (!strcmp(word, ">="    )) return GreaterOrEqual;

    else return NoOperation;
}

static void emitSemicolon(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    generateCode(cGen);
    cGen->node = node->right;
    generateCode(cGen);
}

static void emitEqual(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->right;
    generateCode(cGen);
    fprintf(cGen->codeFile, "pop ");
    cGen->node = node->left;
    generateCode(cGen);
}

static void emitPrint(codeGenerator* cGen)
{
    assert(cGen);

    cGen->node = cGen->node->left;
    if (cGen->node->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "out\n");
}

static void emitAdd(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    if (node->left->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    cGen->node = node->right;
    if (node->right->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "add\n");
}

static void emitSub(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    if (node->left->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    cGen->node = node->right;
    if (node->right->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "sub\n");
}

static void emitMul(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    if (node->left->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    cGen->node = node->right;
    if (node->right->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "mul\n");
}

static void emitDiv(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    if (node->left->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    cGen->node = node->right;
    if (node->right->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "div\n");
}

static void emitSqrt(codeGenerator* cGen)
{
    assert(cGen);

    cGen->node = cGen->node->left;
    if (cGen->node->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "sqrt\n");
}

static void emitSin(codeGenerator* cGen)
{
    assert(cGen);

    cGen->node = cGen->node->left;
    if (cGen->node->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "sin\n");
}

static void emitCos(codeGenerator* cGen)
{
    assert(cGen);

    cGen->node = cGen->node->left;
    if (cGen->node->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    fprintf(cGen->codeFile, "cos\n");
}

static void emitIf(codeGenerator* cGen)
{
    assert(cGen);

    size_t* nestingLevel = (cGen->typeOfVector == funcVector)? &cGen->funcNestingLevel : &cGen->nestingLevel;
    Vector* vec = (cGen->typeOfVector == funcVector) ? &cGen->funcScopeTable : &cGen->scopeTable;

    (*nestingLevel)++;
    symbol* symbolTable = (symbol*)calloc(SCOPE_SIZE, sizeof(symbol));
    assert(symbolTable);
    vectorPush(vec, symbolTable);

    tNode* node = cGen->node;
    cGen->node = node->left;
    generateCode(cGen);
    fprintf(cGen->codeFile, "push 0\n");
    fprintf(cGen->codeFile, "je LABEL_IF_%lu\n", ++cGen->ifCounter);
    cGen->node = node->right;
    generateCode(cGen);
    fprintf(cGen->codeFile, "LABEL_IF_%lu:\n", cGen->ifCounter);
    memset((symbol*)vectorGet(vec, (*nestingLevel)), 0, SCOPE_SIZE * sizeof(symbol));
    (*nestingLevel)--;
}

static void emitWhile(codeGenerator* cGen)
{
    assert(cGen);

    Vector* vec = (cGen->typeOfVector == funcVector) ? &cGen->funcScopeTable : &cGen->scopeTable;
    size_t* nestingLevel = (cGen->typeOfVector == funcVector)? &cGen->funcNestingLevel : &cGen->nestingLevel;

    (*nestingLevel)++;
    symbol* symbolTable = (symbol*)calloc(SCOPE_SIZE, sizeof(symbol));
    assert(symbolTable);
    vectorPush(vec, symbolTable);

    size_t labelNumber = ++cGen->whileCounter;
    fprintf(cGen->codeFile, "FIRST_LABEL_WHILE_%lu:\n", labelNumber);
    tNode* node = cGen->node;
    cGen->node = node->left;
    generateCode(cGen);
    fprintf(cGen->codeFile, "push 0\n");
    fprintf(cGen->codeFile, "je SECOND_LABEL_WHILE_%lu\n", labelNumber);
    cGen->node = node->right;
    generateCode(cGen);
    fprintf(cGen->codeFile, "jmp FIRST_LABEL_WHILE_%lu\n", labelNumber);
    fprintf(cGen->codeFile, "SECOND_LABEL_WHILE_%lu:\n", labelNumber);
    ((symbol*)vectorGet(vec, (*nestingLevel)))->numberOfIDsInScope = 0;
    memset((symbol*)vectorGet(vec, (*nestingLevel)), 0, SCOPE_SIZE * sizeof(symbol));
    (*nestingLevel)--;
}

static void emitNumber(codeGenerator* cGen)
{
    assert(cGen);

    fprintf(cGen->codeFile, "push %s\n", cGen->node->value);
}

static void emitID(codeGenerator* cGen)
{
    assert(cGen);

    Vector* vec = (cGen->typeOfVector == funcVector) ? &cGen->funcScopeTable : &cGen->scopeTable;
    size_t* nestingLevel = (cGen->typeOfVector == funcVector) ? &cGen->funcNestingLevel : &cGen->nestingLevel;

    if (cGen->workingWith == functionСall)
    {
        fprintf(cGen->codeFile, "push ");
    }
    else if (cGen->workingWith == functionDefinition && cGen->isParamsTransmitting == paramsTransmission)
    {
        fprintf(cGen->codeFile, "pop ");
    }

    int i = (int)(*nestingLevel);
    int j = 0;
    bool variableIsKnown = false;

    for (; i >= 0; i--)
    {
        for (;((symbol*)vectorGet(vec, (size_t)i))[j].ID;j++)
        {
            if (!strcmp(((symbol*)vectorGet(vec, (size_t)i))[j].ID, cGen->node->value))
            {
                variableIsKnown = true;
                break;
            }
        }
        if (variableIsKnown)
        {
            break;
        }
    }

    if (variableIsKnown)
    {
        fprintf(cGen->codeFile, "[%lu]\n", ((symbol*)vectorGet(vec, (size_t)i))[j].IDAddress);
    }
    else
    {
        ((symbol*)vectorGet(vec, (*nestingLevel)))[((symbol*)vectorGet(vec, (*nestingLevel)))->numberOfIDsInScope].ID = cGen->node->value;
        ((symbol*)vectorGet(vec, (*nestingLevel)))[((symbol*)vectorGet(vec, (*nestingLevel)))->numberOfIDsInScope++].IDAddress = cGen->freeIndex;
        fprintf(cGen->codeFile, "[%lu]\n", cGen->freeIndex++);
    }

    if ((cGen->workingWith == functionСall || cGen->workingWith == functionDefinition) && cGen->node->left)
    {
        cGen->node = cGen->node->left;
        generateCode(cGen);
    }
}

static void emitComparsion(codeGenerator* cGen)
{
    assert(cGen);

    tNode* node = cGen->node;
    cGen->node = node->left;
    if (node->left->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    cGen->node = node->right;
    if (node->right->type == Identifier)
    {
        fprintf(cGen->codeFile, "push ");
    }
    generateCode(cGen);
    switch (returnNodeValue(node->value))
    {
        case Less:           fprintf(cGen->codeFile, "jb" ); break;
        case Greater:        fprintf(cGen->codeFile, "ja" ); break;
        case Identical:      fprintf(cGen->codeFile, "je" ); break;
        case LessOrEqual:    fprintf(cGen->codeFile, "jbe"); break;
        case NotIdentical:   fprintf(cGen->codeFile, "jne"); break;
        case GreaterOrEqual: fprintf(cGen->codeFile, "jae"); break;

        default: assert(0);
    }
    fprintf(cGen->codeFile, " FIRST_LABEL_COMPARSION_%lu\n", ++cGen->comparsionCounter);
    fprintf(cGen->codeFile, "push 0\n");
    fprintf(cGen->codeFile, "jmp SECOND_LABEL_COMPARSION_%lu\n", cGen->comparsionCounter);
    fprintf(cGen->codeFile, "FIRST_LABEL_COMPARSION_%lu:\n", cGen->comparsionCounter);
    fprintf(cGen->codeFile, "push 1\n");
    fprintf(cGen->codeFile, "SECOND_LABEL_COMPARSION_%lu:\n", cGen->comparsionCounter);
}

static void emitCall(codeGenerator* cGen)
{
    assert(cGen);

    cGen->workingWith = functionСall;

    tNode* node = cGen->node;
    cGen->node = node->left;
    generateCode(cGen);
    fprintf(cGen->codeFile, "call %s_START\n", node->value);

    cGen->workingWith = nonFunction;
}

static void emitDef(codeGenerator* cGen)
{
    assert(cGen);

    cGen->typeOfVector = funcVector;
    cGen->workingWith = functionDefinition;
    tNode* node = cGen->node;
    cGen->nameOfFunc = node->value;
    fprintf(cGen->codeFile, "jmp %s_SKIP\n", node->value);
    fprintf(cGen->codeFile, "%s_START:\n", node->value);
    fprintf(cGen->codeFile, "pop cx\n");
    cGen->isParamsTransmitting = paramsTransmission;
    cGen->node = node->left;
    generateCode(cGen);
    cGen->isParamsTransmitting = noParamsTransmission;
    cGen->node = node->right;
    generateCode(cGen);
    fprintf(cGen->codeFile, "%s_SKIP:\n", node->value);

    cGen->workingWith = nonFunction;
    cGen->typeOfVector = defaultVector;

    size_t releasedMemoryCells = 0;
    for (size_t i = 0; i < cGen->funcScopeTable.size; i++)
    {
        releasedMemoryCells += ((symbol*)vectorGet(&cGen->funcScopeTable, i))->numberOfIDsInScope;
        ((symbol*)vectorGet(&cGen->funcScopeTable, i))->numberOfIDsInScope = 0;
        memset((symbol*)vectorGet(&cGen->funcScopeTable, i), 0, SCOPE_SIZE * sizeof(symbol));
    }
}

static void emitReturn(codeGenerator* cGen)
{
    assert(cGen);

    cGen->node = cGen->node->left;
    fprintf(cGen->codeFile, "push ");
    generateCode(cGen);

    cGen->workingWith = nonFunction;
    fprintf(cGen->codeFile, "push cx\n");
    fprintf(cGen->codeFile, "ret\n");
}
