/* sk_hash.h - the API for the MkrNav hash index.
  */

#ifndef _SK_HASH__H_
#define _SK_HASH__H_

#ifndef J2ME
#define SK_HASH_MODULO 4093
#else
#define SK_HASH_MODULO 397
#endif

struct sk_hash_struct {

   const char *name;

   struct sk_hash_struct *next_hash;
   struct sk_hash_struct *prev_hash;

   int head[SK_HASH_MODULO];

   int    size;
   int   *next;
   void **values;

   /* Statistics: */
   int count_add_first;
   int count_add_next;
   int count_get_first;
   int count_get_next;

};

typedef struct sk_hash_struct Sk_Hash;


Sk_Hash *sk_hash_new (const char *name, int size);

void sk_hash_add       (Sk_Hash *hash, int key, int index);
int  sk_hash_get_first (Sk_Hash *hash, int key);
int  sk_hash_get_next  (Sk_Hash *hash, int index);
void sk_hash_resize    (Sk_Hash *hash, int size);
int  sk_hash_remove    (Sk_Hash *hash, int key, int index);

void sk_hash_free (Sk_Hash *hash);

void  sk_hash_set_value (Sk_Hash *hash, int index, void *value);
void *sk_hash_get_value (Sk_Hash *hash, int index);

void  sk_hash_summary (void);

int sk_hash_string (const char *str);

void sk_hash_clean( Sk_Hash *hash );

#endif // _SK_HASH__H_

