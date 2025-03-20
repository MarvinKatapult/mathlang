
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "cvecs.h"

typedef enum TokenType {
    TokenValue = 0,
    TokenOperator
} TokenType;

typedef enum Operator {
    NoOperator = 0,
    OperatorPlus,
    OperatorMinus,
    OperatorMult,
    OperatorDiv,
} Operator;

typedef struct SyntaxNode SyntaxNode;
typedef struct SyntaxNode {
    TokenType type;
    SyntaxNode * left;
    SyntaxNode * right;
    int value;
    Operator operator;
} SyntaxNode;

typedef enum OperationStep {
    MultDiv = 0,
    AddSub,
} OperationStep;

#define MAX_TOKEN_SIZE 1048

#define IS_WHITE_SPACE_TOKEN(C) (C == ' ' || C == '\n' || C == '\t' || c == '\0')
#define IS_OPERATOR(C)     (C == '+' || C == '-' || C == '*' || C == '/' || C == '(' || C == ')')

#define BOOL_TO_STR(B)     ((B) ? "true" : "false")

const char * operatorToStr(Operator op) {
    switch (op) {
        case NoOperator: return "";
        case OperatorPlus: return "+";
        case OperatorMinus: return "-";
        case OperatorMult: return "*";
        case OperatorDiv: return "/";
    }
    assert(false);
}

SyntaxNode * createSyntaxNode(SyntaxNode * const left, SyntaxNode * const right, TokenType type, int value, Operator operator) {
    SyntaxNode * ret = malloc(sizeof(SyntaxNode));
    assert(ret);
    ret->left = left;
    ret->right = right;
    ret->type = type;
    ret->value = value;
    ret->operator = operator;
    
    return ret;
}

void append_token_and_reset_buffer(char * buf, StrVec * tokens) {
    // printf("Appending Token:>%s<\n", buf);
    appendStrVec(tokens, buf);
    memset(buf, 0, MAX_TOKEN_SIZE);
}

bool strToInt(const char * str, int * val) {
    const size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(str[i])) return false;
        int add = str[i] - '0';
        for (size_t j = 1; j < len - i; j++) add *= 10;
        *val += add;
    }
    return true;
}

bool tokenize(const char * expression, StrVec * tokens);
bool tokenize_file(const char * filename, StrVec * tokens) {
    FILE * fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file >%s<\n", filename);
        return false;
    }
    
    char buf[MAX_TOKEN_SIZE];
    while(fgets(buf, sizeof(buf), fp)) {
        if (!tokenize(buf, tokens)) {
            return false;
        }
    }
    fclose(fp);

    return true;
}

bool tokenize(const char * expression, StrVec * tokens) {

    char buf[MAX_TOKEN_SIZE] = { 0 };
    const size_t len = strlen(expression) + 1;
    for (size_t i = 0; i < len; i++) {
        char c = expression[i];
        const bool whitespace = IS_WHITE_SPACE_TOKEN(c);
        const bool operator = IS_OPERATOR(c);
        assert(!(whitespace && operator));

        if (whitespace) {
            if (strlen(buf) != 0) {
                append_token_and_reset_buffer(buf, tokens);
            }
            continue;
        }

        if (operator) {
            if (strlen(buf) != 0) append_token_and_reset_buffer(buf, tokens);
            buf[0] = c;
            append_token_and_reset_buffer(buf, tokens);
            continue;
        }

        buf[strlen(buf)] = c; 
    }

    return true;
}

SyntaxNode * createSyntaxTree(const StrVec * tokens) {

    SyntaxNode * first = NULL;
    SyntaxNode * current_node = NULL;

    for (size_t i = 0; i < tokens->count; i++) {
        const char * token = tokens->vals[i];
        const bool first_token = i == 0;
        int val = 0;
        bool is_value = strToInt(token, &val);
        if (is_value) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenValue, val, NoOperator);
            if (first_token) {
                current_node = new_node;
                first = new_node; 
            } else {
                current_node->right = new_node;
                new_node->left = current_node;
                current_node = new_node;
            }
            continue;
        }
            
        if (strcmp(token, "+") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorPlus);
            if (first_token) {
                current_node = new_node;
                first = current_node; 
            } else {
                current_node->right = new_node;
                new_node->left = current_node;
                current_node = new_node;
            }
            continue;
        } else if (strcmp(token, "-") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorMinus);
            if (first_token) {
                current_node = new_node;
                first = current_node; 
            } else {
                current_node->right = new_node;
                new_node->left = current_node;
                current_node = new_node;
            }
            continue;
        } else if (strcmp(token, "*") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorMult);
            if (first_token) {
                current_node = new_node;
                first = current_node; 
            } else {
                current_node->right = new_node;
                new_node->left = current_node;
                current_node = new_node;
            }
            continue;
        } else if (strcmp(token, "/") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorDiv);
            if (first_token) {
                current_node = new_node;
                first = current_node; 
            } else {
                current_node->right = new_node;
                new_node->left = current_node;
                current_node = new_node;
            }
            continue;
        }
        assert(false); // UNREACHABLE
    }

    return first;
}

void checkSyntax(const SyntaxNode * root) {
    assert(!root->left); // Root has to be a root

    const SyntaxNode * current = root;
    while (current) {
        TokenType tokentype = current->type;
        switch (tokentype) {
            case TokenValue:
                if (current->right) {
                    if (current->right->type != TokenOperator) {
                        fprintf(stderr, "Syntax Error at Token with Value:%d. Expected Operator\n", current->value);
                        exit(1);
                    }
                }
                break;
            case TokenOperator:
                if (!current->left || !current->right) {
                    fprintf(stderr, "Syntax Error at Token with Operator:%s. Expected Value %s\n", operatorToStr(current->operator), current->left ? "next" : "before");
                    exit(1);
                }
                if (current->left->type != TokenValue || current->right->type != TokenValue) {
                    fprintf(stderr, "Syntax Error at Token with Operator:%s. Expected Value next\n", operatorToStr(current->operator));
                    exit(1);
                }
                break;
        }
        current = current->right;
    }
}

int applyOperator(int a, int b, Operator operator) {
    switch (operator) {
        case OperatorPlus:
            return a + b;
            break;
        case OperatorMinus:
            return a - b;
        case OperatorMult:
            return a * b;
        case OperatorDiv:
            return a / b;
        default:
            assert(false);
    }
}

int calculateResult(SyntaxNode * root) {

    OperationStep current_step = MultDiv;
    SyntaxNode * current = root;
    while (true) {
        while (current) {
            if (current->type == TokenValue) {
                current = current->right;
                continue;
            }
            bool calc = false;
            switch (current_step) {
                case MultDiv:
                    calc = current->operator == OperatorMult || current->operator == OperatorDiv;
                    break;
                case AddSub:
                    calc = current->operator == OperatorPlus || current->operator == OperatorMinus;
                    break;

            }
            if (calc) {
                printf("--- Applying Operation: %d %s %d\n", current->left->value, operatorToStr(current->operator), current->right->value);
                current->value = applyOperator(current->left->value, current->right->value, current->operator);
                current->operator = NoOperator;
                current->type = TokenValue;

                if (current->left->left) {
                    current->left->left->right = current;
                    SyntaxNode * new_left = current->left->left;
                    free(current->left);
                    current->left = new_left;
                } else {
                    free(current->left);
                    current->left = NULL;
                    root = current;
                }

                if (current->right->right) {
                    current->right->right->left = current;
                    SyntaxNode * new_right = current->right->right;
                    free(current->right);
                    current->right = new_right;
                } else {
                    free(current->right);
                    current->right = NULL;
                }
            }

            current = current->right;
        }
        current = root;
        if (current_step == AddSub) break;
        current_step += 1;
    }

    const int ret = current->value;
    free(current);
    return ret;
}

int main(int argc, char * argv[]) {

    if (argc <= 1) {
        fprintf(stderr, "Usage: %s [Expression]\n", argv[0]);
        return -1;
    }

    printf("Starting to calculate Result of Expression:>%s<\n", argv[1]);

    printf("-- Tokenizing\n");
    StrVec tokens = createStrVec();
    if (!tokenize(argv[1], &tokens)) {
        return -1;
    }
    printf("Tokens:\n");
    for (size_t i = 0; i < tokens.count; i++) printf("%s\n", tokens.vals[i]);

    printf("-- Creating Syntax Tree\n");
    SyntaxNode * root = createSyntaxTree(&tokens);
    if (!root) {
        fprintf(stderr, "Creating Syntax Tree failed\n");
        return -1;
    }
    printf("-- Creating Syntax Tree Sucess!\n");

    /*
    SyntaxNode * current_node = root;
    while (current_node) {
        printf("%p -- %p -- %p (OperatorType:>%s<)\n", current_node->left, current_node, current_node->right, operatorToStr(current_node->operator));
        current_node = current_node->right;
    }
    */

    printf("-- Checking Syntax\n");
    checkSyntax(root);
    printf("-- Syntax Check Sucess!!\n");

    printf("-- Resolving Syntaxtree\n");
    printf("Result of Expression:\n%d\n", calculateResult(root));

    freeStrVec(tokens);
    return 0;
}
