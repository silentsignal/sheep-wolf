/* MD5 Collision Generator
 * Copyright (C) 2005 Patrick Stach <pstach@stachliu.com>.
 * Implementation of paper by Xiaoyun Wang, et al.
 * A few optimizations to make the solving method a bit more deterministic.
 * Modifications Copyright (C) 2006 Peter Selinger.
 * 
 * This work is licensed under a modified BSD License, see the file
 * MBSD-LICENSE for full details.
 * Any derivative works or references must cite the authors.
 */

/*
 * Changes:
 * 2006/02/07: Peter Selinger - separated user interface from library
 * 2006/02/07: Peter Selinger - progress reports
 * 2006/02/07: Peter Selinger - added options -h, -V, -q, -s
 *
 * Usage: see ./md5coll --help
 *
 * Notes: Requires being built as 32 bit (unsigned int as 32 bit)
 */

/* This file contains the user interface (mainly parsing of
   command-line). The real work is done in md5coll_lib. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#include "md5coll_lib.h"

#define NAME "md5coll"

/* print usage information */
static void usage(FILE *fout) {
  fprintf(fout, "%s %s. MD5 collision generator.\n", NAME, MD5COLL_VERSION);
  fprintf(fout, "\n");
  fprintf(fout, "Usage: %s [options] [IV0 IV1 IV2 IV3]\n", NAME);
  fprintf(fout,
"Arguments (optional):\n"
"    IV0 IV1 IV2 IV3         four initial vectors in format 0x00000000\n"
"Options:\n"
"    -h, --help              print this help message and exit\n"
"    -L, --license           print license info and exit\n"
"    -V, --version           print version info and exit\n"
"    -q, --quiet             suppress progress information\n"
"    -s, --seed <n>          seed the random number generator\n"
	  );
}

/* print version information */
static void version(FILE *fout) {
  fprintf(fout, "%s %s. MD5 collision generator.\n", NAME, MD5COLL_VERSION);
  fprintf(fout, "Copyright (C) 2005 Patrick Stach <pstach@stachliu.com>.\n");
  fprintf(fout, "Implementation of paper by Xiaoyun Wang, et al.\n");
  fprintf(fout, "Modifications Copyright (C) 2006 Peter Selinger.\n");
}

/* print license information */
static void license(FILE *fout) {
  fprintf(fout, "%s %s. MD5 collision generator.\n", NAME, MD5COLL_VERSION);
  fprintf(fout, "Copyright (C) 2005 Patrick Stach <pstach@stachliu.com>.\n");
  fprintf(fout, "Implementation of paper by Xiaoyun Wang, et al.\n");
  fprintf(fout, "Modifications Copyright (C) 2006 Peter Selinger.\n");
  fprintf(fout, "\n");
  fprintf(fout, 
  "Redistribution and use in source and binary forms, with or without\n"
  "modification, are permitted provided that the following conditions are\n"
  "met:\n"
  "\n"
  "1. Redistributions of source code must retain the above copyright\n"
  "   notice, this list of conditions and the following disclaimer.\n"
  "2. Redistributions in binary form must reproduce the above copyright\n"
  "   notice, this list of conditions and the following disclaimer in the\n"
  "   documentation and/or other materials provided with the distribution.\n"
  "3. The name of the author may not be used to endorse or promote\n"
  "   products derived from this software without specific prior written\n"
  "   permission.\n"
  "\n"
  "THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR\n"
  "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
  "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
  "DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,\n"
  "INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
  "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
  "SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n"
  "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,\n"
  "STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING\n"
  "IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n"
  "POSSIBILITY OF SUCH DAMAGE.\n");
}

static struct option longopts[] = {
  {"help",         0, 0, 'h'},
  {"license",      0, 0, 'L'},
  {"version",      0, 0, 'V'},
  {"quiet",        0, 0, 'q'},
  {"seed",         1, 0, 's'},
  {0, 0, 0, 0}
};

static char *shortopts = "hLVqs:";

#define COL0 "\033[G"
#define CTLK "\033[K"

static int progress_printer_f(void *priv, char *arg) {
  fprintf(stderr, ""COL0"Progress: %s"CTLK"", arg);
  fflush(stderr);
  return 0;
}

static callback_t progress_printer = { &progress_printer_f, NULL};

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

int main(int ac, char *av[])
{
  unsigned int m0[32];
  unsigned int m1[32];
  int c, count;
  callback_t *cb;
  
  /* command line options */
  int have_iv = 0;
  unsigned int IV[4];
  int quiet = 0;
  unsigned int seed = time(NULL) ^ (getpid() << 16);
  
  /* process command line options */
  while ((c = getopt_long(ac, av, shortopts, longopts, NULL)) != -1) {
    switch (c) {
    case 'h':
      usage(stdout);
      exit(0);
      break;
    case 'L':
      license(stdout);
      exit(0);
      break;
    case 'V':
      version(stdout);
      exit(0);
      break;
    case 'q':
      quiet = 1;
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
      
  if (count == 4) {
    IV[0] = strtoul(av[optind], NULL, 0);
    IV[1] = strtoul(av[optind+1], NULL, 0);
    IV[2] = strtoul(av[optind+2], NULL, 0);
    IV[3] = strtoul(av[optind+3], NULL, 0);
    have_iv = 1;
  } else if (count == 0) {
    have_iv = 0;
  } else {
    fprintf(stderr, "%s: wrong number of arguments. Try --help for more information.\n", NAME);
    exit(1);
  }

  /* end command line options */

  if (quiet) {
    cb = NULL;
  } else {
    cb = &progress_printer;
    fprintf(stdout, "Random seed: %u\n", seed);
  }

  srandom(seed);
  
  if (have_iv) {
    md5coll_with_iv(IV, m0, m1, cb);
  } else {
    md5coll(m0, m1, cb);
  }
  if (cb) {
    fprintf(stderr, " (done)\n"); /* finish progress display */
  }  

  write_collision_file(stdout, m0, m1);

  return 0;
}
