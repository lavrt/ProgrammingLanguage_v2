#ifndef DSL_H
#define DSL_H

#include "tokenizer.h"

#define CHECK_LEFT_PARENTHESIS \
    do { if (strcmp(GET_TOKEN(*pos), "(")) syntaxError(__LINE__); } while(0);
#define CHECK_RIGHT_PARENTHESIS \
    do { if (strcmp(GET_TOKEN(*pos), ")")) syntaxError(__LINE__); } while(0);

#define GET_TOKEN(pos_) \
    (((Token*)vectorGet(&tokenVector, pos_))->value)
#define GET_TOKEN_TYPE(pos_) \
    (((Token*)vectorGet(&tokenVector, pos_))->type)

#define NUM(value_) \
    newNode(Number, value_, NULL, NULL)
#define VAR(value_) \
    newNode(Identifier, value_, NULL, NULL)
#define ADD(leftNode_, rightNode_) \
    newNode(Operation, "+", leftNode_, rightNode_)
#define SUB(leftNode_, rightNode_) \
    newNode(Operation, "-", leftNode_, rightNode_)
#define MUL(leftNode_, rightNode_) \
    newNode(Operation, "*", leftNode_, rightNode_)
#define DIV(leftNode_, rightNode_) \
    newNode(Operation, "/", leftNode_, rightNode_)
#define SEMICOLON(leftNode_, rightNode_) \
    newNode(Operation, ";", leftNode_, rightNode_)
#define SQRT(leftNode_, rightNode_) \
    newNode(Operation, "sqrt", leftNode_, rightNode_)
#define SIN(leftNode_, rightNode_) \
    newNode(Operation, "sin", leftNode_, rightNode_)
#define COS(leftNode_, rightNode_) \
    newNode(Operation, "cos", leftNode_, rightNode_)
#define PRINT(leftNode_, rightNode_) \
    newNode(Operation, "print", leftNode_, rightNode_)
#define IF(leftNode_, rightNode_) \
    newNode(Operation, "if", leftNode_, rightNode_)
#define WHILE(leftNode_, rightNode_) \
    newNode(Operation, "while", leftNode_, rightNode_)
#define EQUAL(leftNode_, rightNode_) \
    newNode(Operation, "=", leftNode_, rightNode_)

#endif // DSL_H
