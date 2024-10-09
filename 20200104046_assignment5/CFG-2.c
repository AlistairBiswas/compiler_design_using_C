#include <stdio.h>

char token;
char* input;

void Exp();
void Term();
void Factor();

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

    Exp();

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

void Exp()
{
    Term();
    while (token == '+' || token == '-')
    {
        match(token);
        Term();
    }
}

void Term()
{
    Factor();
    while (token == '*' || token == '/')
    {
        match(token);
        Factor();
    }
}

void Factor()
{
    if (token == '(')
    {
        match('(');
        Exp();
        match(')');
    }
    else if (token == 'a' || token == 'b' || token == 'c' || token == 'd' || token == 'e')
    {
        match(token);
    }
    else if (token == '0' || token == '1' || token == '2' || token == '3' || token == '4' || token == '5' || token == '6' || token == '7' || token == '8' || token == '9')
    {
        match(token);
    }
    else
    {
        printf("Rejected by Grammar!\n");
        return 0;
    }
}
