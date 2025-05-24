#include "nasmGen.h"

#include <assert.h>
#include <string.h>

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word);

// global ------------------------------------------------------------------------------------------

void InitScopeTable(TScopeTable* scopeTable) {
    scopeTable->globalScope.count = 0;
    scopeTable->currentScope = -1;
}

void EnterScope(TScopeTable* scopeTable) {
    if (scopeTable->currentScope >= kMaxScopes - 1) {
        fprintf(stderr, "Scope stack oveerflow\n");
        assert(0);
    }
    scopeTable->currentScope++;
    scopeTable->scopes[scopeTable->currentScope].count = 0;
    scopeTable->scopes[scopeTable->currentScope].currentOffset = 8; // [rbp-8] for first variable
}

void ExitScope(TScopeTable* scopeTable) {
    if (scopeTable->currentScope < 0) {
        fprintf(stderr, "Scope stack underflow");
        assert(0);
    }
    scopeTable->currentScope--;
}

void AddVariable(TScopeTable* scopeTable, const char* name, bool isGlobal) {
    TSymbolTable* table = isGlobal ? &scopeTable->globalScope
                                   : &scopeTable->scopes[scopeTable->currentScope];
    
    for (size_t i = 0; i < table->count; i++) {
        if (!strcmp(table->symbols[i].name, name)) {
            fprintf(stderr, "This variable has already exist\n");
            assert(0);
        }
    }

    if (table->count >= kMaxSymbols) {
        fprintf(stderr, "Symbol table overflow\n");
        assert(0);
    }

    TSymbol* symbol = &table->symbols[table->count++];
    strncpy(symbol->name, name, kMaxLengthOfSymbol);
    symbol->isGlobal = isGlobal;

    if (!isGlobal) {
        symbol->offset = table->currentOffset;
        table->currentOffset += 8;
    }
}

TSymbol* FindVariable(TScopeTable* scopeTable, const char* name) {
    if (scopeTable->currentScope >= 0) {
        TSymbolTable* localScope = &scopeTable->scopes[scopeTable->currentScope];
        for (size_t i = 0; i < localScope->count; i++) {
            if (!strcmp(localScope->symbols[i].name, name)) {
                return &localScope->symbols[i];
            }
        }
    }

    for (size_t i = 0; i < scopeTable->globalScope.count; i++) {
        if (!strcmp(scopeTable->globalScope.symbols[i].name, name)) {
            return &scopeTable->globalScope.symbols[i];
        }
    }

    return NULL;
}

void GenerateCode(TScopeTable* scopeTable, tNode* node, FILE* output) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case Number: {
            fprintf(output, "    push %s\n", node->value);
            break;
        }

        case Identifier: {
            TSymbol* var = FindVariable(scopeTable, node->value);
            if (!var) {
                fprintf(stderr, "Variable '%s' used before assignment\n", node->value);
                assert(0);
            }
            
            if (var->isGlobal) {
                fprintf(output, "    mov rax, [%s]\n", var->name);
            } else {
                fprintf(output, "    mov rax, [rbp-%d]\n", var->offset);
            }
            fprintf(output, "    push rax\n");
            break;
        }

        case Calling: {
            size_t argCount = 0;
            tNode* arg = node->left;
            for (; arg; argCount++) {
                arg = arg->left;
            }

            arg = node->left;
            while (arg) {
                GenerateCode(scopeTable, arg, output);
                arg = arg->left;
            }

            fprintf(output, "    call %s\n", node->value);

            if (argCount) {
                fprintf(output, "    add rsp, %zu\n", argCount * 8);
            }

            fprintf(output, "    push rax\n");
        }

        case Operation: {
            switch (GetOperationType(node->value)) {

                case Semicolon: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);
                    break;
                }
                
                case While: {
                    static size_t whileCounter = 0;
                    size_t currentWhile = whileCounter++;
                    
                    fprintf(output, ".while%zu:\n", currentWhile);

                    GenerateCode(scopeTable, node->left, output);

                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    test rax, rax\n");
                    fprintf(output, "    jz .endwhile%zu\n", currentWhile);

                    EnterScope(scopeTable);
                    GenerateCode(scopeTable, node->right, output);
                    ExitScope(scopeTable);

                    fprintf(output, "    jmp .while%zu\n", currentWhile);
                    fprintf(output, ".endwhile%zu:\n", currentWhile);
                    break;
                }

                case If: {
                    static size_t ifCounter = 0;
                    size_t currentIf = ifCounter++;

                    GenerateCode(scopeTable, node->left, output);

                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    test rax, rax\n");
                    fprintf(output, "    jz .endif%zu\n", currentIf);

                    EnterScope(scopeTable);
                    GenerateCode(scopeTable, node->right, output);
                    ExitScope(scopeTable);

                    fprintf(output, ".endif%zu\n", currentIf);
                    break;
                }

                case Add: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    add rax, rbx\n");
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Sub: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    sub rax, rbx\n");
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Mul: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    imul rax, rbx\n");
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Div: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cqo\n"); // signed extension rax -> rdx:rax
                    fprintf(output, "    idiv rbx\n"); 
                    fprintf(output, "    push rax\n"); 
                    break;
                }

                case Sin: { // FIXME Add to the beginning: extern sin, cos, sqrt, printf
                    GenerateCode(scopeTable, node->left, output);
            
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    movq xmm0, rax\n");  
                    fprintf(output, "    call sin\n");
                    fprintf(output, "    movq rax, xmm0\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Cos: { // FIXME Add to the beginning: extern sin, cos, sqrt, printf
                    GenerateCode(scopeTable, node->left, output);

                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    movq xmm0, rax\n");
                    fprintf(output, "    call cos\n");
                    fprintf(output, "    movq rax, xmm0\n");
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Sqrt: { // FIXME Add to the beginning: extern sin, cos, sqrt, printf
                    GenerateCode(scopeTable, node->left, output);
                    
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    movq xmm0, rax\n");
                    fprintf(output, "    call sqrt\n");
                    fprintf(output, "    movq rax, xmm0\n");
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Print: { // FIXME Add to the beginning: extern sin, cos, sqrt, printf
                              // Also:                       section .data
                              //                                 fmt db "%d", 10, 0
                    GenerateCode(scopeTable, node->left, output);

                    fprintf(output, "    pop rdi\n");
                    fprintf(output, "    mov rsi, rdi\n"); // number
                    fprintf(output, "    mov rdi, fmt\n"); // format string
                    fprintf(output, "    mov rax, 0\n"); // 0 floatint-point args
                    fprintf(output, "    call printf\n");
                }

                case Identical: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    sete al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Less: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setl al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Greater: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setg al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case NotIdentical: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setne al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case LessOrEqual: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setle al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case GreaterOrEqual: {
                    GenerateCode(scopeTable, node->left, output);
                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    pop rbx\n");
                    fprintf(output, "    pop rax\n");
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setge al\n");     
                    fprintf(output, "    movzx rax, al\n"); 
                    fprintf(output, "    push rax\n");
                    break;
                }

                case Equal: {
                    if (node->left->type != Identifier) {
                        fprintf(stderr, "Left side of assignment must be a variable\n");
                        assert(0);
                    }
                    
                    GenerateCode(scopeTable, node->right, output);

                    TSymbol* var = FindVariable(scopeTable, node->left->value);
                    if (!var) {
                        bool isGlobal = (scopeTable->currentScope == -1);
                        AddVariable(scopeTable, node->left->value, isGlobal);
                        var = FindVariable(scopeTable, node->left->value);
                    }

                    fprintf(output, "    pop rax");
                    if (var->isGlobal) {
                        fprintf(output, "    mov [%s], rax\n");
                    } else {
                        fprintf(output, "    mov [rbp-%d], rax\n", var->offset);
                    }
                    break;
                }

                case Def: {
                    AnalyzeFunctionBody(scopeTable, node->right);

                    size_t localsSize = CalculateLocalsSize(scopeTable);

                    fprintf(output, "%s:\n", node->value);
                    fprintf(output, "    push rbp\n");
                    fprintf(output, "    mov rbp, rsp\n");
                    if (localsSize > 0) {
                        fprintf(output, "    sub rsp, %zu\n", localsSize);
                    }

                    GenerateCode(scopeTable, node->right, output);

                    fprintf(output, "    leave\n");
                    fprintf(output, "    ret\n");
                    ExitScope(scopeTable);
                    break;
                }

                case Return: {
                    if (node->left) {
                        GenerateCode(scopeTable, node->left, output);
                        fprintf(output, "    pop rax\n");
                    }

                    fprintf(output, "    mov rsp, rbp\n");  // delete local vars
                    fprintf(output, "    pop rbp\n");       // get previous rbp
                    fprintf(output, "    ret\n");
                    break;
                }
            }
        }
    }
}

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word) {
    assert(word);          

         if (!strcmp(word, "if"    )) return If; // done
    else if (!strcmp(word, "+"     )) return Add; // done
    else if (!strcmp(word, "-"     )) return Sub; // done
    else if (!strcmp(word, "*"     )) return Mul; // done
    else if (!strcmp(word, "/"     )) return Div; // done
    else if (!strcmp(word, "sin"   )) return Sin; // done
    else if (!strcmp(word, "cos"   )) return Cos; // done
    else if (!strcmp(word, "sqrt"  )) return Sqrt; // done
    else if (!strcmp(word, "<"     )) return Less; // done
    else if (!strcmp(word, "while" )) return While; // done
    else if (!strcmp(word, "="     )) return Equal; // done
    else if (!strcmp(word, "print" )) return Print; // done
    else if (!strcmp(word, "return")) return Return; // done
    else if (!strcmp(word, ">"     )) return Greater; // done
    else if (!strcmp(word, ";"     )) return Semicolon; // done
    else if (!strcmp(word, "=="    )) return Identical; // done
    else if (!strcmp(word, "<="    )) return LessOrEqual; // done
    else if (!strcmp(word, "!="    )) return NotIdentical; // done
    else if (!strcmp(word, ">="    )) return GreaterOrEqual; // done

    else return NoOperation;
}

static void AnalyzeFunctionBody(TScopeTable* scopeTable, tNode* body) {
    EnterScope(scopeTable);

    AnalyzeLocals(scopeTable, body);
}

static void AnalyzeLocals(TScopeTable* scopeTable, tNode* node) {
    if (!node) {
        return;
    }

    if (node->type == Operation && !strcmp(node->value, keyEqual)
        && node->left->type == Identifier) {
        AddVariable(scopeTable, node->left->value, false);
    }

    AnalyzeLocals(scopeTable, node->left);
    AnalyzeLocals(scopeTable, node->right);
}

static size_t CalculateLocalsSize(TScopeTable* scopeTable) {
    TSymbolTable* funcScope = &scopeTable->scopes[scopeTable->currentScope];
    size_t varCount = funcScope->count;
    size_t totalSize = varCount * 8;

    if (totalSize % 16) {
        totalSize = ((totalSize / 16) + 1) * 16;
    }

    return totalSize;
}