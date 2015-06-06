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

/* object-oriented callback interface (for progress reports). "arg" is
   the payload of the callback, and f returns 1 if it wishes to
   interrupt. */
struct callback_s {
  int (*f)(void *priv, char *arg);
  void *priv;
};
typedef struct callback_s callback_t;

/* find an MD5 collision (with standard initial vector), and store it
   in m0/m1. Use callback function cb for progress reports (see
   above), if non-NULL. Return 0 on success, 1 if an interrupt was
   requested. */
int md5coll(unsigned int m0[32], unsigned int m1[32], callback_t *cb);

/* same, but use specified initial vector */
int md5coll_with_iv(unsigned int IV[4], unsigned int m0[32], unsigned int m1[32], callback_t *cb);

