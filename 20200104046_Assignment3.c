#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BUFFER_SIZE 100000
#define MAX_TOKEN_LENGTH 1000
#define TABLE_SIZE 100
#define ELEMENTS_FOR_ST_GEN 1000

typedef struct
{
	int slNo;
	char* name;
	char* id_type;
	char* data_type;
	char* scope;
	char* value;
} SymbolTable;

typedef struct
{
	int id;
	char* name;
} Index;

char* elements[ELEMENTS_FOR_ST_GEN];
SymbolTable table[TABLE_SIZE];
int count = 0;

SymbolTable initSymbolTable()
{
	SymbolTable st;
	st.slNo = -1;
	st.name = (char*)"";
	st.id_type = (char*)"";
	st.data_type = (char*)"";
	st.scope = (char*)"";
	st.value = (char*)"";
	return st;
}

char** str_split(char* a_str, const char a_delim)
{
	char** result = 0;
	size_t count = 0;
	char* str = (char*)malloc((strlen(a_str) + 1) * sizeof(char));
	if (str)
	{
		strcpy(str, a_str);
		char* tmp = str;
		char* last_comma = 0;
		char delim[2];
		delim[0] = a_delim;
		delim[1] = 0;

		while (*tmp)
		{
			if (a_delim == *tmp)
			{
				count++;
				last_comma = tmp;
			}
			tmp++;
		}

		count += last_comma < (str + strlen(str) - 1);
		count++;

		result = (char*)malloc(sizeof(char) * count);

		if (result)
		{
			size_t idx = 0;
			char* token = strtok(str, delim);

			while (token)
			{
				*(result + idx++) = strdup(token);
				token = strtok(0, delim);
			}
			*(result + idx) = 0;
		}
	}

	free(str);
	return result;
}

void extractHeaderFile(const char* dir1, const char* dir2)
{
	FILE* p1, * p2;

	p1 = fopen(dir1, "r");
	p2 = fopen(dir2, "w");

	if (!p1)
	{
		perror("File can't be opened");
		return;
	}

	char c;
	int flag = 0;

	while ((c = fgetc(p1)) != EOF)
	{
		if (c == '#')
		{
			flag = 1;
		}
		else if (flag == 1 && c == '<')
		{
			flag = 2;
		}
		else if (flag == 2 && c == '>')
		{
			flag = 3;
		}
		else if (flag == 3 && c == '\n')
		{
			fputc('\n', p2);
			flag = 0;
		}
		else if (flag == 3)
		{
			fputc(c, p2);
		}
	}

	fclose(p1);
	fclose(p2);
}

void addLineNumber(const char* dir1, const char* dir2)
{
	FILE* p1, * p2;

	p1 = fopen(dir1, "r");
	p2 = fopen(dir2, "w");

	if (!p1)
	{
		perror("File can't be opened");
		return;
	}

	char c;
	int line = 1;

	fprintf(p2, "%d: ", line);
	while ((c = fgetc(p1)) != EOF)
	{
		if (c != '\n')
		{
			fputc(c, p2);
		}
		else
		{
			line++;
			fputc(c, p2);
			fprintf(p2, "%d: ", line);
		}
	}

	fclose(p1);
	fclose(p2);
}

void removeComments(const char* dir1, const char* dir2)
{
	FILE* p1, * p2;

	p1 = fopen(dir1, "r");
	p2 = fopen(dir2, "w");

	if (!p1)
	{
		perror("File can't be opened");
		return;
	}

	char c;
	int flag1 = 0;
	int flag2 = 0;

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	while ((c = fgetc(p1)) != EOF)
	{
		if (flag1 == 1 && c == '*')
		{
			c = fgetc(p1);
			if (c == '/')
			{
				flag1 = 0;
			}
		}
		else if (flag2 == 1 && c == '\n')
		{
			flag2 = 0;
		}
		else if (flag1 == 0 && flag2 == 0)
		{
			if (c == '/')
			{
				c = fgetc(p1);
				if (c == '/')
				{
					flag2 = 1;
				}
				else if (c == '*')
				{
					flag1 = 1;
				}
				else
				{
					strncat(buffer, "/", 1);
					strncat(buffer, &c, 1);
				}
			}
			else if (c == '\n' || c == '\t')
			{
				strncat(buffer, " ", 1);
			}
			else
			{
				strncat(buffer, &c, 1);
			}
		}
	}

	int i;
	for (i = 0; buffer[i]; ++i)
	{
		if (!isspace(buffer[i]) || (i > 0 && !isspace(buffer[i - 1])))
		{
			fputc(buffer[i], p2);
		}
	}

	fclose(p1);
	fclose(p2);
}

void separateLexemes(const char* dir1, const char* dir2)
{
	FILE* p2, * p3;

	p2 = fopen(dir1, "r");
	p3 = fopen(dir2, "w");

	if (!p2)
	{
		return;
	}

	char buffer[BUFFER_SIZE], temp[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	memset(temp, 0, BUFFER_SIZE);

	char c;
	while ((c = fgetc(p2)) != EOF)
	{
		strncat(buffer, &c, 1);
	}

	int i = 0, j = 0;
	while (buffer[i] != '\0')
	{
		if (buffer[i] == ';' || buffer[i] == ',' ||
			buffer[i] == '~' || buffer[i] == '?' ||
			buffer[i] == '(' || buffer[i] == ')' ||
			buffer[i] == '[' || buffer[i] == ']' ||
			buffer[i] == '{' || buffer[i] == '}' ||
			buffer[i] == '\'' || buffer[i] == '\"' ||
			buffer[i] == ':')
		{
			temp[j++] = ' ';
			temp[j++] = buffer[i];
			temp[j++] = ' ';
		}
		else if (buffer[i] == '+')
		{
			if (buffer[i + 1] == '+' || buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '-')
		{
			if (buffer[i + 1] == '-' || buffer[i + 1] == '=' || buffer[i + 1] == '>')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '*')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '/')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '%')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '^')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '=')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '!')
		{
			if (buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '<')
		{
			if (buffer[i + 1] == '=' || buffer[i + 1] == '<')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '>')
		{
			if (buffer[i + 1] == '=' || buffer[i + 1] == '>')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '&')
		{
			if (buffer[i + 1] == '&' || buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else if (buffer[i] == '|')
		{
			if (buffer[i + 1] == '|' || buffer[i + 1] == '=')
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = buffer[i + 1];
				temp[j++] = ' ';
				i++;
			}
			else
			{
				temp[j++] = ' ';
				temp[j++] = buffer[i];
				temp[j++] = ' ';
			}
		}
		else
		{
			temp[j++] = buffer[i];
		}
		i++;
	}

	int len = strlen(temp);
	while (len > 0 && (isspace(temp[len - 1]) || temp[len - 1] == '\t')) {
		len--;
	}
	temp[len] = '\0';

	for (i = 0; temp[i]; i++)
	{
		if (!isspace(temp[i]) || (i > 0 && !isspace(temp[i - 1])))
		{
			fputc(temp[i], p3);
		}
	}

	fclose(p2);
	fclose(p3);
}

int isSeparator(char* lex)
{
	int c = 0;
	char sep[][2] = { ";", ",", "\'", "\"" };

	for (int i = 0; i < 15; i++)
	{
		if (strcmp(sep[i], lex) == 0)
		{
			c = 1;
			break;
		}
	}
	return c;
}

int isParenthesis(char* lex)
{
	int c = 0;
	char par[][2] = { "(", ")" };

	for (int i = 0; i < 6; i++)
	{
		if (strcmp(par[i], lex) == 0)
		{
			c = 1;
			break;
		}
	}
	return c;
}

int isBrace(char* lex)
{
	int c = 0;
	char brc[][2] = { "{", "}" };

	for (int i = 0; i < 2; i++)
	{
		if (strcmp(brc[i], lex) == 0)
		{
			c = 1;
			break;
		}
	}
	return c;
}

int isBracket(char* lex)
{
	int c = 0;
	char par[][2] = { "[", "]" };

	for (int i = 0; i < 6; i++)
	{
		if (strcmp(par[i], lex) == 0)
		{
			c = 1;
			break;
		}
	}
	return c;
}

int isPreprocessor(char* lex)
{
	int c = 0;

	if (lex[0] == '#')
	{
		c = 1;
	}
	return c;
}

int isOperator(char* lex)
{
	int c = 0;
	if (strlen(lex) == 1 && (lex[0] == ';' || lex[0] == ',' || lex[0] == ':' || lex[0] == '~' || lex[0] == '?' || lex[0] == '(' || lex[0] == ')' || lex[0] == '[' || lex[0] == ']' || lex[0] == '{' || lex[0] == '}' || lex[0] == '\'' || lex[0] == '\"' || lex[0] == '+' || lex[0] == '-' || lex[0] == '*' || lex[0] == '/' || lex[0] == '%' || lex[0] == '^' || lex[0] == '=' || lex[0] == '!' || lex[0] == '<' || lex[0] == '>' || lex[0] == '&' || lex[0] == '|'))
	{
		c = 1;
	}
	else if (strlen(lex) == 2 && (lex[0] == '+' || lex[0] == '-' || lex[0] == '*' || lex[0] == '/' || lex[0] == '%' || lex[0] == '^' || lex[0] == '=' || lex[0] == '!' || lex[0] == '<' || lex[0] == '>' || lex[0] == '&' || lex[0] == '|'))
	{
		if (lex[1] == '+' || lex[1] == '-' || lex[1] == '<' || lex[1] == '>' || lex[1] == '&' || lex[1] == '|' || lex[1] == '=')
		{
			c = 1;
		}
	}
	else if (strlen(lex) == 3 && (lex[0] == '>' || lex[0] == '<'))
	{
		if (lex[2] == '=')
		{
			c = 1;
		}
	}
	return c;
}

int isNumericConstant(char* lex)
{
	int s = 0, i = 0, l;

	if (isdigit(lex[i]))
	{
		s = 1;
		i++;
	}
	else if (lex[i] == '.')
	{
		s = 2;
		i++;
	}
	else
	{
		s = 0;
	}

	l = strlen(lex);

	if (s == 1)
	{
		for (; i < l; i++)
		{
			if (isdigit(lex[i]))
			{
				s = 1;
			}
			else if (lex[i] == '.')
			{
				s = 2;
				i++;
				break;
			}
			else
			{
				s = 0;
				break;
			}
		}
	}

	if (s == 2)
	{
		if (isdigit(lex[i]))
		{
			s = 3;
			i++;
		}
		else
		{
			s = 0;
		}
	}

	if (s == 3)
	{
		for (; i < l; i++)
		{
			if (isdigit(lex[i]))
			{
				s = 3;
			}

			else
			{
				s = 0;
				break;
			}
		}
	}

	if (s == 3)
	{
		s = 1;
		//return 1;
	}

	return s;
}

int isKeyword(char* lex)
{
	int c = 0;
	char kw[][10] = { "auto", "break", "case", "char", "const", "continue", "default",
			"do", "double", "else", "enum", "extern", "float", "for", "goto",
			"if", "int", "long", "register", "return", "short", "signed",
			"sizeof", "static", "struct", "switch", "typedef", "union",
			"unsigned", "void", "volatile", "while" };

	for (int i = 0; i < 32; i++)
	{
		if (strcmp(kw[i], lex) == 0)
		{
			c = 1;
			break;
		}
	}
	return c;
}

int isIdentifier(char* lex)
{
	int s = 0, i = 0, l;

	if ((isalpha(lex[i])) || (lex[i] == '_'))
	{
		s = 1;
		i++;
	}
	else
	{
		s = 0;
	}

	l = strlen(lex);

	if (s == 1)
	{
		for (; i < l; i++)
		{
			if (isalpha(lex[i]) || lex[i] == '_' || isdigit(lex[i]))
			{
				s = 2;
			}
			else
			{
				s = 0;
				break;
			}
		}
	}

	if (s == 2)
	{
		s = 1;
	}

	return s;
}

void identifyLexemes(const char* dir1, const char* dir2)
{
	FILE* p3, * p4;

	p3 = fopen(dir1, "r");
	p4 = fopen(dir2, "w");

	if (p3 == NULL)
	{
		return;
	}

	char c;
	char lex[1000];
	int index = 0;

	while ((c = fgetc(p3)) != EOF)
	{
		if (c == ' ')
		{
			if (index != 0)
			{
				lex[index] = '\0';
				if (isSeparator(lex))
				{
					fputs("[sep ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isParenthesis(lex))
				{
					fputs("[par ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isBrace(lex))
				{
					fputs("[brc ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isBracket(lex))
				{
					fputs("[brk ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isPreprocessor(lex))
				{
					fputs("[ppc ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isOperator(lex))
				{
					fputs("[op ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isNumericConstant(lex))
				{
					fputs("[num ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isKeyword(lex))
				{
					fputs("[kw ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				else if (isIdentifier(lex))
				{
					fputs("[id ", p4);
					fputs(lex, p4);
					fputs("] ", p4);

				}
				else
				{
					fputs("[unkn ", p4);
					fputs(lex, p4);
					fputs("] ", p4);
				}
				index = 0;
			}
		}
		else
		{
			lex[index++] = c;
		}
	}
	if (isSeparator(lex))
	{
		fputs("[sep ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isParenthesis(lex))
	{
		fputs("[par ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isBrace(lex))
	{
		fputs("[brc ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isBracket(lex))
	{
		fputs("[brk ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isPreprocessor(lex))
	{
		fputs("[ppc ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isOperator(lex))
	{
		fputs("[op ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isNumericConstant(lex))
	{
		fputs("[num ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isKeyword(lex))
	{
		fputs("[kw ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}
	else if (isIdentifier(lex))
	{
		fputs("[id ", p4);
		fputs(lex, p4);
		fputs("] ", p4);

	}
	else
	{
		fputs("[unkn ", p4);
		fputs(lex, p4);
		fputs("] ", p4);
	}

	fclose(p3);
	fclose(p4);
}

void editPairs(const char* dir1, const char* dir2)
{
	FILE* p4, * p5;

	p4 = fopen(dir1, "r");
	p5 = fopen(dir2, "w");

	if (p4 == NULL)
	{
		return;
	}

	fseek(p4, 0, SEEK_END);
	long fileLength = ftell(p4);
	fseek(p4, 0, SEEK_SET);

	char input[BUFFER_SIZE];
	fread(input, 1, fileLength, p4);
	input[fileLength] = '\0';

	char str[BUFFER_SIZE];
	memset(str, 0, BUFFER_SIZE);
	char _pair[2][MAX_TOKEN_LENGTH];

	char* token = strtok(input, "[]");

	while (token != NULL)
	{
		int len = strlen(token);
		if (len > 1)
		{
			while (len > 0 && (isspace(token[len - 1]) || token[len - 1] == '\t'))
			{
				len--;
			}
			token[len] = 0;
			int i = 0;
			while (token[i] != ' ' && token[i] != 0)
			{
				_pair[0][i] = token[i];
				i++;
			}
			_pair[0][i] = 0;
			while (token[i] == ' ')
			{
				i++;
			}
			int j = 0;
			while (token[i] != 0)
			{
				_pair[1][j] = token[i];
				i++;
				j++;
			}
			_pair[1][j] = 0;
			if (strcmp(_pair[0], "id") != 0)
			{
				strcat(str, "[");
				strcat(str, _pair[1]);
				strcat(str, "]");
				strcat(str, " ");
			}
			else
			{
				strcat(str, "[");
				strcat(str, _pair[0]);
				strcat(str, " ");
				strcat(str, _pair[1]);
				strcat(str, "]");
				strcat(str, " ");
			}
		}
		token = strtok(NULL, "[]");
	}

	int len = strlen(str);
	int i;
	while (len > 0 && (isspace(str[len - 1]) || str[len - 1] == '\t')) {
		len--;
	}
	str[len] = 0;

	for (i = 0; str[i]; i++)
	{
		if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
		{
			fputc(str[i], p5);
		}
	}

	fclose(p4);
	fclose(p5);
}

void _search(char* name, char* scope)
{

}

void _insert(SymbolTable* st)
{

}

void _update(char* name, char* scope, char* value)
{

}

void _delete(char* name, char* scope)
{

}

void _display()
{
	int i = 0;
	printf("SI.No\t\tName\t\tId Type\t\tData Type\tScope\t\tValue\n");
	for (int i = 0; i < count; i++)
	{
		printf("%d\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n", i + 1, table[i].name, table[i].id_type, table[i].data_type, table[i].scope, table[i].value);
	}
}

void generateSymbolTable(const char* fileName)
{
	FILE* p;

	p = fopen(fileName, "r");
	if (!p)
	{
		return;
	}

	char buffer[1024];

	int elements_count = 0;

	while (fgets(buffer, sizeof(buffer), p) != NULL)
	{
		char* start = strchr(buffer, '[');
		while (start)
		{
			char* end = strchr(start + 1, ']');
			if (!end)
			{
				break;
			}

			int element_length = end - start - 1;
			char* element = (char*)malloc(element_length + 1);
			if (element)
			{
				strncpy(element, start + 1, element_length);
				element[element_length] = '\0';
			}
			elements[elements_count++] = element;

			start = strchr(end + 1, '[');
		}
	}


	/*printf("Extracted elements:\n");
	for (int i = 0; i < elements_count; i++)
	{
		printf("%s\n", elements[i]);
	}*/

	for (auto& i : table)
	{
		i = initSymbolTable();
	}

	SymbolTable st1 = initSymbolTable();
	char curr_scope[] = "global";

	for (int i = 0; i < elements_count; i++)
	{
		char** tok;
		tok = str_split(elements[i], ' ');
		/*if (strcmp(tok[0], "{") == 0)
		{
			strcpy(scope, st1.scope);
		}
		if (strcmp(tok[0], "id") == 0)
		{
			if (strcmp(elements[i + 1], "(") == 0)
			{
				st1.slNo = count + 1;
				st1.name = tok[1];
				st1.id_type = (char*)"func";
				st1.scope = scope;

			}
		}*/
	}

	for (int i = 0; i < elements_count; i++)
	{
		free(elements[i]);
	}
	fclose(p);
}

void printContent(const char* fileName)
{
	FILE* p;
	char c;

	p = fopen(fileName, "r");
	if (!p)
	{
		perror("File can't be opened");
		return;
	}

	printf("Content of %s:\n", fileName);
	while ((c = fgetc(p)) != EOF)
	{
		printf("%c", c);
	}
	printf("\n");

	fclose(p);
}

int main()
{
	char input_dir[] = "input1.c";
	char intermediate1_dir[] = "intermediate1.txt";
	char intermediate2_dir[] = "intermediate2.txt";
	char intermediate3_dir[] = "intermediate3.txt";
	char intermediate4_dir[] = "intermediate4.txt";

	removeComments(input_dir, intermediate1_dir);

	separateLexemes(intermediate1_dir, intermediate2_dir);

	identifyLexemes(intermediate2_dir, intermediate3_dir);

	editPairs(intermediate3_dir, intermediate4_dir);

	printContent(input_dir);
	printContent(intermediate1_dir);
	printContent(intermediate2_dir);
	printContent(intermediate3_dir);
	printContent(intermediate4_dir);

	generateSymbolTable(intermediate4_dir);

	return 0;
}

