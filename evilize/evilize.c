/* Copyright (C) 2006 Peter Selinger. This file is distributed under
   the terms of the GNU General Public License. See the file COPYING
   for details. */

/* evilize.c: a program to create pairs of files with identical MD5
   hash, from a template. The template must contain two copies of a
   "crib" string defined in crib.h. The crib strings will be replaced,
   in two different ways, to create a "good" and an "evil" version of
   the file. In the "good" version, the two crib strings are replaced
   by the same data, in the "evil" version, they are replaced by
   different data. This is done in such a way that the "good" and
   "evil" files have the same MD5 hash. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <byteswap.h>

#include "md5.h"
#include "md5coll_lib.h"
#include "crib.h"

#define NAME "evilize"

static char *crib = CRIB;

#define LBUFSIZE 1024
#define FASTCOLL_CMD "..\\\\fastcoll\\\\fastcoll.exe -i %08x%08x%08x%08x -o a b"

/* write special purpose format file, compatible with md5coll. */
static void write_collision_file(FILE *fout, unsigned int m0[32], unsigned int m1[32]) {
  int i;
  
  fprintf(fout, "unsigned int m0[32] = {\n");
  for (i=0; i<32; i++) {
      fprintf(fout, "0x%08x, ", m0[i]);
      if ((i & 3) == 3) {
	fprintf(fout, "\n");
      }
  }
  fprintf(fout, "};\n\n");
  fprintf(fout, "unsigned int m1[32] = {\n");
  for (i=0; i<32; i++) {
    fprintf(fout, "0x%08x, ", m1[i]);
    if ((i & 3) == 3) {
      fprintf(fout, "\n");
    }
  }
  fprintf(fout, "};\n\n");
  return;
}

/* read special purpose format file, compatible with md5coll. Return
   number of items successfully read (should be 64 on success). */
static int read_collision_file(FILE *fin, unsigned int m0[32], unsigned int m1[32]) {
  char buf[100];
  int i = 0;
  int j = 0;
  unsigned int *m = m0;
  char *r;
  int c;

  while (j<64) {
    r = fgets(buf, 100, fin);
    if (r == NULL) {
      break;
    }
    c = sscanf(buf, "0x%x, 0x%x, 0x%x, 0x%x,", &m[i], &m[i+1], &m[i+2], &m[i+3]);
    if (c==4) {
      i+=4;
      j+=4;
      if (i==32) {
	i = 0;
	m = m1;
      }
    }
  }
  return j;
}

/* return allocated string, or NULL with errno set on error. */
static char *append_string(char *s1, char *s2) {
  char *s;

  s = (char *)malloc(strlen(s1)+strlen(s2)+1);
  if (!s) {
    return NULL;
  }
  strcpy(s, s1);
  strcat(s, s2);
  return s;
}

static void paste_swap(unsigned int *m, char *c) {
  int i;

  for (i=0; i<32; i++) {
    *c++ = m[i] & 0xff;
    *c++ = (m[i] >> 8) & 0xff;
    *c++ = (m[i] >> 16) & 0xff;
    *c++ = (m[i] >> 24) & 0xff;
  }
}

/* find the initial vector required to evilize FILE, return it in
   IV. Returns 0 on success, 1 if crib was not found. */
static int find_iv(FILE *fin, unsigned int IV[4]) {
  struct md5_ctx ctx;
  char buf[64];
  int r;
  int i;

  md5_init_ctx(&ctx);

  while (1) {
    r = fread(buf, 1, 64, fin);
    if (r < 64) {
      return 1;
    }
    for (i=0; i<64; i++) {
      if (memcmp(buf, crib+i, 64) == 0) {
	goto crib_found;
      }
    }
    md5_process_block(buf, 64, &ctx);
  }
 crib_found:
  /* find md5 collision for the given ctx initial value */

  IV[0] = ctx.A;
  IV[1] = ctx.B;
  IV[2] = ctx.C;
  IV[3] = ctx.D;
  return ftell(fin);
}

/* evilize the given file fin as fout, using the collision blocks m0
   and m1 previously found by md5coll. Replace first crib string using
   m0, and second one using m1. Return 0 on success, 1 if first crib
   not found, 2 if second crib not found. */
static int evilize(FILE *fin, FILE *fout, unsigned int m0[32], unsigned int m1[32]) {
  char lbuf[LBUFSIZE];
  int r;  /* number of bytes in lbuf */
  int i, j;

  r = 0;

  while (1) {
    r += fread(lbuf+r, 1, LBUFSIZE-r, fin); /* fill buffer */
    if (r<128) {
      return 1;
    }
    for (j=0; j<r-127; j+=64) {
      for (i=0; i<64; i++) {
	if (memcmp(lbuf+j, crib+i, 128) == 0) {
	  goto crib_found;
	}
      }
    }
    fwrite(lbuf, 1, j, fout);  /* empty j characters from buffer */
    memmove(lbuf, lbuf+j, r-j);
    r -= j;
  }
 crib_found:
  paste_swap(m0, lbuf+j);
  j += 128;
  fwrite(lbuf, 1, j, fout);  /* empty j characters from buffer */
  memmove(lbuf, lbuf+j, r-j);
  r -= j;

  /* Now replace the next occurence of crib+i[0..63] by m1 */
  while (1) {
    r += fread(lbuf+r, 1, LBUFSIZE-r, fin); /* fill buffer */
    if (r<128) {
      return 1;
    }
    for (j=0; j<r-127; j++) {
      if (memcmp(lbuf+j, crib+i, 128) == 0) {
	goto crib2_found;
      }
    }
    fwrite(lbuf, 1, j, fout);  /* empty non-crib part of buffer */
    memmove(lbuf, lbuf+j, r-j);
    r -= j;
  }
 crib2_found:
  paste_swap(m1, lbuf+j);

  /* continue emptying buffer */
  do {
    fwrite(lbuf, 1, r, fout);
    r = fread(lbuf, 1, LBUFSIZE, fin);
  } while (r>0);

  return 0;
}

#define COL0 "\033[G"
#define CTLK "\033[K"

/* a callback funciton to print progress information */
static int progress_printer_f(void *priv, char *arg) {
  fprintf(stdout, ""COL0"Progress: %s"CTLK"", arg);
  fflush(stdout);
  return 0;
}

static callback_t progress_printer = { &progress_printer_f, NULL};

/* print usage information */

static void usage(FILE *fout) {
  fprintf(fout, "%s %s. Create a pair of files with the same MD5 hash, from a template.\n", NAME, VERSION);
  fprintf(fout, "Templates are generated by linking against goodevil.o.\n");
  fprintf(fout, "\n");
  fprintf(fout, "Usage: %s [options] infile\n", NAME);
  fprintf(fout,
"Options:\n"
"    -h, --help              print this help message and exit\n"
"    -V, --version           print version info and exit\n"
"    -L, --license           print license info and exit\n"
"    -g, --goodfile <file>   file to write 'good' output to\n"
"    -e, --evilfile <file>   file to write 'evil' output to\n"
"    -i, --iv                create initial vectors only\n"
"    -c, --collision <file>  use pre-computed md5 collision blocks from file\n"
"    -s, --seed <n>          specify random seed\n"
"    --                      end of options, filename follows\n");
}

/* print version and copyright information */
static void version(FILE *fout) {
  fprintf(fout, "%s %s. Create a pair of files with the same MD5 hash, from a template.\n", NAME, VERSION);
  fprintf(fout, "Copyright (C) 2006 Peter Selinger.\n");
  fprintf(fout, "Collision generator: Copyright (C) 2005 Patrick Stach <pstach@stachliu.com>.\n");
  fprintf(fout, "Implementation of paper by Xiaoyun Wang, et al.\n");
}

static void license(FILE *fout) {
  fprintf(fout, "%s %s. Create a pair of files with the same MD5 hash, from a template.\n", NAME, VERSION);
  fprintf(fout, "Copyright (C) 2006 Peter Selinger.\n");
  fprintf(fout, "Collision generator: Copyright (C) 2005 Patrick Stach <pstach@stachliu.com>.\n");
  fprintf(fout, "Implementation of paper by Xiaoyun Wang, et al.\n");
  fprintf(fout, "\n");
  fprintf(fout,
  "This program is free software; you can redistribute it and/or modify\n"
  "it under the terms of the GNU General Public License as published by\n"
  "the Free Software Foundation; either version 2 of the License, or\n"
  "(at your option) any later version.\n"
  "\n"
  "This program is distributed in the hope that it will be useful,\n"
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
  "GNU General Public License for more details.\n"
  "\n"
  "You should have received a copy of the GNU General Public License\n"
  "along with this program; if not, write to the Free Software\n"
  "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.\n"
  "\n"
  "Some parts of this code are distributed under a Modified BSD License;\n"
  "see the source code and the file MBSD-LICENSE for details.\n"
  );
}

static struct option longopts[] = {
  {"help",         0, 0, 'h'},
  {"version",      0, 0, 'V'},
  {"license",      0, 0, 'L'},
  {"goodfile",     1, 0, 'g'},
  {"evilfile",     1, 0, 'e'},
  {"iv",           0, 0, 'i'},
  {"seed",         1, 0, 's'},
  {0, 0, 0, 0}
};

static char *shortopts = "hVLg:e:ic:s:";

uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

int main(int ac, char *av[]) {
  int r;
  unsigned int IV[4];
  unsigned int m0[32];
  unsigned int m1[32];
  int c;
  int count;
  FILE *fin;
  FILE *fout;
  FILE *fa;
  FILE *fb;
  FILE *fsheep;
  FILE *fwolf;
  int pos;
  int i=0;
  unsigned char a[128];
  unsigned char b[128];
  unsigned char copybuf[192];
  unsigned char ch;
  unsigned char ch_wolf,ch_sheep;
  callback_t *cb = &progress_printer;
  unsigned int seed = time(NULL) ^ (getpid() << 16);
  char *cmd;

  /* command line options */
  int iv_only = 0;
  char *collisionfile = NULL;
  char *goodfile = NULL;
  char *evilfile = NULL;
  char *infile = NULL;

  while ((c = getopt_long(ac, av, shortopts, longopts, NULL)) != -1) {
    switch (c) {
    case 'h':
      usage(stdout);
      exit(0);
      break;
    case 'V':
      version(stdout);
      exit(0);
      break;
    case 'L':
      license(stdout);
      exit(0);
      break;
    case 'i':
      iv_only = 1;
      break;
    case 'c':
      collisionfile = optarg;
      break;
    case 'g':
      goodfile = optarg;
      break;
    case 'e':
      evilfile = optarg;
      break;
    case 's':
      seed = strtoul(optarg, NULL, 0);
      break;
    case '?':
      fprintf(stderr, "Try --help for more information.\n");
      exit(1);
      break;
    }
  }
  count = ac-optind;
  if (count < 1) {
    fprintf(stderr, "No filename given. Try --help for more information.\n");
    exit(1);
  } else if (count > 1) {
    fprintf(stderr, "Too many filenames given. Try --help for more information.\n");
    exit(1);
  }
  infile = av[optind];

  if (!goodfile && !evilfile) {
    goodfile = append_string(infile, ".good");
    evilfile = append_string(infile, ".evil");
  }

  if (!collisionfile) {
    
    fin = fopen(infile, "rb");
    if (!fin) {
      fprintf(stderr, "%s: %s: %s\n", NAME, infile, strerror(errno));
      exit(1);
    }
    
    r = find_iv(fin, IV);
    fclose(fin);
	
    if (r == 1) {
      fprintf(stderr, "%s: %s: no crib found.\n", NAME, infile);
      exit(1);
    }
    pos=r-strlen(CRIB);
	printf("Pos: %d\n",pos);
    fprintf(stdout, "%08x%08x%08x%08x\n", swap_uint32(IV[0]), swap_uint32(IV[1]), swap_uint32(IV[2]), swap_uint32(IV[3]));
    cmd=(char*)malloc(strlen(FASTCOLL_CMD)+16+1);
	sprintf(cmd,FASTCOLL_CMD,swap_uint32(IV[0]), swap_uint32(IV[1]), swap_uint32(IV[2]), swap_uint32(IV[3]));
	printf("%s\n",cmd);
    if (iv_only) {
	  system(cmd);
	  //printf("Run %s and press any key to continue",cmd);
	  //getchar();
	  fa=fopen("a","rb");
	  fb=fopen("b","rb");
	  fread(a,128,1,fa);
	  fread(b,128,1,fb);
	  fclose(fa);
	  fclose(fb);
	  fin = fopen(infile, "rb");
	  fsheep = fopen("sheep.exe","wb"); 
	  fwolf = fopen("wolf.exe","wb");
	  while(!feof(fin)){
		ch=fgetc(fin);
		
		if (ftell(fin)>pos && ftell(fin)<=pos+128){
			//fputc(a[ftell(fin)-pos-1],fsheep);
			//fputc(b[ftell(fin)-pos-1],fwolf);
			ch_sheep=a[ftell(fin)-pos-1];
			ch_wolf=b[ftell(fin)-pos-1];
		}
		else{
			ch_wolf=ch;
			ch_sheep=ch;
		}
		if (ftell(fin)>pos-16 && ftell(fin)<=pos+176){
			copybuf[i++]=ch_wolf;
		}
		if (ftell(fin)>pos+176 && ch>0 && i>0){
			ch_wolf=copybuf[192-i];
			ch_sheep=copybuf[192-i];
			i--;
		}
		fputc(ch_sheep,fsheep);
		fputc(ch_wolf,fwolf);
	  }
	  fclose(fwolf);
	  fclose(fsheep);
	  fclose(fin);
      return 0;
    }
    /*
    fprintf(stdout, "Searching for MD5 collision (this can take several hours)...\n");
    fprintf(stdout, "Random seed: %u\n", seed);
    srandom(seed);
    md5coll_with_iv(IV, m0, m1, cb);
    fprintf(stdout, "\n");
    fprintf(stdout, "Collision blocks:\n");
    write_collision_file(stdout, m0, m1);
    */
  } else {
    
    fin = fopen(collisionfile, "r");
    if (!fin) {
      fprintf(stderr, "%s: %s: %s\n", NAME, collisionfile, strerror(errno));
      exit(1);
    }
    r = read_collision_file(fin, m0, m1);
    fclose(fin);
    if (r != 64) {
      fprintf(stderr, "%s: %s: bad file format, %d of 64 values read\n", NAME, collisionfile, r);
      exit(1);
    }
  }

  if (goodfile) {
    fin = fopen(infile, "rb");
    if (!fin) {
      fprintf(stderr, "%s: %s: %s\n", NAME, infile, strerror(errno));
      exit(1);
    }
    fout = fopen(goodfile, "wb");
    if (!fout) {
      fprintf(stderr, "%s: %s: %s\n", NAME, goodfile, strerror(errno));
    } else {
      fprintf(stdout, "Writing 'good' file %s.\n", goodfile);
      r = evilize(fin, fout, m1, m1);
      if (r) {
	fprintf(stderr, "%s: %s: not a valid template file, crib not found\n", NAME, infile);
	exit(1);
      }
      fclose(fout);
    }
    fclose(fin);
  }
  
  if (evilfile) {
    fin = fopen(infile, "rb");
    if (!fin) {
      fprintf(stderr, "%s: %s: %s\n", NAME, infile, strerror(errno));
      exit(1);
    }
    fout = fopen(evilfile, "wb");
    if (!fout) {
      fprintf(stderr, "%s: %s: %s\n", NAME, evilfile, strerror(errno));
    } else {
      fprintf(stdout, "Writing 'evil' file %s.\n", evilfile);
      r = evilize(fin, fout, m0, m1);
      if (r) {
	fprintf(stderr, "%s: %s: not a valid template file, crib not found\n", NAME, infile);
	exit(1);
      }
      fclose(fout);
    }
    fclose(fin);
  }
  return 0;
}

