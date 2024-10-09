#include <stdio.h>

char token;
char* input;

void A();
void X();

void match(char expectedToken)
{
    if (token == expectedToken)
    {
        token = *input++;
    }
    else
    {
        printf("Rejected by Grammar!\n");
        return 0;
    }
}

int main()
{
    char expression[100];
    printf("Enter an expression: ");
    fgets(expression, sizeof(expression), stdin);
    input = expression;
    token = *input++;

    A();

    if (token == '\n')   // last token
    {
        printf("Accepted by Grammar!\n");
    }
    else
    {
        printf("Rejected by Grammar!\n");
    }
    return 0;
}

void A()
{
    match('a');
    X();
    match('d');
}

void X()
{
    if (token == 'b')
    {
        match('b');
        if (token == 'b')
        {
            match('b');
            X();
        }
        else if (token == 'c')
        {
            match('c');
            X();
        }
    }
}
