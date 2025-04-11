#ifndef TREE_H
#define TREE_H

#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0);
#define FCLOSE(ptr_) \
    do { fclose(ptr_); ptr_ = NULL; } while(0);

const char* const kDumpFileName = "./dump/dump.gv";

const char* const kFunction = "def";
const char* const kNumber = "number";
const char* const kVariable = "variable";
const char* const kOperation = "operation";

tNode* newNode(NodeType type, const char* value, tNode* left, tNode* right);
void treeDtor(tNode* node);
void dump(tNode* root);
tNode* copyNode(tNode* node);
bool subtreeContainsVariable(tNode* node);

#endif // TREE_H
