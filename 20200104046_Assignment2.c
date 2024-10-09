#include<stdio.h>

int sep(char lexeme)
{
    if(lexeme == '\'' || lexeme == ',' || lexeme == ';' || lexeme == '\"')
    {
        return 1;
    }

    return 0;
}

int par(char lexeme)
{
    if(lexeme == '(' || lexeme == ')')
    {
        return 1;
    }

    return 0;
}

int keyword(char lexeme[])
{
    char *keywords[] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};
    int num_keywords = sizeof(keywords) / sizeof(char *);

    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(lexeme, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int op(char lexeme[])
{
    char *operators[] = {"+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", ",", "->", ".", "->*", ".*", "::", "?:"};
    int num_operators = sizeof(operators) / sizeof(char *);

    for (int i = 0; i < num_operators; i++)
    {
        if (strcmp(lexeme, operators[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int identifier(char lexeme[])
{
    int i = 0;
    int l=0;
    int s=0;
    if(isalpha(lexeme[i]) || (lexeme[i]=='_'))
    {
        s=1;
        i++;
    }
    else
        s=0;
    l=strlen(lexeme);
    if(s)
    {
        for(; i<l; i++)
        {
            if(isalpha(lexeme[i]) || (lexeme[i]=='_') || isdigit(lexeme[i]))
            {
                s=1;
            }
            else
            {
                s=0;
                break;
            }
        }
    }
    return s;
}

int num(char lexeme[])
{
    int i = 0;
    int l=0;
    int s=0;
    if(isdigit(lexeme[i]))
    {
        s=1;
        i++;
    }
    else if(lexeme[i]=='.')
    {
        s=2;
        i++;
    }
    else s=0;
    l=strlen(lexeme);
    if(s==1)
        for( ; i<l; i++)
        {
            if(isdigit(lexeme[i]))
                s=1;
            else if(lexeme[i]=='.')
            {
                s=2;
                i++;
                break;
            }
            else
            {
                s=0;
                break;
            }
        }
    if(s==2)
        if(isdigit(lexeme[i]))
        {
            s=3;
            i++;
        }
        else
            s=0;
    if(s==3)
        for(; i<l; i++)
        {
            if(isdigit(lexeme[i]))
                s=3;
            else
            {
                s=0;
                break;
            }
        }
    if(s==3) s=1;
    return s;
}

int main()
{
    FILE *inputFile = fopen("Input2.txt","r");
    FILE *outputFile = fopen("outputN.txt","w");

    int i=1;
    char c,temp;

    printf("Input File:\n");
    while((c=getc(inputFile))!=EOF)
    {
        printf("%c",c);
        if(c == '<' || c == '>' || c == '!' || c == '=')
        {
            temp = c;
            c=getc(inputFile);
            printf("%c",c);
            if(c == '=')
            {
                fprintf(outputFile," %c%c ",temp,c); // <=
            }
            else if( c == '\'')
            {
                fprintf(outputFile," %c %c ",temp,c);
            }
            else
            {
                fprintf(outputFile," %c %c",temp,c);
            }
        }
        else if(c == ',' || c == ';' || c == '+' || c == '-' || c == '/' || c == '*' || c == '(' || c == '"' || c == ')' || c == '\'')
        {
            fprintf(outputFile," %c ",c);
        }
        else
            fputc(c,outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    outputFile= fopen("outputN.txt","r");

    printf("\n\nOutput Step 1:\n");
    while((c=getc(outputFile))!=EOF)
    {
        printf("%c",c);
    }
    fclose(outputFile);

    outputFile= fopen("outputN.txt","r");
    inputFile = fopen("outputF.txt","w");

    char lex[100];
    char unknown[100];
    int ln;
    i=0;
    int sp = 0,unk_chk = 0;

    while((c=fgetc(outputFile))!=EOF)
    {
        if(!isspace(c))
        {
            lex[i]=c;
            i++;
            sp=0;
        }
        else if(sp != 1)
        {
            sp = 1;
            if(keyword(lex) == 1)
            {
                fprintf(inputFile,"[kw %s] ",lex);
            }
            else if(sep(lex[0]) == 1)
            {
                fprintf(inputFile,"[sep %s] ",lex);
            }
            else if(identifier(lex) == 1)
            {
                fprintf(inputFile,"[id %s] ",lex);
            }
            else if(num(lex) == 1)
            {
                fprintf(inputFile,"[num %s] ",lex);
            }
            else if(op(lex) == 1)
            {
                fprintf(inputFile,"[op %s] ",lex);
            }
            else if(par(lex[0]) == 1)
            {
                fprintf(inputFile,"[par %s] ",lex);
            }
            else
            {
                fprintf(inputFile,"[unkn %s] ",lex);
                unk_chk = 1;
                ln=strlen(lex);
                for(int i=0; i<ln; i++)
                {
                    unknown[i]=lex[i];
                }
            }
            i = 0;
            memset(lex,0,strlen(lex));
        }
    }
    fclose(outputFile);
    fclose(inputFile);

    outputFile= fopen("outputF.txt","r");
    printf("\n\nOutput 2:\n");
    while((c=getc(outputFile))!=EOF)
    {
        printf("%c",c);
    }
    fclose(outputFile);

    if(unk_chk)
        printf("\n\nUnknown lexeme found and the lexeme is ");
    for(int i=0; i<ln; i++)
    {
        printf("%c",unknown[i]);
    }

    printf("\n");
    return 0;
}
