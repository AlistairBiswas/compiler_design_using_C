#include <stdio.h>

int main()
{
    FILE *inputFile = fopen("input1.c", "r");
    FILE *outputFile = fopen("output.txt", "w");

    char c;
    int delete_comment = 0;

    if (!inputFile)
    {
        printf("Error opening input file");
        return 0;
    }
    if (!outputFile)
    {
        printf("Error opening output file");
        return 0;
    }

    while((c = fgetc(inputFile)) != EOF)
    {
        if (delete_comment == 0)
        {
            if (c == '/')
            {
                if ((c = fgetc(inputFile)) == '*')
                {
                    delete_comment = 1;
                }
                else if (c == '/')
                {
                    while((c = fgetc(inputFile)) != '\n');
                }
                else
                {
                    fputc('/', outputFile);
                    fputc(c, outputFile);
                }
            }
            else if (c != ' ' && c != '\t' && c != '\n')
            {
                fputc(c, outputFile);
            }
        }
        else
        {
            if (c == '*')
            {
                if ((c = fgetc(inputFile)) == '/')
                {
                    delete_comment = 0;
                }
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    outputFile = fopen("output.txt", "r");

    while ((c = fgetc(outputFile)) != EOF)
    {
        putchar(c);
    }

    fclose(outputFile);

    return 0;
}
