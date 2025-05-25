#include "nasmGen.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// static ------------------------------------------------------------------------------------------

static const char* const kNasmFileName = "nasm.s";

static Operations GetOperationType(const char* const word);
static TSymbol* FindSymbol(TSymbolTable* st, const char* name);
static void GetGlobals(TSymbolTable* st, tNode* node);
static void GenerateCode(TSymbolTable* st, tNode* node, FILE* output);

static void EmitNumber(FILE* output, tNode* node);
static void EmitIdentifier(FILE* output, tNode* node);
static void EmitSemicolon(FILE* output, tNode* node, TSymbolTable* st);
static void EmitEqual(FILE* output, tNode* node, TSymbolTable* st);
static void EmitPrint(FILE* output, tNode* node);
static void EmitAdd(FILE* output, tNode* node, TSymbolTable* st);
static void EmitSub(FILE* output, tNode* node, TSymbolTable* st);
static void EmitMul(FILE* output, tNode* node, TSymbolTable* st);
static void EmitDiv(FILE* output, tNode* node, TSymbolTable* st);
static void EmitWhile(FILE* output, tNode* node, TSymbolTable* st);
static void EmitIf(FILE* output, tNode* node, TSymbolTable* st);
static void EmitIdentical(FILE* output, tNode* node, TSymbolTable* st);
static void EmitLess(FILE* output, tNode* node, TSymbolTable* st);
static void EmitGreater(FILE* output, tNode* node, TSymbolTable* st);
static void EmitNotIdentical(FILE* output, tNode* node, TSymbolTable* st);
static void EmitLessOrEqual(FILE* output, tNode* node, TSymbolTable* st);
static void EmitGreaterOrEqual(FILE* output, tNode* node, TSymbolTable* st);

// global ------------------------------------------------------------------------------------------

void RunGenerator(tNode* root) {
    FILE* output = fopen(kNasmFileName, "w");
    assert(output);

    fprintf(output, "global main\n");
    fprintf(output, "extern sin, cos, sqrt, printf\n");
    
    TSymbolTable st = {
        .count = 0,
    };
    GetGlobals(&st, root); // найти все глобальные переменные 

    fprintf(output, "\nsection .data\n");
    fprintf(output, "    fmt db \"%%zu\", 10, 0\n");

    for (size_t i = 0; i < st.count; i++) {
        fprintf(output, "    %s dq %s\n", st.symbols[i].name, st.symbols[i].initialValue);
    } // распечатать все глобалки в цикле 

    fprintf(output, "section .text\n");
    fprintf(output, "main:\n");
    fprintf(output, "    call main1\n");
    fprintf(output, "    mov rax, 60\n");
    fprintf(output, "    xor rdi, rdi\n");
    fprintf(output, "    syscall\n");


    fprintf(output, "\nmain1:\n");
    GenerateCode(&st, root, output); // TODO генерация кода
    fprintf(output, "    ret\n");

    fclose(output);
}

// static ------------------------------------------------------------------------------------------

static void GenerateCode(TSymbolTable* st, tNode* node, FILE* output) {
    if (!node) {
        return;
    }

    switch(node->type) {
        case Number:                    EmitNumber(output, node); break;
        case Identifier:                EmitIdentifier(output, node); break;
        case Operation: {
            switch (GetOperationType(node->value)) {
                case Semicolon:         EmitSemicolon(output, node, st); break;
                case Equal:             EmitEqual(output, node, st); break;
                case Print:             EmitPrint(output, node); break;
                case Add:               EmitAdd(output, node, st); break;
                case Sub:               EmitSub(output, node, st); break;
                case Mul:               EmitMul(output, node, st); break;
                case Div:               EmitDiv(output, node, st); break;
                case While:             EmitWhile(output, node, st); break;
                case If:                EmitIf(output, node, st); break;
                case Identical:         EmitIdentical(output, node, st); break;
                case Less:              EmitLess(output, node, st); break;
                case Greater:           EmitGreater(output, node, st); break;
                case NotIdentical:      EmitNotIdentical(output, node, st); break;
                case LessOrEqual:       EmitLessOrEqual(output, node, st); break;
                case GreaterOrEqual:    EmitGreaterOrEqual(output, node, st); break;
            }
        }
    }
}

static void GetGlobals(TSymbolTable* st, tNode* node) {
    if (!node) {
        return;
    }

    if ((node->type == Operation) && (GetOperationType(node->value) == Equal)) {
        TSymbol* sym = FindSymbol(st, node->left->value);
        if (!sym) {
            for (size_t i = 0; node->right->value[i]; i++) {
                assert(isdigit(node->right->value[i]));
            }
            strncpy(st->symbols[st->count].name, node->left->value, kMaxLengthOfSymbol);
            strncpy(st->symbols[st->count].initialValue, node->right->value, kMaxLengthOfNumber);
            st->count++;
        }
    }

    GetGlobals(st, node->left);
    GetGlobals(st, node->right);
}

static TSymbol* FindSymbol(TSymbolTable* st, const char* name) {
    for (size_t i = 0; i < st->count; i++) {
        if (!strcmp(st->symbols[i].name, name)) {
            return &(st->symbols[i]);
        }
    }
    return NULL;
}

static Operations GetOperationType(const char* const word) {
    assert(word);          

         if (!strcmp(word, "if"    )) return If;                // done
    else if (!strcmp(word, "+"     )) return Add;               // done
    else if (!strcmp(word, "-"     )) return Sub;               // done
    else if (!strcmp(word, "*"     )) return Mul;               // done
    else if (!strcmp(word, "/"     )) return Div;               // done
    else if (!strcmp(word, "sin"   )) return Sin; // done
    else if (!strcmp(word, "cos"   )) return Cos; // done
    else if (!strcmp(word, "sqrt"  )) return Sqrt; // done
    else if (!strcmp(word, "<"     )) return Less;              // done
    else if (!strcmp(word, "while" )) return While;             // done
    else if (!strcmp(word, "="     )) return Equal;             // done
    else if (!strcmp(word, "print" )) return Print;             // done
    else if (!strcmp(word, "return")) return Return; // done
    else if (!strcmp(word, ">"     )) return Greater;           // done
    else if (!strcmp(word, ";"     )) return Semicolon;         // done
    else if (!strcmp(word, "=="    )) return Identical;         // done
    else if (!strcmp(word, "<="    )) return LessOrEqual;       // done
    else if (!strcmp(word, "!="    )) return NotIdentical;      // done
    else if (!strcmp(word, ">="    )) return GreaterOrEqual;    // done

    else return NoOperation;
}

static void EmitNumber(FILE* output, tNode* node) {
    fprintf(output, "\n    push %s; Number\n", node->value);
}

static void EmitIdentifier(FILE* output, tNode* node) {
    fprintf(output, "\n    mov rax, [%s]; start Identifier\n", node->value);
    fprintf(output, "    push rax; end Identifier\n");
}

static void EmitSemicolon(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);
}

static void EmitEqual(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->right, output);
    fprintf(output, "\n    pop rax; start Equal\n");
    fprintf(output, "    mov [%s], rax; end Equal\n", node->left->value); 
}

static void EmitPrint(FILE* output, tNode* node) {
    fprintf(output, "\n    mov rsi, [%s]; start Print\n", node->left->value);  
    fprintf(output, "    mov rdi, fmt\n");
    fprintf(output, "    xor rax, rax\n");
    fprintf(output, "    call printf; end Print\n");
}

static void EmitAdd(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Add\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    add rax, rbx\n");
    fprintf(output, "    push rax; end Add\n");
}

static void EmitSub(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Sub\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    sub rax, rbx\n");
    fprintf(output, "    push rax; end Sub\n");
}

static void EmitMul(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Mul\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    imul rax, rbx\n");
    fprintf(output, "    push rax; end Mul\n");
}

static void EmitDiv(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Div\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cqo\n"); // signed extension rax -> rdx:rax
    fprintf(output, "    idiv rbx\n"); 
    fprintf(output, "    push rax; end Div\n"); 
}

static void EmitWhile(FILE* output, tNode* node, TSymbolTable* st) {
    static size_t whileCounter = 0;
    size_t currentWhile = whileCounter++;
    
    fprintf(output, "\n.while%zu:; start While\n", currentWhile);

    GenerateCode(st, node->left, output);

    fprintf(output, "    pop rax\n");
    fprintf(output, "    test rax, rax\n");
    fprintf(output, "    jz .endwhile%zu\n", currentWhile);

    GenerateCode(st, node->right, output);

    fprintf(output, "    jmp .while%zu\n", currentWhile);
    fprintf(output, ".endwhile%zu:; end While\n", currentWhile);
}

static void EmitIf(FILE* output, tNode* node, TSymbolTable* st) {
    static size_t ifCounter = 0;
    size_t currentIf = ifCounter++;

    GenerateCode(st, node->left, output);

    fprintf(output, "\n    pop rax; start If\n");
    fprintf(output, "    test rax, rax\n");
    fprintf(output, "    jz .endif%zu\n", currentIf);

    GenerateCode(st, node->right, output);

    fprintf(output, ".endif%zu:; end If\n", currentIf);
}

static void EmitIdentical(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Identical\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    sete al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end Identical\n");
}

static void EmitLess(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Less\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    setl al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end Less\n");
}

static void EmitGreater(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start Greater\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    setg al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end Greater\n");
}

static void EmitNotIdentical(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start NotIdentical\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    setne al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end NotIdentical\n");
}

static void EmitLessOrEqual(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start LessOrEqual\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    setle al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end LessOrEqual\n");
}

static void EmitGreaterOrEqual(FILE* output, tNode* node, TSymbolTable* st) {
    GenerateCode(st, node->left, output);
    GenerateCode(st, node->right, output);

    fprintf(output, "\n    pop rbx; start GreaterOrEqual\n");
    fprintf(output, "    pop rax\n");
    fprintf(output, "    cmp rax, rbx\n");
    fprintf(output, "    setge al\n");     
    fprintf(output, "    movzx rax, al\n"); 
    fprintf(output, "    push rax; end GreaterOrEqual\n");
}
