#include "codeGenerator.h"
#include "vector.h"
#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main()
{
    Vector tokens = tokenizer();

    tNode* root = runParser(tokens);

    dump(root);

    runCodeGenerator(root);

    tokenVectorDtor(&tokens);
    free(tokens.data);

    treeDtor(root);

    return 0;
}
