/* The MIT License

   Copyright (C) 2011 Zilong Tan (tzlloch@gmail.com)

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef __ULIB_RC4_H
#define __ULIB_RC4_H

#include <stddef.h>

typedef struct rc4_key_t {
        unsigned char state[256];
        unsigned char x;
        unsigned char y;
} rc4_key_t;

#ifdef __cplusplus
extern "C" {
#endif

        void rc4_set_key(const unsigned char *key_data, size_t key_data_len,
                         rc4_key_t * key);

        void rc4_crypt(unsigned char *buffer, size_t buffer_len,
                       rc4_key_t * key);

#ifdef __cplusplus
}
#endif

#endif  /* __RC4_H */

