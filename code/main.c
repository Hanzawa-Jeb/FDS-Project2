#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
/*with functions to determine whether a character is a letter or a number*/
#include <string.h>
/*a set of string processing functions*/
#include "header.h"
/*necessary header files included*/

int main()
{
    char * inputExpr = (char *)calloc(50, sizeof(char));
    /*initialize the input string*/
    TokenList * tokenListPtr = (TokenList * )calloc(1, sizeof(TokenList));
    /*create the tokenlist to store tokens that are extracted from the expression*/
    Node * rootPtr = (Node * )calloc(1, sizeof(Node));
    /*malloc memory for the input expression*/
    printf("Please input the expression: ");
    /*user input prompt*/
    fgets(inputExpr, EXPR_MAX_LEN, stdin);
    /*get the expression from the user*/
    tokenize(inputExpr, tokenListPtr);
    /*tokenize the input expression string*/
    rootPtr = createExpressionTree(tokenListPtr);
    if (rootPtr == NULL)
    /*which means that the expression tree is not successfully created*/
    {
        return 0;
    }
    else
    {
        calculateGrad(rootPtr);
        /*calculate the gradient of every variable inside*/
    }
    getchar();
    /*used to avoid the terminal from directly shutting down*/
}