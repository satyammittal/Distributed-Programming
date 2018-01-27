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

int put_file(char *host, char *name)
{
    CLIENT *clnt;
    char data[1024];
    int total_bytes = 0, read_bytes;
    int *result;
    chunksend chunk;
    FILE *file;

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
    cout<<name<<'\n';
    chunk.name = name;
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
        String* res = sendfile_1(&chunk, clnt);
        char* str = res->String_val;
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
        int len_pattern = 5;
        int j=0;
        int u = str[0];
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
string get_file_name(string s1)
{
    string str=s1;
    int l = s1.length();
    char *command = new char[s1.length() + 1];
    const char *cstar = str.c_str();
    strcpy(command, cstar);
    char * pch;
    printf ("Splitting string \"%s\" into tokens:\n",command);
    pch = strtok (command," -");
    vector<string> arg;
    int flag=0;
    while (pch != NULL)
    {
        cout<<pch<<'\n';
        string str1(pch);
        if(str1.compare("i"))
        {
            flag=1;
        }
        else{
            flag=2;
        }
        arg.push_back(str1);
        pch = strtok (NULL, " -");
    }
    int is_flag=0;
    if(flag!=0)
        is_flag=1;
    string pattern = arg[1+is_flag];
    string file = arg[2+is_flag];
    ifstream infile(file.c_str());
    cout<<file.c_str()<<1<<'\n';
    string line;
    cout<<file.length()<<'\n';
    int line_number=0;
    if (!infile.is_open())
    perror("error while opening file");
    else
    return file;
    return "ERROR";
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
    string file = get_file_name(t);
    char *command = new char[file.length() + 1];
    const char *cstar = file.c_str();
    strcpy(command, cstar);
    put_file(host,command);
    return 0;
}
