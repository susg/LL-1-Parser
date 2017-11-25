#include <stdio.h>
#include <string.h>

//Storing the tokens
struct map
{
	char type[100], lexme[100];
	int id, x, y;
}TOKENS[500];

//Symbol table
struct symbol_table
{
	char type[100], lexme[100], dtype[100];
	int sc, x,y, ref_x, ref_y;
}SYM_TAB[500];

//Global variables to store scope and indexes to array
int count=-1, scope = 0, ctr = 0;
char prev_id[100];

//Declaring the keywords and operators globally
char keyword[][100] = {
	{ "else" }, { "if" } , {"for"}, {"int"}, {"float"}, {"void"}, {"return"}
};

char arop[][2] = { {"+"}, {"-"}, {"*"}, {"/"} };
char relop[][2] = { {"<"}, {">"} };
char punc[][2] = { {";"},  {","},  {"("},  {")"},  {"{"},  {"}"} };

//Function to check if a string is a keyword
int check_keyword(char token[])
{
	for(int i=0; i<7; i++)
	{
		if(strcmp(token, keyword[i]) == 0)
			return 1;
	}
	return 0;
}

//Function to check if a string is an arithmetic operator
int check_arop(char token[])
{
	for(int i=0; i<4; i++)
	{
		if(strcmp(token, arop[i]) == 0)
			return 1;
	}
	return 0;
}

//Function to check if a string is a relational operator
int check_relop(char token[])
{
	for(int i=0; i<2; i++)
	{
		if(strcmp(token, relop[i]) == 0)
			return 1;
	}
	return 0;
}

//Function to check if a string is a punctuator
int check_punc(char token[])
{
	for(int i=0; i<6; i++)
	{
		if(strcmp(token, punc[i]) == 0)
			return 1;
	}
	return 0;
}

//Function to check if a string is a constant
int check_const(char token[])
{	
	int len = strlen(token);
	for(int i=0; i<len; i++)
	{
		if( !(token[i] >= '0' && token[i] <= '9'))
			return 0;
	}
	return 1;
}

//Function to check the type of token and store it 
void tokenize(char token[], int line, int col, FILE *fp3)
{
	count++;
	TOKENS[count].id = (count+1)*5;
	strcpy( TOKENS[count].lexme, token );
	TOKENS[count].x = line;
	TOKENS[count].y = col;

	if(check_keyword(token))
	{
		strcpy(TOKENS[count].type,"Keyword");
		fprintf(fp3,"%s ", token);
	}
	else if(token[0] == '=')
	{
		strcpy(TOKENS[count].type,"Assignment OP");
		fprintf(fp3,"%s ", token);
	}
	else if(check_arop(token))
	{
		strcpy(TOKENS[count].type,"Arithmetic OP");
		fprintf(fp3,"%s ", token);
	}

	else if(check_relop(token))
	{
		strcpy(TOKENS[count].type,"Relational OP");
		fprintf(fp3,"%s ", token);
	}

	else if(check_punc(token))
	{
		strcpy(TOKENS[count].type,"Punctuator");
		fprintf(fp3,"%s ", token);
	}
	else if(check_const(token))
	{
		strcpy(TOKENS[count].type,"Constant");
		fprintf(fp3,"numbers ");
	}
	else
	{
		strcpy(TOKENS[count].type,"Identifier");
		if(strcmp(token, "main")==0)
			fprintf(fp3,"main ");
		else if(strcmp(token, "read")==0)
			fprintf(fp3,"read ");
		else if(strcmp(token, "write")==0)
			fprintf(fp3,"write ");
		else		
		{
			fprintf(fp3,"identifiers ");
			if(strcmp(prev_id, "int") == 0 || strcmp(prev_id, "float") == 0)
			{
				strcpy(SYM_TAB[ctr].type, "Identifier");
				strcpy(SYM_TAB[ctr].lexme, token);
				strcpy(SYM_TAB[ctr].dtype, prev_id);
				SYM_TAB[ctr].x = line;
				SYM_TAB[ctr].y = col;
				SYM_TAB[ctr].sc = scope;
				ctr++;
			}
			else
			{
				int err = 1;
				for(int i=ctr; i>=0; i--)
				{
					if(strcmp(token, SYM_TAB[i].lexme) == 0 && SYM_TAB[i].sc <= scope)
					{
						SYM_TAB[i].ref_x = line;
						SYM_TAB[i].ref_y = col;
						err = 0;
						break;
					}
				}
				// Variable out of scope
				if(err)
				{
					printf("ERROR IN CODE. VARIABLE %s OUT OF SCOPE\n", token);
				}

			}
		}	
	}	
}

//Function to write output to file
void display()
{
	FILE *fp2 = fopen("tokens", "w");
	
	//Writing output to file
	fprintf(fp2, "\nTotal tokens = %d\n\n", count+1);
	fprintf(fp2, "TOKEN ID\t  TOKEN TYPE\tLEXME\t POSITION\n");
	for(int i=0; i<=count; i++)
	{
		fprintf(fp2, "%3d %20s\t%5s\t   (%d, %d)\n", TOKENS[i].id, TOKENS[i].type, TOKENS[i].lexme, TOKENS[i].x, TOKENS[i].y);
	}
	fclose(fp2);
}

//Function to display symbol table after every '}' is encountered
void display_symtab(FILE *fp4)
{
	fprintf(fp4, "CURRENT SCOPE = %d\n", scope);
	for(int i=0; i< ctr; i++)
	{
		if(SYM_TAB[i].sc != 1000)
		{
			fprintf(fp4, "\nName = %s\nType = %s\nData type = %s\nScope = %d\nPosition = %d, %d\nReferenced in : %d, %d\n",
			SYM_TAB[i].lexme, SYM_TAB[i].type,SYM_TAB[i].dtype ,SYM_TAB[i].sc, SYM_TAB[i].x, SYM_TAB[i].y,
			SYM_TAB[i].ref_x, SYM_TAB[i].y);
		}
		if(SYM_TAB[i].sc == scope)
			SYM_TAB[i].sc = 1000;
	}
	fprintf(fp4, "\n\n");
}

//main function
int main()
{
	//Opening input file
	FILE *fp = fopen("input.c", "r");
	FILE *fp3 = fopen("output_prog", "w");
	FILE *fp4 = fopen("symbol_table", "w");
	char c, buff[100], g;

	int i=0, line=1, col=0, flag = 0, comnt = 0;

	while(1)
	{
		if(!flag)
		{
			c = fgetc(fp);

			//If end of file then return
			if(c == EOF)
				break;
		}
		else
			c = g;		

		col++;

		//If there is a space or new line then tokenize the string found till here
		if(c == ' ' || c == '\n' || c == '\t')
		{
			if( i>0)
			{
				buff[i] = '\0';
				tokenize(buff, line, col, fp3);
				strcpy(prev_id, buff);
				i=0;
			}	
			if(c == '\n')
			{
				line++;
				col=0;
				fprintf(fp3,"\n");
			}
			continue;
		}

		//Checking for comments
		if(c == '/')
		{
		    g = fgetc(fp);

		    //Removing single line comment
			if(g == '/')
			{
			    do
			    {
			    	c = fgetc(fp);
			    }while(c != '\n');
			    flag = 0;
			    comnt = 1;
			}

			//Removing multiple lines comment
			else if(g == '*')
			{
				do
				{
					c = fgetc(fp);
					g = fgetc(fp);
				}while( c!= '*' && g != '/');
				flag = 0;
				comnt=1;
			}

			//If not a comment then continue
			else
			{
				flag = 1;
				comnt = 0;
			}	
		}    	

		char temp[3];
		temp[0]=c;
		
		if(!comnt) 
		{
			//Checking for operator or punctuator
			if(check_punc(temp) || check_relop(temp) || check_arop(temp) || temp[0] == '=')
			{
				//Tokenizing the string found before the operator
				if(i>0)
				{
					buff[i] = '\0';
					tokenize(buff, line, col, fp3);
					col++;
					strcpy(prev_id, buff);
				}

				//Tokenizing the operator
				tokenize(temp, line, col, fp3);
				strcpy(prev_id, buff);
				
				//increasing the scope
				if(temp[0] == '{')
					scope++;

				//decreasing the scope as well as displaying the symbol table
				else if(temp[0] == '}')
				{
					display_symtab(fp4);
					scope--;
				}	
				i=0;
				continue;
			}

			//Checking for string literal
			if(c == '"')
			{
				do
				{
					buff[i++] = c;
					c = fgetc(fp);	
				}while(c!= '"');

				//Tokenizing the string literal
				buff[i++]=c;
				buff[i]='\0';
				count++;
				TOKENS[count].id = (count+1)*5;
				strcpy( TOKENS[count].lexme, buff );
				TOKENS[count].x = line;
				TOKENS[count].y = col;
				strcpy(TOKENS[count].type,"String Literal");
				i=0;
				strcpy(prev_id, "string");
				continue;

			}
			buff[i++] =  c;
		
		}
		comnt=0;
	}
	
	//Write the output in file
	display();

	fclose(fp);
	fclose(fp3);
	fclose(fp4);
	return 0;
}
