#ifndef NODE_H
#define NODE_H

enum NodeType
{
    Number     = 1,
    Operation  = 2,
    Identifier = 3,
    Function   = 4,
};

struct tNode
{
    NodeType type;
    const char* value;
    tNode* left;
    tNode* right;
};

enum Operations
{
    NoOperation =  0,
    Semicolon = 1,
    Print = 2,
    While = 3,
    Sqrt = 4,
    Sin = 5,
    Cos = 6,
    If = 7,
    Add = 8,
    Sub = 9,
    Mul = 10,
    Div = 11,
    Less = 12,
    Equal = 13,
    Greater = 14,
    Identical = 15,
    LessOrEqual = 16,
    NotIdentical = 17,
    GreaterOrEqual = 18,
    Def,
    Call,
    Return,
};

const char* const keyIf = "if";
const char* const keyAdd = "+";
const char* const keySub = "-";
const char* const keyMul = "*";
const char* const keyDiv = "/";
const char* const keyLess = "<";
const char* const keyDef = "def";
const char* const keySin = "sin";
const char* const keyCos = "cos";
const char* const keyEnd = "end";
const char* const keyEqual = "=";
const char* const keyGreater = ">";
const char* const keyCall = "call";
const char* const keySqrt = "sqrt";
const char* const keyPrint = "print";
const char* const keyWhile = "while";
const char* const keySemicolon = ";";
const char* const keyIdentical = "==";
const char* const keyReturn = "return";
const char* const keyLessOrEqual = "<=";
const char* const keyNotIdentical = "!=";
const char* const keyGreaterOrEqual = ">=";
const char* const keyLeftParenthesis = "(";
const char* const keyRightParenthesis = ")";
const char* const keyLeftCurlyBracket = "{";
const char* const keyRightCurlyBracket = "}";

#endif // NODE_H
