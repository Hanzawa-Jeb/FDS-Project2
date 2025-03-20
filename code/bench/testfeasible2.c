#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>  // Added for bool type

#define MAX_LEN 50

// Node type for expression tree
typedef struct Node {
    char type;  // 'N': number, 'V': variable, 'O': operator
    char op;    // Operator: +, -, *, /, ^
    int num;    // For numbers
    char var[MAX_LEN];  // For variables
    struct Node *left, *right;
    struct Node *parent;  // Add parent pointer
} Node;

// Structure to store tokens
typedef struct {
    char tokens[MAX_LEN][MAX_LEN];
    int count;
} TokenList;

void setChildren(Node * parent, Node * left, Node * right);

// Function declarations
Node* createNode(char type, char op, int num, char* var);
int getPrecedence(char op);
bool isOperator(char c);
void tokenize(char* expr, TokenList* tokens);
Node* parseExpression(TokenList* tokens);

// Create a new node
Node* createNode(char type, char op, int num, char* var) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    node->type = type;
    node->op = op;
    node->num = num;
    node->var[0] = '\0';
    if (var) strcpy(node->var, var);
    node->left = node->right = NULL;
    node->parent = NULL;  // Initialize parent pointer
    return node;
}

// Get operator precedence
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

// Check if char is operator
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

// Collect unique variables
typedef struct {
    char vars[MAX_LEN][MAX_LEN];
    int count;
} VarList;

// Initialize variable list
void initVarList(VarList* list) {
    list->count = 0;
}

// Add variable to list if not already present
void addVar(VarList* list, char* var) {
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->vars[i], var) == 0) return;
    }
    strcpy(list->vars[list->count++], var);
}

// Compare function for qsort
int compareStrings(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

// Tokenize the input expression
void tokenize(char* expr, TokenList* tokens) {
    int i = 0, j = 0;
    tokens->count = 0;

    while (expr[i] && tokens->count < MAX_LEN) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        j = 0;
        if (isdigit(expr[i])) {
            while (isdigit(expr[i]) && j < MAX_LEN - 1) {
                tokens->tokens[tokens->count][j++] = expr[i++];
            }
        }
        else if (isalpha(expr[i])) {
            while (isalpha(expr[i]) && j < MAX_LEN - 1) {
                tokens->tokens[tokens->count][j++] = expr[i++];
            }
        }
        else if (i < strlen(expr)) {
            tokens->tokens[tokens->count][j++] = expr[i++];
        }
        
        tokens->tokens[tokens->count][j] = '\0';
        if (j > 0) tokens->count++;
    }
}

// Convert infix expression to expression tree
Node* parseExpression(TokenList* tokens) {
    Node* stack[MAX_LEN];
    char opStack[MAX_LEN];
    int stackTop = -1;
    int opTop = -1;

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
                   getPrecedence(opStack[opTop]) >= getPrecedence(token[0])) {
                Node* right = stack[stackTop--];
                Node* left = stack[stackTop--];
                Node* op = createNode('O', opStack[opTop--], 0, NULL);
                setChildren(op, left, right);
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
                setChildren(op, left, right);
                stack[++stackTop] = op;
            }
            if (opTop >= 0) opTop--;
        }
    }

    while (opTop >= 0) {
        Node* right = stack[stackTop--];
        Node* left = stack[stackTop--];
        Node* op = createNode('O', opStack[opTop--], 0, NULL);
        setChildren(op, left, right);
        stack[++stackTop] = op;
    }

    return stackTop >= 0 ? stack[stackTop] : NULL;
}

// Collect variables from expression tree
void collectVariables(Node* root, VarList* vars) {
    if (!root) return;
    if (root->type == 'V') {
        addVar(vars, root->var);
    }
    collectVariables(root->left, vars);
    collectVariables(root->right, vars);
}

// Print expression tree without unnecessary parentheses
void printExpression(Node* root) {
    if (!root) return;
    
    if (root->type == 'N') {
        // Skip printing if this is 0 in addition/subtraction
        if (root->num == 0 && root->parent && 
            (root->parent->op == '+' || root->parent->op == '-')) {
            return;
        }
        // Skip printing if this is 1 in multiplication
        if (root->num == 1 && root->parent && root->parent->op == '*') {
            return;
        }
        printf("%d", root->num);
    } 
    else if (root->type == 'V') {
        printf("%s", root->var);
    } 
    else {
        // Handle multiplication with zero
        if (root->op == '*' && 
            ((root->left->type == 'N' && root->left->num == 0) ||
             (root->right->type == 'N' && root->right->num == 0))) {
            printf("0");
            return;
        }

        // Handle multiplication with one
        if (root->op == '*') {
            if (root->left->type == 'N' && root->left->num == 1) {
                printExpression(root->right);
                return;
            }
            if (root->right->type == 'N' && root->right->num == 1) {
                printExpression(root->left);
                return;
            }
        }

        // Handle addition with zero
        if ((root->op == '+' || root->op == '-')) {
            if (root->left->type == 'N' && root->left->num == 0) {
                if (root->op == '-') printf("-");
                printExpression(root->right);
                return;
            }
            if (root->right->type == 'N' && root->right->num == 0) {
                printExpression(root->left);
                return;
            }
        }

        // Normal printing with parentheses check
        bool needParens = root->left && root->left->type == 'O' && 
                         getPrecedence(root->left->op) < getPrecedence(root->op);
        if (needParens) printf("(");
        printExpression(root->left);
        if (needParens) printf(")");
        
        printf(" %c ", root->op);
        
        needParens = root->right && root->right->type == 'O' && 
                    (getPrecedence(root->right->op) < getPrecedence(root->op) ||
                     (root->op == '-' || root->op == '/') && 
                     getPrecedence(root->right->op) == getPrecedence(root->op));
        if (needParens) printf("(");
        printExpression(root->right);
        if (needParens) printf(")");
    }
}

// Free the expression tree
void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Add this function before main()

// Perform differentiation on the expression tree with respect to given variable
Node* differentiate(Node* root, char* var) {
    if (!root) return NULL;

    // Derivative of a constant is 0
    if (root->type == 'N') {
        return createNode('N', 0, 0, NULL);
    }

    // Derivative of a variable
    if (root->type == 'V') {
        // If it's the variable we're differentiating with respect to, derivative is 1
        // Otherwise, derivative is 0
        return createNode('N', 0, strcmp(root->var, var) == 0 ? 1 : 0, NULL);
    }

    // For operators, apply differentiation rules
    Node *dLeft = differentiate(root->left, var);
    Node *dRight = differentiate(root->right, var);

    switch (root->op) {
        case '+':
        case '-': {
            // d/dx(f ± g) = d/dx(f) ± d/dx(g)
            Node* result = createNode('O', root->op, 0, NULL);
            setChildren(result, dLeft, dRight);
            return result;
        }
        case '*': {
            // d/dx(f*g) = f'*g + f*g'
            Node* term1 = createNode('O', '*', 0, NULL);
            Node* term2 = createNode('O', '*', 0, NULL);
            Node* result = createNode('O', '+', 0, NULL);
            
            // Deep copy of original nodes for term2
            Node* leftCopy = createNode(root->left->type, root->left->op, 
                                      root->left->num, root->left->var);
            Node* rightCopy = createNode(root->right->type, root->right->op, 
                                       root->right->num, root->right->var);

            setChildren(term1, dLeft, rightCopy);
            setChildren(term2, leftCopy, dRight);
            
            setChildren(result, term1, term2);
            return result;
        }
        case '/': {
            // d/dx(f/g) = (f'*g - f*g')/(g^2)
            // Create numerator: f'*g - f*g'
            Node* term1 = createNode('O', '*', 0, NULL);
            Node* term2 = createNode('O', '*', 0, NULL);
            Node* numerator = createNode('O', '-', 0, NULL);
            
            // Deep copy of nodes
            Node* leftCopy = createNode(root->left->type, root->left->op, 
                                      root->left->num, root->left->var);
            Node* rightCopy = createNode(root->right->type, root->right->op, 
                                       root->right->num, root->right->var);
            Node* rightCopy2 = createNode(root->right->type, root->right->op, 
                                        root->right->num, root->right->var);

            setChildren(term1, dLeft, rightCopy);
            setChildren(term2, leftCopy, dRight);
            setChildren(numerator, term1, term2);

            // Create denominator: g^2
            Node* two = createNode('N', 0, 2, NULL);
            Node* denominator = createNode('O', '^', 0, NULL);
            setChildren(denominator, rightCopy2, two);

            // Create final division
            Node* result = createNode('O', '/', 0, NULL);
            setChildren(result, numerator, denominator);
            return result;
        }
        case '^': {
            // Handle constant exponent case
            if (root->right->type == 'N') {
                int n = root->right->num;
                Node* coef = createNode('N', 0, n, NULL);
                Node* power = createNode('N', 0, n-1, NULL);
                Node* base = createNode(root->left->type, root->left->op, 
                                      root->left->num, root->left->var);
                
                Node* exp = createNode('O', '^', 0, NULL);
                setChildren(exp, base, power);
                
                Node* result = createNode('O', '*', 0, NULL);
                Node* term = createNode('O', '*', 0, NULL);
                setChildren(term, exp, dLeft);
                setChildren(result, coef, term);
                return result;
            }
            // Handle variable exponent case
            else {
                return handleVariableExponent(root->left, root->right, var);
            }
        }
    }
    
    // For unsupported operations
    return createNode('N', 0, 0, NULL);
}

// Helper function to handle logarithmic differentiation for variable exponents
Node* handleVariableExponent(Node* base, Node* exponent, char* var) {
    // Create ln(base)
    Node* lnBase = createNode('O', '*', 0, NULL);
    Node* baseCopy = createNode(base->type, base->op, base->num, base->var);
    
    // Create the product rule: exp * (ln(base) * d(exponent) + exponent * d(ln(base))/base)
    Node* result = createNode('O', '*', 0, NULL);
    
    // Original expression b^x as left child
    Node* powerCopy = createNode('O', '^', 0, NULL);
    Node* baseCopy2 = createNode(base->type, base->op, base->num, base->var);
    Node* exponentCopy = createNode(exponent->type, exponent->op, 
                                  exponent->num, exponent->var);
    setChildren(powerCopy, baseCopy2, exponentCopy);
    
    // Right part of the multiplication
    Node* dExponent = differentiate(exponent, var);
    Node* dBase = differentiate(base, var);
    
    // Create exponent * d(base)/base
    Node* term1 = createNode('O', '/', 0, NULL);
    Node* mulTop = createNode('O', '*', 0, NULL);
    Node* exponentCopy2 = createNode(exponent->type, exponent->op, 
                                   exponent->num, exponent->var);
    setChildren(mulTop, exponentCopy2, dBase);
    Node* baseCopy3 = createNode(base->type, base->op, base->num, base->var);
    setChildren(term1, mulTop, baseCopy3);
    
    setChildren(result, powerCopy, term1);
    return result;
}

void setChildren(Node* parent, Node* left, Node* right) {
    parent->left = left;
    parent->right = right;
    if (left) left->parent = parent;
    if (right) right->parent = parent;
}

int main() {
    // Read input expression
    char expr[MAX_LEN];
    printf("Enter expression: ");
    if (!fgets(expr, MAX_LEN, stdin)) {
        printf("Error reading input!\n");
        return 1;
    }
    expr[strcspn(expr, "\n")] = 0;  // Remove newline

    // Tokenize the expression
    TokenList tokens = {0};
    tokenize(expr, &tokens);
    
    // Parse into expression tree
    Node* root = parseExpression(&tokens);
    if (!root) {
        printf("Failed to parse expression!\n");
        return 1;
    }

    // Collect variables
    VarList vars;
    initVarList(&vars);
    collectVariables(root, &vars);

    // Sort variables lexicographically
    qsort(vars.vars, vars.count, MAX_LEN, compareStrings);

    // Calculate and print derivative for each variable
    for (int i = 0; i < vars.count; i++) {
        Node* derivative = differentiate(root, vars.vars[i]);
        if (derivative) {
            printf("%s: ", vars.vars[i]);
            printExpression(derivative);
            printf("\n");
            freeTree(derivative);
        }
    }

    // Clean up
    freeTree(root);
    return 0;
}