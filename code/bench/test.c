#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LEN 50

typedef struct Node {
    char type;  
    char op;   
    int num;    
    char var[MAX_LEN];  
    struct Node *left, *right;
} Node;

typedef struct {
    char tokens[MAX_LEN][MAX_LEN];
    int count;
} TokenList;

typedef struct {
    char vars[MAX_LEN][MAX_LEN];
    int count;
} VarList;

Node* createNode(char type, char op, int num, char* var) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Memory Allocation Failed\n");
        return NULL;
    }
    node->type = type;
    node->op = op;
    node->num = num;
    node->var[0] = '\0';
    if (var) strcpy(node->var, var);
    node->left = node->right = NULL;
    return node;
}

int getPrecedence(char op) {
    switch (op) {
        case '^': return 3; 
        case '*':
        case '/': return 2;
        case '+':
        case '-': return 1;
        case '(': return 0;
        default: return -1;
    }
}

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

void initVarList(VarList* list) {
    list->count = 0;
}

void addVar(VarList* list, char* var) {
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->vars[i], var) == 0) return;
    }
    strcpy(list->vars[list->count++], var);
}

Node* parseExpression(TokenList* tokens) {
    Node* stack[MAX_LEN];
    char opStack[MAX_LEN];
    int stackTop = -1, opTop = -1;

    for (int i = 0; i < tokens->count; i++) {
        char* token = tokens->tokens[i];

        if (isdigit(token[0])) {
            stack[++stackTop] = createNode('N', 0, atoi(token), NULL);
        }
        else if (isalpha(token[0])) {
            stack[++stackTop] = createNode('V', 0, 0, token);
        }
        else if (isOperator(token[0])) {
            while (opTop >= 0 &&
                   getPrecedence(opStack[opTop]) > getPrecedence(token[0])) {
                Node* right = stack[stackTop--];
                Node* left = stack[stackTop--];
                Node* op = createNode('O', opStack[opTop--], 0, NULL);
                op->left = left;
                op->right = right;
                stack[++stackTop] = op;
            }
            opStack[++opTop] = token[0];
        }
        else if (token[0] == '(') {
            opStack[++opTop] = token[0];
        }
        else if (token[0] == ')') {
            while (opTop >= 0 && opStack[opTop] != '(') {
                Node* right = stack[stackTop--];
                Node* left = stack[stackTop--];
                Node* op = createNode('O', opStack[opTop--], 0, NULL);
                op->left = left;
                op->right = right;
                stack[++stackTop] = op;
            }
            if (opTop >= 0) opTop--;
        }
    }

    while (opTop >= 0) {
        Node* right = stack[stackTop--];
        Node* left = stack[stackTop--];
        Node* op = createNode('O', opStack[opTop--], 0, NULL);
        op->left = left;
        op->right = right;
        stack[++stackTop] = op;
    }

    return stackTop >= 0 ? stack[stackTop] : NULL;
}

Node* differentiate(Node* root, char* var) {
    if (!root) return NULL;

    if (root->type == 'N') return createNode('N', 0, 0, NULL);
    if (root->type == 'V') return createNode('N', 0, strcmp(root->var, var) == 0 ? 1 : 0, NULL);

    Node *dLeft = differentiate(root->left, var);
    Node *dRight = differentiate(root->right, var);

    switch (root->op) {
        case '+':
        case '-':
            return createNode('O', root->op, 0, NULL)->left = dLeft, root->right = dRight, root;
        case '*':
            return createNode('O', '+', 0, NULL)->left = createNode('O', '*', 0, NULL)->left = dLeft, root->right = root->right, root->right = createNode('O', '*', 0, NULL)->left = root->left, root->right = dRight, root;
        case '^':
            if (root->right->type == 'N') {
                int n = root->right->num;
                Node* coef = createNode('N', 0, n, NULL);
                Node* power = createNode('N', 0, n - 1, NULL);
                Node* exp = createNode('O', '^', 0, NULL);
                exp->left = root->left;
                exp->right = power;
                return createNode('O', '*', 0, NULL)->left = coef, root->right = createNode('O', '*', 0, NULL)->left = exp, root->right = dLeft, root;
            }
            break;
    }
    return createNode('N', 0, 0, NULL);
}


void printExpression(Node* root) {
    if (!root) return;
    if (root->type == 'N') printf("%d", root->num);
    else if (root->type == 'V') printf("%s", root->var);
    else {
        if (root->left) printExpression(root->left);
        printf(" %c ", root->op);
        if (root->right) printExpression(root->right);
    }
}

// release the tree
void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {
    char expr[MAX_LEN];
    printf("Enter the expression: ");
    fgets(expr, MAX_LEN, stdin);
    expr[strcspn(expr, "\n")] = 0;

    TokenList tokens = {0};
    tokenize(expr, &tokens);
    Node* root = parseExpression(&tokens);

    VarList vars;
    initVarList(&vars);
    collectVariables(root, &vars);

    for (int i = 0; i < vars.count; i++) {
        printf("%s's gradient: ", vars.vars[i]);
        printExpression(differentiate(root, vars.vars[i]));
        printf("\n");
    }

    freeTree(root);
    return 0;
}
