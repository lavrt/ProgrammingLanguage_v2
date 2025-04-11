#include "tokenizer.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "node.h"
#include "tree.h"
#include "debug.h"

Vector tokenizer()
{
    FILE* inputFile = fopen(kNameOfFileWithCode, "r");
    assert(inputFile);

    size_t fileSize = getFileSize(inputFile);

    char* dataArray = (char*)calloc(fileSize + 1, sizeof(char));
    assert(dataArray);

    fread(dataArray, sizeof(char), fileSize, inputFile);

    Vector tokenVector;
    vectorInit(&tokenVector, kInitialSizeOfTokenVector);

    for (char* pointerToWord = strtok(dataArray, " \n"); pointerToWord; pointerToWord = strtok(NULL, " \n"))
    {
        Token* currentToken = (Token*)calloc(1, sizeof(Token));
        assert(currentToken);

        currentToken->value = pointerToWord;

        if (tokenVector.size)
        {
            currentToken->left = (Token*)vectorGet(&tokenVector, tokenVector.size - 1);
            ((Token*)vectorGet(&tokenVector, tokenVector.size - 1))->right = currentToken;
        }
        else
        {
            Token* serviceToken = (Token*)calloc(1, sizeof(Token));
            assert(serviceToken);

            currentToken->left = serviceToken;

            serviceToken->value = dataArray;
        }

        if (!strcmp(pointerToWord, keyDef))
        {
            currentToken->type = Function;
        }
        else if (isKeyWord(pointerToWord))
        {
            currentToken->type = Operation;
        }
        else if (isdigit(pointerToWord[0]))
        {
            currentToken->type = Number;
        }
        else
        {
            currentToken->type = Identifier;
        }

        vectorPush(&tokenVector, currentToken);
    }

    return tokenVector;
}

void tokenVectorDtor(Vector* vec)
{
    FREE(((Token*)vectorGet(vec, 0))->left->value);
    FREE(((Token*)vectorGet(vec, 0))->left);
    freeAllocatedVectorCells(vec);
}

size_t getFileSize(FILE* file)
{
    assert(file);

    long currentPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    fseek(file, currentPos, SEEK_SET);

    return size;
}

bool isKeyWord(const char* const word)
{
         if (!strcmp(word, keyIf               )) return true;
    else if (!strcmp(word, keyDef              )) return true;
    else if (!strcmp(word, keyAdd              )) return true;
    else if (!strcmp(word, keySub              )) return true;
    else if (!strcmp(word, keyMul              )) return true;
    else if (!strcmp(word, keyDiv              )) return true;
    else if (!strcmp(word, keyEnd              )) return true;
    else if (!strcmp(word, keySin              )) return true;
    else if (!strcmp(word, keyCos              )) return true;
    else if (!strcmp(word, keyCall             )) return true;
    else if (!strcmp(word, keySqrt             )) return true;
    else if (!strcmp(word, keyLess             )) return true;
    else if (!strcmp(word, keyWhile            )) return true;
    else if (!strcmp(word, keyPrint            )) return true;
    else if (!strcmp(word, keyReturn           )) return true;
    else if (!strcmp(word, keyGreater          )) return true;
    else if (!strcmp(word, keySemicolon        )) return true;
    else if (!strcmp(word, keyIdentical        )) return true;
    else if (!strcmp(word, keyLessOrEqual      )) return true;
    else if (!strcmp(word, keyNotIdentical     )) return true;
    else if (!strcmp(word, keyGreaterOrEqual   )) return true;
    else if (!strcmp(word, keyLeftParenthesis  )) return true;
    else if (!strcmp(word, keyRightParenthesis )) return true;
    else if (!strcmp(word, keyLeftCurlyBracket )) return true;
    else if (!strcmp(word, keyRightCurlyBracket)) return true;

    else return false;
}
