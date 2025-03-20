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
    /* Stack for operand nodes*/
    Node * nodeStack[TOKEN_MAX_NUM];
    int nodeTop = -1;
    /* Stack for operator nodes*/
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

char* getNodeExpr(Node* node)
{
    /*used to visit the expression of the current node*/
    if (node->type == TOKEN_IS_VAR)
    {
        /*variable case*/
        return strdup(node->variable);
        /*strdup is used to return a string that is copied, but not a pointer to the same memory space*/
    } 
    else if (node->type == TOKEN_IS_NUM)
    {
        /*number case*/
        char buf[EXPR_MAX_LEN];
        /*initialize a buffer zone*/
        sprintf(buf, "%d", node->number);
        /*we use sprintf to store the string into the buffer zone*/
        /*copy the number to the buffer zone*/
        /*note that the number here is stored in the string form*/
        return strdup(buf);
    } 
    else if (node->type == TOKEN_IS_OPERATOR)
    {
        /*the case where the token is an operator*/
        char* left = getNodeExpr(node->Left);
        /*extract the number as the left operand*/
        char* right = getNodeExpr(node->Right);
        /*extract the number as the right operand*/
        char op = node->operator;
        /*extract the operator*/
        char buf[EXPR_MAX_LEN];
        /*create the buffer zone for storing*/
        sprintf(buf, "(%s %c %s)", left, op, right);
        /*push the expression into the buffer*/
        free(left);
        free(right);
        /*free the memory space that are malloced*/
        return strdup(buf);
    }
    return strdup("0");
    /*if no situation is satisfied, then return 0 directly*/
}

char* formatExpr(const char* fmt, ...)
/*the ... is used to express that the parameters we accept can be different, according to the situation we are in*/
/*we use va to tackle this problem because we might be faced with inputs with different lengths and requirements*/
{
    va_list args;
    /*variable arguments, change depend on the situation*/
    va_start(args, fmt);
    /*fmt is the last fixed parameter*/
    int len = vsnprintf(NULL, 0, fmt, args);
    /*although we don't need the buffer, we can use this function to get the length of the string*/
    va_end(args);
    /*clean up the argument list*/
    char* buf = (char*)malloc(len + 1);
    /*malloc the buffer space for the expression*/
    va_start(args, fmt);
    vsnprintf(buf, len + 1, fmt, args);
    /*store the string into the buffer zone*/
    va_end(args);
    /*clean the entire variable argument list*/
    return buf;
}

void collectVariables(Node* node, char** vars, int* count)
{
/*used to determine whether the given variable exists in our expression*/
    if (!node)
    { 
        return;
    }
    /*the case where the node is NULL*/
    if (node->type == TOKEN_IS_VAR)
    {
        bool exists = false;
        /*initially set to false*/
        for (int i = 0; i < *count; i++)
        /*traverse through all the variables*/
        {
            if (strcmp(vars[i], node->variable) == 0)
            /*indicates that the variable exists*/
            {
                exists = true;
                /*flag set to true*/
                break;
            }
        }
        if (!exists && *count < TOKEN_MAX_NUM)
        /*there doesn't exist the variable*/
        {
            vars[*count] = strdup(node->variable);
            /*copy the variable that has never been seen*/
            (*count)++;
            /*count increment*/
            /*note that count is a pointer because we want to modify the value of it*/
        }
    }
    collectVariables(node->Left, vars, count);
    /*tail recursion to implement the collection in left subtree*/
    collectVariables(node->Right, vars, count);
    /*collect in the right subtree*/
}

/*calculate the derivatives*/
char* derive(Node* node, char* var)
{
    if (node->type == TOKEN_IS_VAR)
    {
        if (strcmp(node->variable, var) == 0)
        {
            return strdup("1");
            /*if the variable is equal to the current node, then the variable is one*/
            /*note that the numbers are always stored in string*/
        }
        else
        {
            /*if the variable is not equal to the current node, then the derivate must be independent*/
            return strdup("0");
        }
    }
    else if (node->type == TOKEN_IS_NUM)
    {
        /*the derivative of a constant is undoubtedly 0*/
        return strdup("0");
    }
    else if (node->type == TOKEN_IS_OPERATOR)
    {
        /*tackle the problem of derivative with operators*/
        char op = node->operator;
        /*get the operator*/
        char* leftDeriv = derive(node->Left, var);
        /*get the derivative of the left operand*/
        char* rightDeriv = derive(node->Right, var);
        /*get the derivative of the right operand*/
        char* leftExpr = getNodeExpr(node->Left);
        /*get the expression of the left operand*/
        char* rightExpr = getNodeExpr(node->Right);
        /*get the expression of the right operand*/
        /*these four expression will be used in the deriving process*/
        char* result = NULL;
        /*default output*/
        switch(op) {
            /*switch the case according to the operator*/
            case '+':
                if (strcmp(leftDeriv, "0") == 0 && strcmp(rightDeriv, "0") == 0)
                {
                    result = strdup("0");
                    /*if the left and right derivative are all zero, then the output must be also zero.*/
                }
                else if (strcmp(leftDeriv, "0") == 0)
                {
                    /*simplifying the answer*/
                    result = strdup(rightDeriv);
                }
                else if (strcmp(rightDeriv, "0") == 0)
                {
                    /*simplifying the answer in the same way*/
                    result = strdup(leftDeriv);
                }
                else
                {
                    /*in this case, there's no space for further simplification*/
                    result = formatExpr("(%s + %s)", leftDeriv, rightDeriv);
                }
                break;
            case '-':
            /*the situation of minus*/
                if (strcmp(leftDeriv, "0") == 0 && strcmp(rightDeriv, "0") == 0)
                {
                    result = strdup("0");
                    /*if the left and right are both 0, then output 0 directly*/
                }
                else if (strcmp(leftDeriv, "0") == 0)
                {
                    /*if the left is 0, then output the negation of the right derivative*/
                    result = formatExpr("(-%s)", rightDeriv);
                    /*single variable format expression is also feasible because of our function*/
                }
                else if (strcmp(rightDeriv, "0") == 0)
                {
                    /*if the right derivative is 0, then directly output the leftderivative*/
                    result = strdup(leftDeriv);
                }
                else
                {
                    /*no space for simplification*/
                    result = formatExpr("(%s - %s)", leftDeriv, rightDeriv);
                    break;
                }
            case '*':
            /*here we tackle the case of multiplication*/
                if (strcmp(leftExpr, "0") == 0 && strcmp(rightExpr, "0") == 0)
                /*if all zero, then directly output 0*/
                {
                    result = strdup("0");
                    /*directly output 0*/
                }
                else if (strcmp(leftExpr, "0") == 0)
                /*only one side of the expression is zero*/
                {
                    if (strcmp(rightDeriv, "0") == 0)
                    {
                        /*if the right derivative is zero*/
                        result = strdup("0");
                        /*output 0*/
                    }
                    else
                    {
                        /*if the right derivative is not zero*/
                        result = formatExpr("(%s * %s)", leftExpr, rightDeriv);
                        /*output the answer of the multiplication*/
                    }
                }
                else if (strcmp(rightExpr, "0") == 0)
                {
                    /*if the right expression is 0*/
                    if (strcmp(leftDeriv, "0") == 0)
                    {
                        /*if the right expression and left derivative are all zero*/
                        result = strdup("0");
                        /*output 0*/
                    }
                    else
                    {
                        /*if the left derivative is not zero*/
                        result = formatExpr("(%s * %s)", rightExpr, leftDeriv);
                        /*output the answer of the multiplication*/
                    }
                }
                else if (strcmp(leftDeriv, "0") == 0 && strcmp(rightDeriv, "0") == 0)
                {
                    /*if the left and right derivative are all zero*/
                    result = strdup("0");
                    /*output 0*/
                }
                else if (strcmp(leftDeriv, "0") == 0)
                {
                    /*if the left derivative is zero*/
                    result = formatExpr("(%s * %s)", leftExpr, rightDeriv);
                    /*output the answer of the multiplication*/
                }
                else if (strcmp(rightDeriv, "0") == 0)
                {
                    /*if the right derivative is zero*/
                    result = formatExpr("(%s * %s)", rightExpr, leftDeriv);
                    /*output the answer of the multiplication*/
                }
                else
                {
                    /*if there's no space for simplification*/
                    result = formatExpr("(%s * %s + %s * %s)", leftExpr, rightDeriv, rightExpr, leftDeriv);
                    /*output the answer of the multiplication*/
                }
                break;
            case '/':
                result = formatExpr("((%s * %s - %s * %s) / (%s ^ 2))", leftDeriv, rightExpr, rightDeriv, leftExpr, rightExpr);
                /*output the answer of the division by the division derivative law*/
                break;
            case '^':
                {
                    char* powExpr = getNodeExpr(node);
                    /*get the power of the expression*/
                    char* term1 = formatExpr("%s * ln(%s)", rightDeriv, leftExpr);
                    /*separate the expression into two parts*/
                    char* term2 = formatExpr("%s * %s / %s", rightExpr, leftDeriv, leftExpr);
                    char* sumTerms = formatExpr("(%s + %s)", term1, term2);
                    /*get the sumterms*/
                    result = formatExpr("%s * %s", powExpr, sumTerms);
                    free(term1);
                    /*free the memory space*/
                    free(term2);
                    free(sumTerms);
                    free(powExpr);
                }
                break;
            default:
                result = strdup("0");
                /*default output*/
        }

        free(leftDeriv);
        free(rightDeriv);
        free(leftExpr);
        free(rightExpr);
        /*free all the memory that is malloced*/
        return result;
    }
    return strdup("0");
}

void calculateGrad(Node * root) {
    if (!root){
        return;
    /*if the expression tree is not generated, then return NULL*/
    }
    char* variables[TOKEN_MAX_NUM];
    /*create the variable list*/
    int varCount = 0;
    /*count the number of variables*/
    collectVariables(root, variables, &varCount);
    /*collect variables recursively from the root pointer of the entire expression tree*/
    qsort(variables, varCount, sizeof(char*), compareStrings);
    /*sort the variables in the lexicographical order, with compareStrings() providing the comparing function*/
    /*because the requirement is to output with the lexicographical order, I use this.*/
    for (int i = 0; i < varCount; i++)
    {
        char* derivExpr = derive(root, variables[i]);
        /*calculate the derivative of the expression*/
        /*traversing through every variable from the root*/
        printf("%s: %s\n", variables[i], derivExpr);
        /*output the variable and their derivatives*/
        free(derivExpr);
        /*setting free memory space*/
    }
    for (int i = 0; i < varCount; i++)
    {
        free(variables[i]);
        /*setting free the memory space*/
    }
}

int compareStrings(char * a, char * b)
{
    return strcmp(* (char **)a, * (char **)b);
    /*compare the strings in the lexicographical order, which is going to be used in the qsort()*/
}