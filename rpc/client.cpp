#include <iostream>
#include <rpc/rpc.h>
#include <unistd.h>
#include "prog.h"
#include <vector>
#include <fstream>
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
        cout<<data<<'\n';
        total_bytes += read_bytes;
        //cout<<"asda\n";
        cout<<data<<'\n';
        memcpy(chunk.data, data, read_bytes);
        chunk.bytes = read_bytes;
        String* res = sendfile_1(&chunk, clnt);
        char* str = res->String_val;
        string parsed(str);
        cout<<parsed<<'\n';
 
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
    String *s;
    s = (String *)malloc(sizeof(String));
    s->String_val = &t[0];
    s->String_len = t.length() * sizeof(char); 
    void *h;
    String * print = sendstrings_1(s, Client);
    cout << "String: " << print->String_val << endl;
    return 0;
}
