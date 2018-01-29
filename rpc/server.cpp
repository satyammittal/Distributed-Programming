/***********************************/
/*                                 */
/* server.cpp                      */
/* Handles all requests of clients */
/* get the file data and pattern   */
/* and findout the matched pattern */
/* and return the data highlighting*/
/* pattern by {}                   */
/*                                 */
/***********************************/

#include <iostream>
#include <rpc/rpc.h>
#include "prog.h"
#include <vector>
#include <fstream>
#include<sstream>
#include<algorithm>
using namespace std;

string find_grep(char* t, string pattern, int flag)
{
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
	String pat = rec->pattern;
	string pattern(pat.String_val);
	string r1 = find_grep(rec->data,pattern,rec->flag);
	String *tosend;
	tosend = (String *)malloc(sizeof(String));
	int l=r1.length();
	tosend->String_val = (char *)malloc(sizeof(char) * 1024);
	strncpy(tosend->String_val, r1.c_str(), sizeof(char)*l); 
	//cout << tosend->String_val << endl;
	//cout<<l<<'\n';
	tosend->String_len = l * sizeof(char);
	return tosend;
}
