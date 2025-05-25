#include "vector.h"
#include "tokenizer.h"
#include "parser.h"
#include "tree.h"
#include "nasmGen.h"

int main() {
    Vector tokens = tokenizer();

    tNode* root = runParser(tokens);

    dump(root);

    RunGenerator(root);

    tokenVectorDtor(&tokens);
    free(tokens.data);

    treeDtor(root);

    return 0;
}
