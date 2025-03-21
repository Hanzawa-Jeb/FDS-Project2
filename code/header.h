#ifndef HEADER
#define HEADER
/*include guard, ensuring the overall safety*/

#define EXPR_MAX_LEN 50
/*maximum length for the input expression*/
#define VAR_MAX_LEN 10
/*maximum length for the variable name*/
#define TOKEN_MAX_NUM 20
/*maximum number for tokens*/

#define TOKEN_IS_NUM 'N'
#define TOKEN_IS_VAR 'V'
#define TOKEN_IS_OPERATOR 'O'
/*define some representative values*/

typedef struct Node {
    int type;   
    /*corresponding to the #define ahead*/
    char operator;
    /*operator if the type is N +, -, *, /, ^*/
    int number;
    /*literal num if the type is N*/
    char variable[VAR_MAX_LEN];
    /*name of the variable if the type is V*/
    struct Node * Left, * Right;
    /*Left and Right child tree*/
    struct Node * Parent;
    /*parent node*/
} Node;
/*the struct Node is for the construction of expression tree*/

typedef struct TokenList {
    char tokens[TOKEN_MAX_NUM][EXPR_MAX_LEN];
    /*store the tokens*/
    char types[TOKEN_MAX_NUM];
    /*store the types of the tokens, N, V, O*/
    int count;
    /*count of the tokens*/
} TokenList;
/*Implement a type of datastructure to store*/

void tokenize(char * expression, TokenList * tokenListPtr);
/*the function to parse the expression and storage the tokens into our tokenlist*/
Node * createNode(char type, char operation, int number, char * variable);
/*it is used to create a node, assigning features to it.*/
int getPrecedence(char op);
/*return the precedence of various operators*/
bool isOperator(char c);
/*determine whether c is an operator*/
void setChildren(Node * parent, Node * left, Node * right);
/*set the parent of Node left and right, and set the children of the current node*/
Node * createExpressionTree(TokenList * tokenListPtr);
/*use the tokenlist to create an expression tree*/
void calculateGrad(Node * root);
/*sort the variables with lexicographical order and output their corresponding derivative*/
char* getNodeExpr(Node* node);
/*get the expression of the node*/
char* formatExpr(char* fmt, ...);
/*unified format expression function, no matter what is the length*/
void collectVariables(Node* node, char** vars, int* count);
/*collect all the variables in the expression*/
char* derive(Node* node, char* var);
/*calculate the derivative of variables*/
int compareStrings(char * a, char * b);
/*compare the lexicographical order of strings, used in qsort()*/
#endif