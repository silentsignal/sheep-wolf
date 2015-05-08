// shepherd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rc4.h"
#include<iostream>
#include<fstream>
#include<string>

int main(int argc, char* argv[])
{
	if (argc<3){
		printf("Usage: %s [KEY] [shellcode.raw]\n");
		return -1;
	}

	unsigned char* key=(unsigned char*)argv[1];
	std::ifstream in(argv[2]);
	
	std::string contents((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
	
	rc4_key_t rc4_key;
	rc4_set_key(key,strlen((const char*)key),&rc4_key);
	unsigned char* crypted=(unsigned char*)malloc(contents.length()*sizeof(unsigned char));
	memset(crypted,0,contents.length()*sizeof(unsigned char));
	rc4_crypt(crypted,contents.length(),&rc4_key);
	printf("#define SC \"");
	for (int i=0;i<contents.length();i++){
		printf("\\x%02x",(unsigned char)((unsigned char)(contents.c_str()[i])^crypted[i]));
	}
	printf("\"");
	return 0;
}

