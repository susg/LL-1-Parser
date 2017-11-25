/*
	1. Epsilon is represented by "Epsilon" in first and follow . 
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
using namespace std;

// Global variables to store terminals, non-terminals, parser table, first and follow
vector <string> production, non_terminals, terminals, parser_table[100];
map <  string,  vector< pair<string, int> > > first1;
map< string, vector< string> > follow;
map<string, int> vis, vis2;
int no_of_rules;

//Function to tokenize the string
pair<string, int> tokenize(string str, int idx, char delim)
{
	string token = "";
	if(idx >= str.size())
		return (make_pair(token, -2));
	for( ; idx < str.size(); idx++)
	{
		if(str[idx] == delim)
			return (make_pair(token, idx+1));
		token += str[idx];
	}
	return ( make_pair(token, -1)) ;
}

//function to check if it is terminal
int check_terminal(string a)
{
	for(int i=0; i< terminals.size(); i++)
		if(a == terminals[i])
			return 1;

	return 0;	
}

//recursive function to find first
void find_first(string non_terminal, int idx)
{
	pair<string, int> token = tokenize(production[idx], 0, '-');
	token.second++;
	int cnt = 1;
	do
	{
		token = tokenize(production[idx], token.second, '|');
		
		int flag =0;
		pair<string, int> term = make_pair("", 0);
		do
		{
			term = tokenize(token.first, term.second, ' ');
			if(term.second == -2)
				break;
			if(check_terminal(term.first) || term.first == "Epsilon")
			{
				first1[non_terminal].push_back( make_pair( term.first, cnt));
				break;
			}
			else 
			{
				int pos;
				for(int i=0; i<non_terminals.size(); i++)
					if(term.first == non_terminals[i])
					{
						pos = i;
						break;
					}	
				if(!vis[term.first])
					find_first(term.first, pos);
				for(int i=0; i< first1[term.first].size(); i++)
				{
					flag = 0;
					if(first1[term.first][i].first != "Epsilon")
						first1[non_terminal].push_back( make_pair(first1[term.first][i].first, cnt) );
					else
					{
						flag =1;
						if(term.second == -1)
						{
							first1[non_terminal].push_back( make_pair("Epsilon", cnt));
							flag = 0;				
						}
					}
				}	
			}
			if(!flag)
				break;
		}while(term.second != -1);
		cnt++;	
	}while(token.second != -1);
		
	vis[non_terminal] = 1;
}

//function to check if element is not visited
bool not_visit(string a, string b)
{
	for(int i=0; i<follow[a].size(); i++)
		if(follow[a][i] == b)
			return false;

	return true;	
}

//recursive function to find follow
void find_follow(string non_terminal)
{
	
	for(int i=0; i<no_of_rules; i++)
	{
		pair<string, int> token = tokenize(production[i], 0, '-');
		string par= token.first;
		token.second++;

		do
		{
			token = tokenize(production[i], token.second, '|');
			if(token.first == "Epsilon")
				break;
			int flag = 0;
			pair<string, int> term = make_pair("", 0);
			do
			{
				
				term = tokenize(token.first, term.second, ' ');
				

				if(term.second == -2)
					break;
				if(check_terminal(term.first) && !flag)
					continue;
				else if(check_terminal(term.first) && flag)
				{
					if(not_visit(non_terminal, term.first))
						follow[non_terminal].push_back(term.first);
					break;
				}
				
				if(term.first == non_terminal)
				{
					
					if(term.second == -1)
					{
						
						if(par == non_terminal)
							break;
						if(!vis[par])
							find_follow(par);
						for(int k=0; k< follow[par].size(); k++)
							if(not_visit(non_terminal, follow[par][k]))
								follow[non_terminal].push_back(follow[par][k]);
						break;
					}
					else
						flag =1;
				}
				else if(flag)
				{
					flag=1;
					for(int ii=0; ii< first1[term.first].size(); ii++)
					{
						flag = 0;
						if(first1[term.first][ii].first != "Epsilon" && not_visit(non_terminal, first1[term.first][ii].first))
							follow[non_terminal].push_back( first1[term.first][ii].first );
						else
						{
							flag =1;
							
							if(term.second == -1)
							{
								if(!vis[par])
									find_follow(par);
								for(int k=0; k< follow[par].size(); k++)
									if(not_visit(non_terminal, follow[par][k]))
										follow[non_terminal].push_back(follow[par][k]);
								flag = 0;				
							}
						}
					}
				}
				

			}while(term.second != -1);
		
		}while(token.second != -1);	
			
	}
	
	vis[non_terminal] = 1;
}

string fill_table(int idx, int x)
{
	
		pair<string, int> token = tokenize(production[idx], 0, '-');
		token.second++;
		int cnt = 1;
		do
		{
			token = tokenize(production[idx], token.second, '|');
			
			if(cnt == x)
				return token.first;
			cnt++;
		}while(token.second != -1);
	
		return NULL;
}

int main()
{
	ifstream file;
	ifstream file2;
	file.open("grammar.txt");
	
	string line;
	cout<<endl;

	//Reading from input file and store it
	while ( getline (file,line) ) 
	{
		production.push_back(line);
	}
	no_of_rules = production.size();
	
	//storing all the non terminals
	pair<string, int> token;
	for(int i=0; i< no_of_rules; i++)
	{
		token = tokenize(production[i], 0, '-');
		non_terminals.push_back( token.first );
	}

	//storing all the terminals 
	for(int i=0; i< no_of_rules; i++)
	{
		pair<string, int> token = tokenize(production[i], 0, '-');
		token.second++;

		do
		{
			token = tokenize(production[i], token.second, '|');
			
			pair<string, int> term = make_pair("", 0);
			do
			{
				term = tokenize(token.first, term.second, ' ');
				if(term.second == -2)
					break;
				int f=0;
				for(int i=0; i<non_terminals.size(); i++)
					if(term.first == non_terminals[i])
					{
						f=1; 
						break;
					}	

				if(!f && term.first != "Epsilon" && !check_terminal(term.first))
					terminals.push_back(term.first);	
			}while(term.second != -1);
		}while(token.second != -1);
	}
	
	//finding first for the grammar
	for(int i=0; i< no_of_rules; i++)
	{
		if( !vis[non_terminals[i]] )
			find_first(non_terminals[i], i) ;
	}	
	vis.clear();	
	
	//find follow for the grammar
	follow[non_terminals[0]].push_back("$");
	for(int i=0; i< no_of_rules; i++)
	{
		if(!vis[non_terminals[i]])
			find_follow(non_terminals[i]);
		
	}
	
	map< string, vector< pair <string, int> > > :: iterator itr;

	//Uncomment to display first

	/*
	for(itr = first1.begin(); itr!= first1.end(); itr++)
	{
		cout<<"FIRST( "<< itr->first<<" ) = { ";
		for(int i= 0; i< itr->second.size()-1; i++)
			cout<<itr->second[i].first<<", ";
		cout<<itr->second[ itr->second.size()-1 ].first << " }";
		
		cout<<endl;
	}
	*/
	cout<<endl;
	map< string, vector< string> > :: iterator itr2;
	
	// Uncomment to display follow

	/*
	for(itr2 = follow.begin(); itr2 != follow.end(); itr2++)
	{
		
		cout<<"FOLLOW( "<<itr2->first<<" ) = { ";
		for(int i= 0; i< itr2->second.size()-1; i++)
			cout<<itr2->second[i]<<", ";
		cout<<itr2->second[ itr2->second.size()-1 ] <<" }";
		
		cout<<endl;
	}
	*/
	
	//creating the parser table
	terminals.push_back("$");
	for(int i=0; i< no_of_rules; i++)
	{
		int has_epsi = 0;
		
		for(int j=0; j< terminals.size(); j++)
		{
			int f=0;
			for(int k=0; k< first1[non_terminals[i]].size(); k++)
			{
				
				if(first1[non_terminals[i]][k].first == terminals[j])
				{	
					string y = fill_table(i, first1[non_terminals[i]][k].second);
					
					parser_table[i].push_back(y);
					f=1;
					
				}
				if(first1[non_terminals[i]][k].first == "Epsilon")
					has_epsi = 1;
			}
			if(has_epsi && !f)
			{
				for(int k = 0; k< follow[non_terminals[i]].size(); k++)
				{
					if(follow[non_terminals[i]][k] == terminals[j])
					{
						parser_table[i].push_back("Epsilon"); 
						f=1;
						break;
					}	
				}
			}	
			if(!f)
				parser_table[i].push_back(" ");
		}
		
	}

	//Uncomment to display the parsre table
	/*
	cout<<"\n\nPARSER TABLE\n\n";  
	cout<<endl;
	
	for(int i=0; i< no_of_rules; i++)
	{
		cout <<endl << non_terminals[i] << endl;
		for(int j=0; j< terminals.size(); j++)
			cout<< terminals[j] << " -> " << parser_table[i][j]<<endl;
	}
	*/

	string str;
	cout<<"\n";
	
	
	//taking prog as input
	file2.open("output_prog");
	ofstream f3;
	f3.open("output");

	std::vector<string> program;

	while ( getline (file2,str) ) 
	{
		program.push_back(str);
	}

	program.push_back("$");
	vector<string> stck;
	stck.push_back("$");
	stck.push_back(non_terminals[0]);
	int i=0, err = 0;
	while(i < production.size() && stck.size() > 0)
	{
		pair<string, int> term = make_pair("", 0);
		term = tokenize(program[i], term.second, ' ');
		do
		{
			f3 << "\nToken to check = "<< term.first << " " << endl;
			string y = stck.back();
			f3 << "Top of stack = " << y << endl;
			stck.pop_back();
			if(check_terminal(y) || y[0] == '$')
			{
				if(y == term.first)
				{
					term = tokenize(program[i], term.second, ' ');
					if(term.second == -2 )
					{
						break;
					}
					continue;
				}
				else
				{
					err=1;
					cout<<"Error\n";
					break;
				}

			}
			int j,k;
			for( k=0; k<terminals.size(); k++)
				if(terminals[k] == term.first)
					break;
			for( j=0; j<non_terminals.size(); j++)
				if(non_terminals[j] == y)
					break;
			string z = parser_table[j][k];
			if(z == " ")
			{
				err=1;
				break;
			}
			if(z == "Epsilon")
				continue;
			string ss="";
			std::vector<string> temp;
			for(j= 0; j < z.size(); j++)
			{
				if(z[j] == ' ')
				{
					temp.push_back(ss);
					ss = "";
				}	
				else
					ss += z[j];
			}
			temp.push_back(ss);
			for(j= temp.size()-1; j>=0; j--)
			{
				stck.push_back(temp[j]);
			}	
		}while(term.second != -2);
		i++;
		if(err)
			break;
	}
	
	if(!err)
		f3<<"\nINPUT C PROGRAM IS CORRECT\n";
	else
		f3<<"\nINPUT C PROGRAM IS INCORRECT\n";
	
	//close all open files
	file.close();
	file2.close();
	return 0;
}		







