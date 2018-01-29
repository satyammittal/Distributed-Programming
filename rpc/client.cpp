/***********************************/
/*                                 */
/* client.cpp                      */
/* implement rgrep command which is*/
/* like unix grep command          */
/* rgrep -<flag> <pattern> <file>  */
/* Implemented 2 flags:            */
/* -c : counts the  occurences     */
/* -i : ignore patter cases        */
/*                                 */
/***********************************/
#include <iostream>
#include <rpc/rpc.h>
#include <unistd.h>
#include "prog.h"
#include <vector>
#include <fstream>
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#define CLEAR "\033[2J"  // clear screen escape code 

using namespace std;
char* cast_to_char(string file)
{
	char *command = new char[file.length() + 1];
	const char *cstar = file.c_str();
	strcpy(command, cstar);
	return command;
}
vector<string> get_file_name(string s1)
{
	string str=s1;
	int l = s1.length();
	char *command = new char[s1.length() + 1];
	const char *cstar = str.c_str();
	strcpy(command, cstar);
	char * pch;
	pch = strtok (command," -");
	vector<string> arg;
	int flag=0;
	while (pch != NULL)
	{
		string str1(pch);

		arg.push_back(str1);
		pch = strtok (NULL, " -");
	}
	return arg;
}
int put_file(char *host, string command)
{
	CLIENT *clnt;
	char data[1025];
	int total_bytes = 0, read_bytes;
	int *result;
	chunksend chunk;
	FILE *file;
	vector<string> arg = get_file_name(command);
	if(arg.size()<3 || arg.size()>4 ||  arg[0].compare("rgrep")!=0)
	{
		cout<<"format: rgrep -<flag> <pattern> <file>\n";
		return 1;
	}
	int param = arg.size();
	int flag=0;
	string pattern = arg[param-2];
	string grep_file = arg[param-1];
	string str1 = arg[param-3];
	if(str1.compare("i")==0)
	{
		flag=1;
	}
	else if (str1.compare("c")==0){
		flag=2;
	}
	else{
		flag=0;
	}
	String *pat;
	pat = (String *)malloc(sizeof(String));
	int l=pattern.length();
	pat->String_val = (char *)malloc(sizeof(char) * 1024);
	strncpy(pat->String_val, pattern.c_str(), sizeof(char)*l); 
	pat->String_len = l * sizeof(char);

	ifstream infile(grep_file.c_str());
	string line;
	int line_number=0;
	if (!infile.is_open())
	{
		perror("error while opening file");
		return 0;
	}
	char* name=cast_to_char(grep_file);
	/*
	 * Create client handle used for calling FTPPROG on
	 * the server designated on the command line. Use
	 * the tcp protocol when contacting the server.
	 */
	clnt = clnt_create(host, MATHPROG, MATHVERS, "tcp");
	if (clnt == NULL) {
		/*
		 * Couldn't establish connection with server.
		 * Print error message and stop.
		 */
		clnt_pcreateerror(host);
		exit(1);
	}

	file = fopen(name, "r");
	//cout<<name<<'\n';
	chunk.name = name;
	int count=0;
	/*
	 * Call the remote procedure readdir on the server
	 */
	string last_part="";
	while (1) {
		//cout<<"asda\n";
		read_bytes = fread(data, 1, MAXLEN/2, file);
		data[read_bytes]='\0';
		read_bytes+=1;
		total_bytes += read_bytes;
		memcpy(chunk.data, data, read_bytes);
		chunk.bytes = read_bytes;
		chunk.pattern.String_val = pat->String_val;
		chunk.pattern.String_len = pat->String_len;
		chunk.flag = flag;
		String* res = sendfile_1(&chunk, clnt);
		if(res==NULL)
		{
			clnt_perror(clnt, host);
			exit(1);
		}
		char* y = res->String_val;
		string r(y);
		r=last_part + r;
		char* str= cast_to_char(r);
		if(flag==2)
		{
			int u=atoi(str);
			count+=u;
		}else{
			char* token;
			token = strtok (str,"\n");
			while(token != NULL)
			{
				string parsed(token);
				if(parsed.find('{')==string::npos)
				{

					last_part = token;
					token = strtok (NULL,"\n");
					continue;
				}
				for(int i=0;i<parsed.length();i++){
					if(parsed[i]=='{')
					{
						cout<<BOLDRED;
					}
					else if(parsed[i]=='}')
					{
						cout<<RESET;
					}
					else{
						cout<<parsed[i];
					}
				}
				cout<<'\n';
				token = strtok (NULL,"\n");

			}
		}

		/*
		 * Successfully got a chunk of the file.
		 * Write into our local file.
		 */
		if (read_bytes < MAXLEN/2) 
			break;
	}

	fclose(file);
	if(flag==2)
		cout<<count<<'\n';

	return 0;
}

int main(int argc, char *argv[])
{
	CLIENT *Client;
	int *result;
	char host[256];

	gethostname(host, 256);

	Client = clnt_create(host, MATHPROG, MATHVERS, "udp"); //Create a handle for Client

	if(Client == NULL)
	{
		cout << "Error: Client not formed" << endl;
		exit(-1);
	}
	string t;
	getline(cin, t);
	put_file(host,t);
	return 0;
}
