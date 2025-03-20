#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"
/*necessary header files included*/

Node * createNode(char type, char operation, int number, char * variable){
    Node * tempNode = (Node * )calloc(1, sizeof(Node));
    /*malloc memory for the node*/
    tempNode->type = type;
    tempNode->operator = operation;
    tempNode->number = number;
    /*assign basic information of the node*/
    strcpy(tempNode->variable, "");
    /*initialize the tempNode*/
    if (variable != NULL)
    {
        strcpy(tempNode->variable, variable);
        /*if the variable is not empty, then initialize the variable*/
    }
    tempNode->Left = NULL;
    tempNode->Right = NULL;
    tempNode->Parent = NULL;
    /*set to NULL first, later we would use */
    return tempNode;
}

bool isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
    /*implement the judgement of operators*/
}

void tokenize(char* expression, TokenList* tokenListPtr) {
    int expressionLength = strlen(expression);
    int i = 0, j = 0;
    /*there are to pointers here, i is for the traversal of the entire expression*/
    /*j is for the traversal of the storage of the token in tokenList*/
    tokenListPtr->count = 0;
    /*initialize the count of all tokens*/
    while (i < expressionLength) {
        if (isspace(expression[i])) {
        /*if it is space, then skip*/
            i ++;
            continue;
        }
        j = 0;
        /*j will be initialized for every token*/
        if (isdigit(expression[i])) {
            while (i < expressionLength && isdigit(expression[i]) && j < TOKEN_MAX_NUM - 1) {
                tokenListPtr->tokens[tokenListPtr->count][j ++] = expression[i ++];
                /*the tokens will be stored in tokenList*/
                /*note that the while loop here is for the storage of multi-bit numbers*/
            }
            tokenListPtr->types[tokenListPtr->count] = TOKEN_IS_NUM;
            /*if the type is number, then it will be stored*/
        }
        else if (isOperator(expression[i]) || expression[i] == '(' || expression[i] == ')') {
            /*if the token is an operator or parentheses, it will also need to be stored*/
            tokenListPtr->tokens[tokenListPtr->count][j++] = expression[i++];
            tokenListPtr->types[tokenListPtr->count] = TOKEN_IS_OPERATOR;
            /*store the operator type*/
        }
        else if (isalpha(expression[i]) || expression[i] == '_') {
            /*store the variable type with C standard, which can start with letters of _*/
            while (i < expressionLength && (isalnum(expression[i]) || expression[i] == '_') && j < TOKEN_MAX_NUM - 1) {
                /*the isalnum here is for the bits that can be numbers or letters*/
                tokenListPtr->tokens[tokenListPtr->count][j++] = expression[i++];
                /*the same as the storage in digit, we also need to consider more bits*/
            }
            tokenListPtr->types[tokenListPtr->count] = TOKEN_IS_VAR;
        }
        else {
            i ++;  
            /*the case of other invalid inputs, we can directly skip the characters*/
            continue;
        }
        tokenListPtr->tokens[tokenListPtr->count][j] = '\0';
        /*store the last token to be NULL string, indicating the termination of the entire sequence*/
        if (j > 0) {
        /*j can serve as a flag for actual storage, if 0, then it means that no tokens are stored. */
            tokenListPtr -> count ++;
            /*the count will only be increased when there exists valid token*/
        }
    }
}
/*tokenize will help us seperate the expression into different parts*/

int getPrecedence(char operator)
/*help us to get the precedence of different operators*/
{
    switch (operator)
    {
        case '^':
            return 3;
    /*top priority for the power operator*/
        case '*':
        case '/':
            return 2;
    /*the same priority will use the feature of fall-through of switch*/
        case '+':
        case '-':
            return 1;
        case '(':
            return 0;
        /*because the case of parentheses will be individually treated, so here is the minimum priority*/
        default:
            return -1;
    }
}
/*createExpressionTree: Build an expression tree from the token list*/
/*we will implement with two stacks to store numbers/variables(operands) and operators*/
Node * createExpressionTree(TokenList * tokenListPtr)
{
    int len = tokenListPtr->count;  /* total number of tokens */
    /* Stack for operand nodes. */
    Node * nodeStack[TOKEN_MAX_NUM];
    int nodeTop = -1;
    /* Stack for operator nodes. */
    Node * opStack[TOKEN_MAX_NUM];
    int opTop = -1;
    /* Process each token in the token list. */
    for (int i = 0; i < len; i++)
    {
        /*if the token is a number, push it in the stack*/
        if (tokenListPtr->types[i] == TOKEN_IS_NUM)
        {
            Node * newNode = createNode(TOKEN_IS_NUM, '\0', atoi(tokenListPtr->tokens[i]), NULL);
            nodeStack[++nodeTop] = newNode;
        }
        /*note that every time we need to create the node*/
        else if (tokenListPtr->types[i] == TOKEN_IS_VAR)
        {
            Node * newNode = createNode(TOKEN_IS_VAR, '\0', 0, tokenListPtr->tokens[i]);
            nodeStack[++nodeTop] = newNode;
        }
        /*the case where the token is an operator or a parenthesis*/
        else if (tokenListPtr->types[i] == TOKEN_IS_OPERATOR)
        {
            char currentOp = tokenListPtr->tokens[i][0];
            /*tackle the case where left parenthesis appears*/
            if (currentOp == '(')
            {
                Node * newNode = createNode(TOKEN_IS_OPERATOR, currentOp, 0, NULL);
                opStack[++opTop] = newNode;
            }
            /*pop */
            else if (currentOp == ')')
            {
                while (opTop >= 0 && opStack[opTop]->operator != '(')
                {
                    /* Pop an operator node.*/
                    Node * opNode = opStack[opTop--];
                    /* Pop two operand nodes from nodeStack.*/
                    Node * right = nodeStack[nodeTop--];
                    Node * left = nodeStack[nodeTop--];
                    /*set the children nodes and their parents*/
                    setChildren(opNode, left, right);
                    /*push the corresponding sub-tree*/
                    nodeStack[++nodeTop] = opNode;
                }
                /*pop the left parenthesis if there is any of them left*/
                if (opTop >= 0)
                    opTop--;
            }
            else
            {
                /*tackle the case of meeting greater precedence*/
                while (opTop >= 0 && getPrecedence(opStack[opTop]->operator) >= getPrecedence(currentOp))
                {
                    Node * opNode = opStack[opTop--];
                    /*pop two nodes as operands*/
                    Node * right = nodeStack[nodeTop--];
                    Node * left = nodeStack[nodeTop--];
                    /*set the children*/
                    setChildren(opNode, left, right);
                    nodeStack[++nodeTop] = opNode;
                    /*push the node pack to the stack*/
                }
                Node * newOpNode = createNode(TOKEN_IS_OPERATOR, currentOp, 0, NULL);
                opStack[++opTop] = newOpNode;
            }
        }
    }
    /*processing left parenthesis that are left here*/
    while (opTop >= 0)
    {
        Node * opNode = opStack[opTop--];
        Node * right = nodeStack[nodeTop--];
        Node * left = nodeStack[nodeTop--];
        setChildren(opNode, left, right);
        /*set the corresponding children*/
        nodeStack[++nodeTop] = opNode;
    }
    /*return the final node, which is the root.*/
    return (nodeTop >= 0) ? nodeStack[nodeTop] : NULL;
}

void setChildren(Node * parent, Node * left, Node * right)
/*the function to set the children of a node and the parent of the node, to make indexing easier.*/
{
    parent->Left = left;
    parent->Right = right;
    /*setting the children of the parent node*/
    left->Parent = parent;
    right->Parent = parent;
    /*setting the parent node of the currentnode*/
}

char* getNodeExpr(Node* node) {
    /*used to visit the expression of the current node*/
    if (node->type == TOKEN_IS_VAR) {
        /*variable case*/
        return strdup(node->variable);
        /*strdup is used to return a string that is copied, but not a pointer to the same memory space*/
    } else if (node->type == TOKEN_IS_NUM) {
        /*number case*/
        char buf[EXPR_MAX_LEN];
        /*initialize a buffer zone*/
        sprintf(buf, "%d", node->number);
        /*we use sprintf to store the string into the buffer zone*/
        /*copy the number to the buffer zone*/
        /*note that the number here is stored in the string form*/
        return strdup(buf);
    } else if (node->type == TOKEN_IS_OPERATOR) {
        /*the case where the token is an operator*/
        char* left = getNodeExpr(node->Left);
        /*extract the number as the left operand*/
        char* right = getNodeExpr(node->Right);
        /*extract the number as the right operand*/
        char op = node->operator;
        /*extract the operator*/
        char buf[EXPR_MAX_LEN];
        /*create the buffer zone for storing*/
        if ((op == '0') && strcmp(left, right) == 0)
        {
            sprintf(buf, "0");
        }
        else if ((op == '*') && (strcmp(left, "0") == 0 || strcmp(right, "0") == 0))
        {
            sprintf(buf, "0");
        }
        else if ((op == '^') && strcmp(right, "0") == 0)
        {
            sprintf(buf, "1");
        }
        else if (op == '/' && strcmp(right, "1") == 0)
        {
            sprintf(buf, "%s", left);
        }
        else if (op == '+' && strcmp(left, "0") == 0)
        {
            sprintf(buf, "%s", right);
        }
        else if (op == '-' && strcmp(right, "0") == 0)
        {
            sprintf(buf, "%s", left);
        }
        else
        {
            sprintf(buf, "(%s %c %s)", left, op, right);
        }
        sprintf(buf, "(%s %c %s)", left, op, right);
        free(left);
        free(right);
        return strdup(buf);
    }
    return strdup("0");
}

char* formatExpr(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* buf = (char*)malloc(len + 1);
    va_start(args, fmt);
    vsnprintf(buf, len + 1, fmt, args);
    va_end(args);
    return buf;
}

void collectVariables(Node* node, char** vars, int* count) {
    if (!node) return;
    if (node->type == TOKEN_IS_VAR) {
        bool exists = false;
        for (int i = 0; i < *count; i++) {
            if (strcmp(vars[i], node->variable) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists && *count < TOKEN_MAX_NUM) {
            vars[*count] = strdup(node->variable);
            (*count)++;
        }
    }
    collectVariables(node->Left, vars, count);
    collectVariables(node->Right, vars, count);
}

// 计算导数
char* derive(Node* node, char* var) {
    if (node->type == TOKEN_IS_VAR) {
        return strcmp(node->variable, var) == 0 ? strdup("1") : strdup("0");
    } else if (node->type == TOKEN_IS_NUM) {
        return strdup("0");
    } else if (node->type == TOKEN_IS_OPERATOR) {
        char op = node->operator;
        char* leftDeriv = derive(node->Left, var);
        char* rightDeriv = derive(node->Right, var);
        char* leftExpr = getNodeExpr(node->Left);
        char* rightExpr = getNodeExpr(node->Right);
        char* result = NULL;

        switch(op) {
            case '+':
                result = formatExpr("(%s + %s)", leftDeriv, rightDeriv);
                break;
            case '-':
                result = formatExpr("(%s - %s)", leftDeriv, rightDeriv);
                break;
            case '*':
                result = formatExpr("(%s * %s + %s * %s)", leftExpr, rightDeriv, rightExpr, leftDeriv);
                break;
            case '/':
                result = formatExpr("((%s * %s - %s * %s) / (%s ^ 2))", leftDeriv, rightExpr, rightDeriv, leftExpr, rightExpr);
                break;
            case '^':
                {
                    char* powExpr = getNodeExpr(node);
                    char* term1 = formatExpr("%s * ln(%s)", rightDeriv, leftExpr);
                    char* term2 = formatExpr("%s * %s / %s", rightExpr, leftDeriv, leftExpr);
                    char* sumTerms = formatExpr("(%s + %s)", term1, term2);
                    result = formatExpr("%s * %s", powExpr, sumTerms);
                    free(term1);
                    free(term2);
                    free(sumTerms);
                    free(powExpr);
                }
                break;
            default:
                result = strdup("0");
        }

        free(leftDeriv);
        free(rightDeriv);
        free(leftExpr);
        free(rightExpr);
        return result;
    }
    return strdup("0");
}

void calculateGrad(Node * root) {
    if (!root) return;

    char* variables[TOKEN_MAX_NUM];
    int varCount = 0;
    collectVariables(root, variables, &varCount);

    qsort(variables, varCount, sizeof(char*), compareStrings);

    for (int i = 0; i < varCount; i++) {
        char* derivExpr = derive(root, variables[i]);
        printf("%s: %s\n", variables[i], derivExpr);
        free(derivExpr);
    }

    for (int i = 0; i < varCount; i++) {
        free(variables[i]);
    }
}

int compareStrings(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}