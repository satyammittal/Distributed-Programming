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
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
 
#define CLEAR "\033[2J"  // clear screen escape code 
extern __thread int errno;
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
    //printf ("Splitting string \"%s\" into tokens:\n",command);
    pch = strtok (command," -");
    vector<string> arg;
    int flag=0;
    while (pch != NULL)
    {
        //cout<<pch<<'\n';
        string str1(pch);
       
        arg.push_back(str1);
        pch = strtok (NULL, " -");
    }
    return arg;
}
int put_file(char *host, string command)
{
    CLIENT *clnt;
    char data[1024];
    int total_bytes = 0, read_bytes;
    int *result;
    chunksend chunk;
    FILE *file;
    vector<string> arg = get_file_name(command);
    int param = arg.size();
    int flag=0;
    string pattern = arg[param-2];
    string grep_file = arg[param-1];
    string str1 = arg[param-3];
    //cout<<"here is "<<str1<<'\n';
    if(str1.compare("i")==0)
    {
        flag=1;
        //cout<<"here is "<<str1<<flag<<'\n';
    }
    else if (str1.compare("c")==0){
        flag=2;
       // cout<<"here is "<<str1<<flag<<'\n';
    }
    else{
        flag=0;
       // cout<<"here is "<<str1<<flag<<'\n';
    }
    String *pat;
    pat = (String *)malloc(sizeof(String));
    int l=pattern.length();
    pat->String_val = (char *)malloc(sizeof(char) * 1024);
    strncpy(pat->String_val, pattern.c_str(), sizeof(char)*l); 
    pat->String_len = l * sizeof(char);

    //cout << ->String_val << endl;
    //cout<< S->String_val << endl;
    //cout<<l<<'\n';
    
    ifstream infile(grep_file.c_str());
    //cout<<grep_file.c_str()<<1<<'\n';
    string line;
    //cout<<file.length()<<'\n';
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
    while (1) {
        //cout<<"asda\n";
        read_bytes = fread(data, 1, MAXLEN, file);
        //cout<<"asda\n";
        //cout<<data<<'\n';
        total_bytes += read_bytes;
        //cout<<"asda\n";
        //cout<<data<<'\n';
        memcpy(chunk.data, data, read_bytes);
        chunk.bytes = read_bytes;
        chunk.pattern.String_val = pat->String_val;
        chunk.pattern.String_len = pat->String_len;
        chunk.flag = flag;
        String* res = sendfile_1(&chunk, clnt);
        char* str = res->String_val;
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
            token = strtok (NULL,"\n");
            continue;
        }
        //cout<<parsed<<'\n';
        //std::cout << CLEAR;
        for(int i=0;i<parsed.length();i++){
                if(parsed[i]=='{')
                {
                    cout<<GREEN;
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
        if(flag==2)
            cout<<count<<'\n';
        if (result == NULL) {
            /*
             * An RPC error occurred while calling the server.
             * Print error message and stop.
             */
            clnt_perror(clnt, host);
            exit(1);
        }

        /*
         * Okay, we successfully called the remote procedure.
         */
        if (*result != 0) {
            /*
             * A remote system error occurred.
             * Print error message and stop.
             */
            errno = *result;
            perror(name);
            exit(1);
        }

        /*
         * Successfully got a chunk of the file.
         * Write into our local file.
         */
        if (read_bytes < MAXLEN) 
            break;
    }

    fclose(file);

    return 0;
}

int main(int argc, char *argv[])
{
    CLIENT *Client;
    intpair Pair;
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
    //string file = get_file_name(t);
    //char *command = new char[file.length() + 1];
    //const char *cstar = file.c_str();
    //strcpy(command, cstar);
    put_file(host,t);
    return 0;
}
