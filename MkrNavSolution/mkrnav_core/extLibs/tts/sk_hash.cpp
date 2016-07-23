/* sk_hash.c - A simple hash index mechanism for MkrNav.
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mkrnav.h"
#include "sk_hash.h"

#define _abs_(x) x<0?-x:x 

static Sk_Hash *HashLast = NULL;


Sk_Hash *sk_hash_new (const char *name, int size) {

   int i;
   Sk_Hash *hash = (Sk_Hash *)malloc (sizeof(Sk_Hash));

   sk_check_allocated(hash);

   hash->name = name;

   for (i = 0; i < SK_HASH_MODULO; i++) {
      hash->head[i] = -1;
   }

   hash->size = size;
   hash->next =(int *) malloc (size * sizeof(int));
   hash->values = NULL;

   sk_check_allocated(hash->next);

   for (i = 0; i < size; i++) {
      hash->next[i] = -1;
   }

	if (HashLast) {
		HashLast->prev_hash = hash;
	}
   hash->next_hash = HashLast;
   hash->prev_hash = NULL;
   HashLast = hash;

   return hash;
}


void sk_hash_add (Sk_Hash *hash, int key, int index) {

   int hash_code = _abs_(key % SK_HASH_MODULO);

   if ((index < 0) || (index > hash->size)) {
      sk_log (SK_FATAL, "invalid index %d in hash table %s",
                         index, hash->name);
   }

   if (hash->head[hash_code] < 0) {
      hash->count_add_first += 1;
   } else {
      hash->count_add_next += 1;
   }

   hash->next[index] = hash->head[hash_code];
   hash->head[hash_code] = index;
}


int  sk_hash_get_first (Sk_Hash *hash, int key) {

   int hash_code = _abs_(key % SK_HASH_MODULO);

   hash->count_get_first += 1;

   return hash->head[hash_code];
}


int  sk_hash_get_next  (Sk_Hash *hash, int index) {

   if ((index < 0) || (index >= hash->size)) {
      sk_log (SK_FATAL, "invalid index %d in hash table %s",
                         index, hash->name);
   }

   hash->count_get_next += 1;

   return hash->next[index];
}


int	sk_hash_remove	(Sk_Hash *hash, int key, int index) {

   int hash_code = _abs_(key % SK_HASH_MODULO);
   int *slot;

   if ((index < 0) || (index > hash->size)) {
      sk_log (SK_FATAL, "invalid index %d in hash table %s",
                         index, hash->name);
   }

	slot = &(hash->head[hash_code]);

	while (*slot >= 0) {

		if (*slot == index) {

			*slot = hash->next[index];
			return 1;
		}
		slot = &(hash->next[*slot]);
	}

	return 0;
}

/*
 *	Just clean hash entries
 */
void sk_hash_clean( Sk_Hash *hash )
{
   int i;
   for (i = 0; i < SK_HASH_MODULO; i++)
   {
	  hash->head[i] = -1;
   }

   for (i = 0; i < hash->size; i++)
   {
	  hash->next[i] = -1;
   }
}



void sk_hash_resize (Sk_Hash *hash, int size) {

   int i;

   hash->next = (int*)realloc (hash->next, size * sizeof(int));
   sk_check_allocated(hash->next);

   if (hash->values != NULL) {
      hash->values = (void**)realloc (hash->values, size * sizeof(void *));
      sk_check_allocated(hash->values);
   }

   for (i = hash->size; i < size; i++) {
      hash->next[i] = -1;
   }
   hash->size = size;
}


void sk_hash_free (Sk_Hash *hash) {

	Sk_Hash *prev = hash->prev_hash;
	Sk_Hash *next = hash->next_hash;

	if (hash == HashLast) {
		HashLast = hash->next_hash;
	}
	if (hash->prev_hash) {
		hash->prev_hash->next_hash = next;
	}
	if (hash->next_hash) {
		hash->next_hash->prev_hash = prev;
	}

   if (hash->values != NULL) {
      free (hash->values);
   }
   free (hash->next);
   free (hash);
}


void  sk_hash_set_value (Sk_Hash *hash, int index, void *value) {

   if ((index < 0) || (index > hash->size)) {
      sk_log (SK_FATAL, "invalid index %d in hash table %s",
                         index, hash->name);
   }

   if (hash->values == NULL) {
      hash->values = (void**)calloc (hash->size, sizeof(void *));
      sk_check_allocated(hash->values);
   }

   hash->values[index] = value;
}


void *sk_hash_get_value (Sk_Hash *hash, int index) {

   if ((index < 0) || (index > hash->size)) {
      sk_log (SK_FATAL, "invalid index %d in hash table %s",
                         index, hash->name);
   }

   if (hash->values == NULL) return NULL;

   return hash->values[index];
}


#ifndef J2ME
void  sk_hash_summary (void) {

   Sk_Hash *hash;

   for (hash = HashLast; hash != NULL; hash = hash->next_hash) {

      fprintf (stderr, "-- hash table %s:", hash->name);

      fprintf (stderr,
               "\n--      %d lists, %d items",
               hash->count_add_first,
               hash->count_add_first + hash->count_add_next);
      if (hash->count_add_first > 0) {
         fprintf (stderr,
                  " (%d items/list)",
                  (hash->count_add_first + hash->count_add_next)
                      / hash->count_add_first);
      }

      fprintf (stderr,
               "\n--      %d get first, %d get",
               hash->count_get_first,
               hash->count_get_first + hash->count_get_next);
      if (hash->count_get_first > 0) {
         fprintf (stderr,
                  " (%d loops/search)",
                  (hash->count_get_first + hash->count_get_next)
                      / hash->count_get_first);
      }
      fprintf (stderr, "\n");
   }
}

#endif


int sk_hash_string (const char *str) {

   int hash = 0;
   unsigned int i;

   for (i=0; i<strlen(str); i++) {
      hash += str[i]*(i+1);
   }

   return hash;
}

