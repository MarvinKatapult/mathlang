
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

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
    OperatorPower,
} Operator;

typedef struct SyntaxNode SyntaxNode;
typedef struct SyntaxNode {
    TokenType type;
    SyntaxNode * left;
    SyntaxNode * right;
    double value;
    Operator operator;
} SyntaxNode;

typedef enum OperationStep {
    Exp = 0,
    MultDiv,
    AddSub,
} OperationStep;

#define MAX_TOKEN_SIZE 1048

#define IS_WHITE_SPACE_TOKEN(C) (C == ' ' || C == '\n' || C == '\t' || c == '\0')
#define IS_OPERATOR(C)     (C == '+' || C == '-' || C == '*' || C == '/' || C == '(' || C == ')' || C == '^')

#define BOOL_TO_STR(B)     ((B) ? "true" : "false")
#define ABS(X) ((X) > 0 ? (X) : (-(X)))

static char err_msg[1024];
#define SHOW_ERROR    fprintf(stderr, err_msg)
#define SHOW_ERROR_AND_ABORT SHOW_ERROR; exit(-1)

size_t find_dot_pos(const char * str) {
    const char * current = str;
    while (*current) {
        if (*current == '.') return (size_t)(current - str);
        current++;
    }
    return 0;
}

const char * operatorToStr(Operator op) {
    switch (op) {
        case NoOperator: return "";
        case OperatorPlus: return "+";
        case OperatorMinus: return "-";
        case OperatorMult: return "*";
        case OperatorDiv: return "/";
        case OperatorPower: return "^";
    }
    assert(false);
}

SyntaxNode * createSyntaxNode(SyntaxNode * const left, SyntaxNode * const right, TokenType type, double value, Operator operator) {
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

bool strToValue(const char * str, double * val) {
    size_t dot_pos = find_dot_pos(str);
    if (dot_pos) dot_pos++;
    const size_t len = strlen(str);
    bool after_decimal = false;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '.' && !after_decimal) {
            after_decimal = true;
            continue;
        }
        if (!isdigit(str[i])) return false;

        double add = str[i] - '0';
        const size_t mult_count = dot_pos ? dot_pos - 2 - i : len - i - 1;
        const size_t div_count  = i - (dot_pos - 1);
        if (!after_decimal) for (size_t j = 0; j < mult_count; j++) add *= 10.;
        else                for (size_t j = 0; j < div_count;  j++) add /= 10.;
        *val += add;
    }
    return true;
}

bool tokenize(const char * expression, StrVec * tokens);
bool tokenize_file(const char * filename, StrVec * tokens) {
    FILE * fp = fopen(filename, "r");
    if (!fp) {
        sprintf(err_msg, "Could not open file >%s<\n", filename);
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

void appendSyntaxNode(SyntaxNode ** current_node, SyntaxNode ** new_node, bool first_token, SyntaxNode ** first) {
    if (first_token) {
        *current_node = *new_node;
        *first = *current_node; 
    } else {
        (*current_node)->right = *new_node;
        (*new_node)->left = *current_node;
        *current_node = *new_node;
    }
}

SyntaxNode * createSyntaxTree(const StrVec * tokens) {

    SyntaxNode * first = NULL;
    SyntaxNode * current_node = NULL;

    for (size_t i = 0; i < tokens->count; i++) {
        const char * token = tokens->vals[i];
        const bool first_token = i == 0;
        double val = 0;
        bool is_value = strToValue(token, &val);
        if (is_value) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenValue, val, NoOperator);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        }
            
        if (strcmp(token, "+") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorPlus);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        } else if (strcmp(token, "-") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorMinus);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        } else if (strcmp(token, "*") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorMult);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        } else if (strcmp(token, "/") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorDiv);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        } else if (strcmp(token, "^") == 0) {
            SyntaxNode * new_node = createSyntaxNode(current_node, NULL, TokenOperator, 0, OperatorPower);
            appendSyntaxNode(&current_node, &new_node, first_token, &first);
            continue;
        }

        sprintf(err_msg, "Unknown Operator >%s<\n", token);
        return NULL;
    }

    return first;
}

bool checkSyntax(const SyntaxNode * root) {
    assert(!root->left); // Root has to be a root

    const SyntaxNode * current = root;
    while (current) {
        TokenType tokentype = current->type;
        switch (tokentype) {
            case TokenValue:
                if (current->right) {
                    if (current->right->type != TokenOperator) {
                        sprintf(err_msg, "Syntax Error at Token with Value:%lf. Expected Operator\n", current->value);
                        return false;
                    }
                }
                break;
            case TokenOperator:
                if (!current->left || !current->right) {
                    sprintf(err_msg, "Syntax Error at Token with Operator:%s. Expected Value %s\n", operatorToStr(current->operator), current->left ? "next" : "before");
                    return false;
                }
                if (current->left->type != TokenValue || current->right->type != TokenValue) {
                    sprintf(err_msg, "Syntax Error at Token with Operator:%s. Expected Value next\n", operatorToStr(current->operator));
                    return false;
                }
                break;
        }
        current = current->right;
    }
    return true;
}

bool applyOperator(double a, double b, Operator operator, double * result) {
    if (!result) return false;
    switch (operator) {
        case OperatorPlus:
            *result = a + b;
            return true;
            break;
        case OperatorMinus:
            *result = a - b;
            return true;
            break;
        case OperatorMult:
            *result = a * b;
            return true;
            break;
        case OperatorDiv:
            *result = a / b;
            return true;
            break;
        case OperatorPower:
            *result = pow(a, b);
            return true;
        case NoOperator:
            break;
    }
    return false;
}

bool calculateResult(SyntaxNode * root, double * result) {

    OperationStep current_step = Exp;
    SyntaxNode * current = root;
    while (true) {
        while (current) {
            if (current->type == TokenValue) {
                current = current->right;
                continue;
            }
            bool calc = false;
            switch (current_step) {
                case Exp:
                    calc = current->operator == OperatorPower;
                    break;
                case MultDiv:
                    calc = current->operator == OperatorMult || current->operator == OperatorDiv;
                    break;
                case AddSub:
                    calc = current->operator == OperatorPlus || current->operator == OperatorMinus;
                    break;

            }
            if (calc) {
                printf("--- Applying Operation: %lf %s %lf\n", current->left->value, operatorToStr(current->operator), current->right->value);
                if(!applyOperator(current->left->value, current->right->value, current->operator, &current->value)) {
                    sprintf(err_msg, "Cannot apply Operator >%s<\n", operatorToStr(current->operator));
                    return false;
                }
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

    *result = current->value;
    free(current);
    return true;
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
        SHOW_ERROR_AND_ABORT;
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
    if (!checkSyntax(root)) {
        SHOW_ERROR_AND_ABORT;
    }
    printf("-- Syntax Check Sucess!!\n");

    printf("-- Resolving Syntaxtree\n");
    double result;
    if (!calculateResult(root, &result)) {
        SHOW_ERROR_AND_ABORT;
    }
    printf("Result of Expression:\n%lf\n", result);

    freeStrVec(tokens);
    return 0;
}
