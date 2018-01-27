#include <iostream>
#include <rpc/rpc.h>
#include "prog.h"
#include <vector>
#include <fstream>
#include<sstream>
#include<algorithm>
using namespace std;
extern __thread int errno;

int *add_1_svc(intpair *pair, struct svc_req *X)
{
    static int result;
    result = pair->a + pair->b;
    return(&result);
}

int *sub_1_svc(intpair *pair, struct svc_req *X)
{
    static int result;
    result = pair->a - pair->b;
    return(&result);
}


String *sendstring_1_svc(void *vd, struct svc_req *X)
{
    String *tosend;
    tosend = (String *)malloc(sizeof(String));
    tosend->String_val = (char *)malloc(sizeof(char) * 100);
    char to[27] = "Hello from the other size";
    strncpy(tosend->String_val, (char *)"hello from the other side", sizeof(char)*27); 
    cout << tosend->String_val << endl;
    tosend->String_len = 27 * sizeof(char);
    return tosend;
}


String *sendstrings_1_svc(String *S, struct svc_req *X)
{

    char* command = S->String_val;
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
    ifstream infile("index.html_server");
    cout<<file.c_str()<<1<<'\n';
    string line;
    cout<<file.length()<<'\n';
    int line_number=0;
    if (!infile.is_open())
   {
    perror("error while opening file");
   
        string u=file;
        FILE *file;
    char data[1024];
    int bytes;
    static readfile_res res;

    file = fopen(u.c_str(), "r");
    if (file == NULL) {
        cout<<"asdds22"<<'\n';
    }
    cout<<"yy1"<<'\n';
    //fseek (file, req->start, SEEK_SET);
    bytes = fread(res.readfile_res_u.chunk.data, 1, 1024, file);

    //res.readfile_res_u.chunk.data = data;
    res.readfile_res_u.chunk.bytes = bytes;
    cout<<res.readfile_res_u.chunk.data<<'\n';
    /*
     * Return the result
     */
    res.errno = 0;
    fclose(file);
    }

    while (getline(infile, line, '\n'))
    {
        cout<<line<<'\n';
        size_t nPos = line.find(pattern, 0);
        while(line.find(pattern)!=string::npos)
        {
            int pos = line.find(pattern);
            nPos = line.find(pattern, nPos+1);
            cout<<line_number<<' '<<pos<<'\n';
        }
        line_number++;
    }
    cout<<"yy2\n";
    if (infile.bad())
    perror("error while reading file");
    String *tosend;
    tosend = (String *)malloc(sizeof(String));
    tosend->String_val = (char *)malloc(sizeof(char) * 100);
    char to[27] = "Hello from the other size";
    strncpy(tosend->String_val, (char *)"hello from the other side", sizeof(char)*27); 
    cout << tosend->String_val << endl;
    cout<< S->String_val << endl;
    tosend->String_len = 27 * sizeof(char);
    return tosend;
}
readfile_res* retrievefile_1_svc(request *req, struct svc_req *rqstp)
{
    FILE *file;
    char data[1024];
    int bytes;
    static readfile_res res;

    file = fopen(req->name, "rb");
    if (file == NULL) {
        res.errno = errno;
        return (&res);
    }

    fseek (file, req->start, SEEK_SET);
    bytes = fread(res.readfile_res_u.chunk.data, 1, 1024, file);

    //res.readfile_res_u.chunk.data = data;
    res.readfile_res_u.chunk.bytes = bytes;

    /*
     * Return the result
     */
    res.errno = 0;
    fclose(file);
    return (&res);
}
string find_grep(char* t, string pattern, int flag)
{
    cout<<flag<<'\n';
    string line(t);
    string real_line = line;
    if(flag==1)
    {
            transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
            transform(line.begin(), line.end(), line.begin(), ::tolower);
    }
    size_t nPos = line.find(pattern, 0);
    int i=0;
    string s1="";
    int count=0;
    while(nPos!=string::npos)
    {
            s1 += real_line.substr(i, nPos-i);
            i = nPos+pattern.length();
            count++;
            s1 += "{"+real_line.substr(nPos, pattern.length())+"}";
            nPos = line.find(pattern, nPos+1);
    }
    s1+=real_line.substr(i);
    //s1+="\0";
    if(flag!=2)
    return s1;
    else{
        stringstream ss;
        ss<<count;
        string s1;
        ss>>s1;
        return s1;
    }
}
String *sendfile_1_svc(chunksend *rec, struct svc_req *rqstp)
{
    FILE *file;
    int write_bytes;
    static int result;
    cout<<"output-> "<<rec->name<<'\n';
    String pat = rec->pattern;
    string pattern(pat.String_val);
    string r1 = find_grep(rec->data,pattern,rec->flag);
    String *tosend;
    tosend = (String *)malloc(sizeof(String));
    int l=r1.length();
    tosend->String_val = (char *)malloc(sizeof(char) * 1024);
    strncpy(tosend->String_val, r1.c_str(), sizeof(char)*l); 
    cout << tosend->String_val << endl;
    //cout<< S->String_val << endl;
    cout<<l<<'\n';
    tosend->String_len = l * sizeof(char);
    //result = 0;
    return tosend;
}
