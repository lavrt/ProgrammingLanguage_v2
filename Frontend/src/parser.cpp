#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "vector.h"
#include "tree.h"
#include "debug.h"
#include "dsl.h"

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(Vector tokenVector);
static tNode* getIf(Vector tokenVector, size_t* pos);
static tNode* getDef(Vector tokenVector, size_t* pos);
static tNode* getWhile(Vector tokenVector, size_t* pos);
static tNode* getNumber(Vector tokenVector, size_t* pos);
static tNode* getVariable(Vector tokenVector, size_t* pos);
static tNode* getOperation(Vector tokenVector, size_t* pos);
static tNode* getExpression(Vector tokenVector, size_t* pos);
static tNode* getComparsion(Vector tokenVector, size_t* pos);
static tNode* getAssignment(Vector tokenVector, size_t* pos);
static tNode* getParentheses(Vector tokenVector, size_t* pos);
static tNode* getMathFunction(Vector tokenVector, size_t* pos);
static tNode* getMultiplication(Vector tokenVector, size_t* pos);

[[noreturn]] static void syntaxError(int line);

// global --------------------------------------------------------------------------------------------------------------

tNode* runParser(Vector tokenVector)
{
    tNode* root = getGrammar(tokenVector);

    return root;
}

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(Vector tokenVector)
{
    size_t pos = 0;

    tNode* leftNode = getDef(tokenVector, &pos);
    if (strcmp(GET_TOKEN(pos++), keySemicolon))
    {
        syntaxError(__LINE__);
    }
    while (strcmp(GET_TOKEN(pos), keyEnd))
    {
        tNode* rightNode = getDef(tokenVector, &pos);
        if (strcmp(GET_TOKEN(pos++), keySemicolon))
        {

            syntaxError(__LINE__);
        }
        leftNode = SEMICOLON(leftNode, rightNode);
    }

    return leftNode;
}

static tNode* getExpression(Vector tokenVector, size_t* pos)
{
    tNode* leftNode = getMultiplication(tokenVector, pos);

    while (!strcmp(GET_TOKEN(*pos), keyAdd) || !strcmp(GET_TOKEN(*pos), keySub))
    {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getMultiplication(tokenVector, pos);
        if (!strcmp(GET_TOKEN(op), keyAdd))
        {
            leftNode = ADD(leftNode, rightNode);
        }
        else
        {
            leftNode = SUB(leftNode, rightNode);
        }
    }
    return leftNode;
}

static tNode* getComparsion(Vector tokenVector, size_t* pos)
{
    tNode* leftNode = getExpression(tokenVector, pos);

    if (!strcmp(GET_TOKEN(*pos), ">" ) || !strcmp(GET_TOKEN(*pos), "<" ) || !strcmp(GET_TOKEN(*pos), "==") ||
        !strcmp(GET_TOKEN(*pos), ">=") || !strcmp(GET_TOKEN(*pos), "<=") || !strcmp(GET_TOKEN(*pos), "!="))
    {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getExpression(tokenVector, pos);
        if (!strcmp(GET_TOKEN(op), ">"))
        {
            leftNode = newNode(Operation, ">", leftNode, rightNode);
        }
        else if (!strcmp(GET_TOKEN(op), "<"))
        {
            leftNode = newNode(Operation, "<", leftNode, rightNode);
        }
        else if (!strcmp(GET_TOKEN(op), "=="))
        {
            leftNode = newNode(Operation, "==", leftNode, rightNode);
        }
        else if (!strcmp(GET_TOKEN(op), "<="))
        {
            leftNode = newNode(Operation, "<=", leftNode, rightNode);
        }
        else if (!strcmp(GET_TOKEN(op), ">="))
        {
            leftNode = newNode(Operation, ">=", leftNode, rightNode);
        }
        else if (!strcmp(GET_TOKEN(op), "!="))
        {
            leftNode = newNode(Operation, "!=", leftNode, rightNode);
        }
    }

    return leftNode;
}

static tNode* getMultiplication(Vector tokenVector, size_t* pos)
{
    tNode* leftNode = getParentheses(tokenVector, pos);

    while (!strcmp(GET_TOKEN(*pos), keyMul) || !strcmp(GET_TOKEN(*pos), keyDiv))
    {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getParentheses(tokenVector, pos);
        if (!strcmp(GET_TOKEN(op), keyMul))
        {
            leftNode = MUL(leftNode, rightNode);
        }
        else
        {
            leftNode = DIV(leftNode, rightNode);
        }
    }
    return leftNode;
}

static tNode* getParentheses(Vector tokenVector, size_t* pos)
{
    if (!strcmp(GET_TOKEN(*pos), keyLeftParenthesis))
    {
        (*pos)++;
        tNode* node = getComparsion(tokenVector, pos);
        if (strcmp(GET_TOKEN(*pos), keyRightParenthesis))
        {
            syntaxError(__LINE__);
        }
        (*pos)++;
        return node;
    }
    else if (GET_TOKEN_TYPE(*pos) == Identifier)
    {
        tNode* node = getVariable(tokenVector, pos);
        return node;
    }
    else if (GET_TOKEN_TYPE(*pos) == Number)
    {
        return getNumber(tokenVector, pos);
    }
    else if (GET_TOKEN_TYPE(*pos) == Operation)
    {
        return getMathFunction(tokenVector, pos);
    }
    else assert(0);
}

static tNode* getMathFunction(Vector tokenVector, size_t* pos)
{
    if (!strcmp(GET_TOKEN(*pos), keySqrt))
    {
        tNode* node = SQRT(NULL, NULL);

        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;

        node->left = getComparsion(tokenVector, pos);

        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;

        return node;
    }
    else if (!strcmp(GET_TOKEN(*pos), keySin))
    {
        tNode* node = SIN(NULL, NULL);

        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;

        node->left = getComparsion(tokenVector, pos);

        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;

        return node;
    }
    else if (!strcmp(GET_TOKEN(*pos), keyCos))
    {
        tNode* node = COS(NULL, NULL);

        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;

        node->left = getComparsion(tokenVector, pos);

        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;

        return node;
    }
    else
    {
        //FREE(GET_TOKEN(*pos));
        syntaxError(__LINE__);
    }
}

static tNode* getNumber(Vector tokenVector, size_t* pos)
{
    return NUM(GET_TOKEN((*pos)++));
}

static tNode* getVariable(Vector tokenVector, size_t* pos)
{
    return VAR(GET_TOKEN((*pos)++));
}

static tNode* getDef(Vector tokenVector, size_t* pos)
{
    if (!strcmp(GET_TOKEN(*pos), keyDef))
    {
        (*pos)++;
        char* name = GET_TOKEN(*pos);
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getVariable(tokenVector, pos);
        tNode* node = leftNode;
        while (!strcmp(GET_TOKEN(*pos), keySemicolon))
        {
            (*pos)++;
            leftNode->left = newNode(Identifier, GET_TOKEN(*pos), NULL, NULL);
            leftNode = leftNode->left;
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        tNode* rightNode = getOperation(tokenVector, pos);
        return newNode(Function, name, node, rightNode);
    }
    tNode* leftNode = getOperation(tokenVector, pos);
    return leftNode;
}

static tNode* getOperation(Vector tokenVector, size_t* pos)
{
    if (!strcmp(GET_TOKEN(*pos), keyIf))
    {
        (*pos)++;
        tNode* node = getIf(tokenVector, pos);
        return node;
    }
    else if (!strcmp(GET_TOKEN(*pos), keyWhile))
    {
        (*pos)++;
        tNode* node = getWhile(tokenVector, pos);
        return node;
    }
    else if (!strcmp(GET_TOKEN(*pos), keyPrint))
    {
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getComparsion(tokenVector, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;

        return PRINT(leftNode, NULL);
    }
    else if (!strcmp(GET_TOKEN(*pos), keyReturn))
    {
        (*pos)++;
        tNode* leftNode = getComparsion(tokenVector, pos);
        return newNode(Operation, keyReturn, leftNode, NULL);
    }
    else if (!strcmp(GET_TOKEN(*pos), keyLeftCurlyBracket))
    {
        (*pos)++;
        tNode* leftNode = getOperation(tokenVector, pos);
        if (strcmp(GET_TOKEN((*pos)++), keySemicolon))
        {
            syntaxError(__LINE__);
        }
        while (strcmp(GET_TOKEN(*pos), keyRightCurlyBracket))
        {
            tNode* rightNode = getOperation(tokenVector, pos);
            if (strcmp(GET_TOKEN((*pos)++), keySemicolon))
            {
                syntaxError(__LINE__);
            }
            leftNode = SEMICOLON(leftNode, rightNode);
        }

        if (strcmp(GET_TOKEN((*pos)++), keyRightCurlyBracket))
        {
            syntaxError(__LINE__);
        }

        return leftNode;
    }
    else if (!isKeyWord(GET_TOKEN(*pos)))
    {
        return getAssignment(tokenVector, pos);
    }
    else
    {
        syntaxError(__LINE__);
    }
}

static tNode* getIf(Vector tokenVector, size_t* pos)
{
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokenVector, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokenVector, pos);

    return IF(leftNode, rightNode);
}

static tNode* getWhile(Vector tokenVector, size_t* pos)
{
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokenVector, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokenVector, pos);

    return WHILE(leftNode, rightNode);
}

static tNode* getAssignment(Vector tokenVector, size_t* pos)
{
    tNode* leftNode = getVariable(tokenVector, pos);
    tNode* rightNode = NULL;
    if (strcmp(GET_TOKEN(*pos), keyEqual))
    {
        syntaxError(__LINE__);
    }
    (*pos)++;
    if (!strcmp(GET_TOKEN(*pos), keyCall))
    {
        (*pos)++;
        char* name = GET_TOKEN(*pos);
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getVariable(tokenVector, pos);
        while (!strcmp(GET_TOKEN(*pos), keySemicolon))
        {
            (*pos)++;
            leftNode = newNode(Identifier, GET_TOKEN(*pos), leftNode, NULL);
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(Operation, name, leftNode, NULL);
    }
    else
    {
        rightNode = getComparsion(tokenVector, pos);
    }
    return EQUAL(leftNode, rightNode);
}

static void syntaxError(int line)
{
    fprintf(stderr, "Syntax error in %d\n", line);

    exit(EXIT_FAILURE);
}
