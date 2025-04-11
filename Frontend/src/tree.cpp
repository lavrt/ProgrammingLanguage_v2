#include "tree.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static tNode* memoryAllocationForNode();
static void dumpTreeTraversal(tNode* node, FILE* dumpFile);
static void dumpTreeTraversalWithArrows(tNode* node, FILE* dumpFile);

// global --------------------------------------------------------------------------------------------------------------

tNode* newNode(NodeType type, const char* value, tNode* left, tNode* right)
{
    tNode* node = NULL;

    switch (type)
    {
        case Number:
        {
            node = memoryAllocationForNode();

            node->type = Number;
        }
        break;
        case Identifier:
        {
            node = memoryAllocationForNode();
            node->left = left;
            node->right = right;

            node->type = Identifier;
        }
        break;
        case Operation:
        {
            node = memoryAllocationForNode();

            node->type = Operation;
            node->left = left;
            node->right = right;
        }
        break;
        case Function:
        {
            node = memoryAllocationForNode();

            node->type = Function;
            node->left = left;
            node->right = right;
        }
        break;
        default: assert(0);
    }

    node->value = value;

    return node;
}

void treeDtor(tNode* node)
{
    assert(node);

    if (node->left)
    {
        treeDtor(node->left);
    }
    if (node->right)
    {
        treeDtor(node->right);
    }

    memset(node, 0, sizeof(tNode));
    FREE(node);
}

void dump(tNode* root)
{
    assert(root);

    FILE* dumpFile = fopen(kDumpFileName, "w");
    assert(dumpFile);

    fprintf(dumpFile, "digraph\n");
    fprintf(dumpFile, "{\n    ");
    fprintf(dumpFile, "rankdir = TB;\n    ");
    fprintf(dumpFile, "node [shape=record,style = filled,penwidth = 2.5];\n    ");
    fprintf(dumpFile, "bgcolor = \"#FDFBE4\";\n\n");

    dumpTreeTraversal(root, dumpFile);
    dumpTreeTraversalWithArrows(root, dumpFile);

    fprintf(dumpFile, "}\n");

    FCLOSE(dumpFile);

    #if defined GRAPHVIZ
        system("dot dump.gv -Tpng -o dump.png");
    #endif
}

tNode* copyNode(tNode* node)
{
    return (node)
                  ? newNode(node->type, node->value, copyNode(node->left), copyNode(node->right))
                  : NULL;
}

bool subtreeContainsVariable(tNode* node)
{
    if (!node) return false;

    static int rank = 0;
    static bool presenceOfVariable = false;

    if (node->type == Identifier)
    {
        presenceOfVariable = true;
    }
    else
    {
        rank++;
        subtreeContainsVariable(node->left);
        subtreeContainsVariable(node->right);
        rank--;
    }

    if (presenceOfVariable && !rank)
    {
        presenceOfVariable = false;
        return true;
    }
    else
    {
        return false;
    }
}

// static --------------------------------------------------------------------------------------------------------------

static tNode* memoryAllocationForNode(void)
{
    tNode* node = (tNode*)calloc(1, sizeof(tNode));
    assert(node);

    return node;
}

static void dumpTreeTraversal(tNode* node, FILE* dumpFile)
{
    assert(dumpFile);
    if (!node)
    {
        return;
    }

    static size_t rank = 0;
    fprintf(dumpFile, "    node_%p [rank=%lu,label=\" { node: %p", node, rank, node);

    if (node->type == Number)
    {
        fprintf(dumpFile, " | type: %s | value: %s | ", kNumber, node->value);
    }
    else if (node->type == Identifier)
    {
        fprintf(dumpFile, " | type: %s | value: %s | ", kVariable, node->value);
    }
    else if (node->type == Operation)
    {
        if (!strcmp(node->value, ">" ) || !strcmp(node->value, "<" ) || !strcmp(node->value, "==") ||
        !strcmp(node->value, ">=") || !strcmp(node->value, "<=") || !strcmp(node->value, "!="))
        {
            fprintf(dumpFile, " | type: %s | value: \\%s | ", kOperation, node->value);
        }
        else
        {
            fprintf(dumpFile, " | type: %s | value: %s | ", kOperation, node->value);
        }
    }
    else if (node->type == Function)
    {
        fprintf(dumpFile, " | type: %s | value: %s | ", kFunction, node->value);
    }
    else assert(0);

    fprintf(dumpFile, "{ left: %p | right: %p }} \"", node->left, node->right);

    if (node->type == Number)
    {
        fprintf(dumpFile, ", color = \"#DBD4FF\"];\n");
    }
    else if (node->type == Identifier)
    {
        fprintf(dumpFile, ", color = \"#EBAEE6\"];\n");
    }
    else if (node->type == Operation)
    {
        fprintf(dumpFile, ", color = \"#E8D59E\"];\n");
    }
    else if (node->type == Function)
    {
        fprintf(dumpFile, ", color = \"#E7FFAC\"];\n");
    }

    if (node->left)
    {
        rank++;
        dumpTreeTraversal(node->left, dumpFile);
    }
    if (node->right)
    {
        rank++;
        dumpTreeTraversal(node->right, dumpFile);
    }
    rank--;
}

static void dumpTreeTraversalWithArrows(tNode* node, FILE* dumpFile)
{
    assert(dumpFile);
    if (!node)
    {
        return;
    }

    static int flag = 0;
    if (node->left)
    {
        (flag++) ? fprintf(dumpFile, "-> node_%p ", node->left)
                 : fprintf(dumpFile, "    node_%p -> node_%p ", node, node->left);
        dumpTreeTraversalWithArrows(node->left, dumpFile);
    }
    if (node->right)
    {
        (flag++) ? fprintf(dumpFile, "-> node_%p ", node->right)
                 : fprintf(dumpFile, "    node_%p -> node_%p ", node, node->right);
        dumpTreeTraversalWithArrows(node->right, dumpFile);
    }
    if (flag)
    {
        fprintf(dumpFile, ";\n");
    }
    flag = 0;
}
