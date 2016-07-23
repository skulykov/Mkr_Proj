/* sk_list.h - Manage a list.
 *
 *
 */

#ifndef INCLUDE__SK_LIST__H
#define INCLUDE__SK_LIST__H

/* Linked lists are fully circular -- no pointer is ever NULL. 
 * When empty, a list head's "list_first" and "list_last"
 * pointers point at the list head itself.
 * 
 * The head of a list is identical to any element in the list. 
 * To aid in debugging, the head is given a different type --
 * this helps reduce confusion when calling the insert/remove
 * routines, by letting the compiler enforce type safety.
 *
 * However, the gcc optimizer complains about some of the casts
 * ("dereferencing type-punned pointer will break strict-aliasing
 * rules") when the list head and the items have separate
 * typedefs.  So, when optimizing, we simply declare them both as
 * the same thing.
 *
 */

#include "mkrnav.h"

struct sk_list_link {
    struct sk_list_link *next;
    struct sk_list_link *prev;
};

typedef struct sk_list_link Sk_ListItem;

#if SK_LISTS_TYPESAFE

/* Separate list-head structure. */
struct sk_list_head {
    struct sk_list_link *list_first;
    struct sk_list_link *list_last;
};

typedef struct sk_list_head Sk_List;

#else

/* Use the same struct for both the list head and the items. */
#define list_first next
#define list_last  prev
typedef struct sk_list_link Sk_List;

#endif

/* List heads must be initialized before first use.  This
 * initialization could be done with a declaration like:
 *  Sk_List MyListHead = 
 *      { (Sk_ListItem *)&MyListHead, (Sk_ListItem *)&MyListHead };
 * but the runtime macro seems cleaner:
 */
#define SK_LIST_INIT(head) (head)->list_first = \
                ((head)->list_last = (queue *)head)

/* The enqueue routine works on both list heads and list items.  The
 * assumption is that the two are isomorphic.
 */
void sk_list_enqueue(Sk_ListItem *before, Sk_ListItem *after);

/* Removes an item from whatever list it's part of */
Sk_ListItem *sk_list_remove(Sk_ListItem *item);

/* Returns the number of elements in a list */
int sk_list_count(const Sk_List *list);

/* Inline functions (not macros) to help enforce type safety. */
static inline void sk_list_append(Sk_List *lh, Sk_ListItem *e) {
        sk_list_enqueue(e, (Sk_ListItem *)lh->list_last);
}
static inline void sk_list_insert(Sk_List *lh, Sk_ListItem *e) {
        sk_list_enqueue(e, (Sk_ListItem *)lh);
}
static inline void sk_list_put_before
                (Sk_ListItem *olde, Sk_ListItem *newe) {
        sk_list_enqueue(newe, olde->prev);
}
static inline void sk_list_put_after
                (Sk_ListItem *olde, Sk_ListItem *newe) {
        sk_list_enqueue(newe, olde);
}

#define SK_LIST_FIRST(head)   ((head)->list_first)
#define SK_LIST_LAST(head)    ((head)->list_last)
#define SK_LIST_NEXT(element) ((element)->next)
#define SK_LIST_PREV(element) ((element)->prev)
#define SK_LIST_EMPTY(head)   ((head)->list_first == (Sk_ListItem *)(head))

/* Anything attached to oldhead will be re-anchord at newhead.
 * oldhead will be cleared, and anything previously at newhead
 * will be lost.
 */
#define SK_LIST_MOVE(newhead,oldhead) \
        if ( SK_LIST_EMPTY(oldhead) ) {\
                SK_LIST_INIT(newhead); \
        } \
        else { \
                (newhead)->list_first = (oldhead)->list_first; \
                (newhead)->list_last = (oldhead)->list_last; \
                (newhead)->list_first->prev = (Sk_ListItem *)(newhead); \
                (newhead)->list_last->next = (Sk_ListItem *)(newhead); \
        } \
        SK_LIST_INIT(oldhead)

/* Anything attached to fromhead will be appended at the end of tohead.
 * fromhead is emptied.
 */
#define SK_LIST_SPLICE(tohead,fromhead) \
        if ( !SK_LIST_EMPTY(fromhead) ) {\
                (tohead)->list_last->next = (Sk_ListItem *)(fromhead)->list_first; \
                (fromhead)->list_first->prev = (Sk_ListItem *)(tohead)->list_last; \
                (fromhead)->list_last->next = (Sk_ListItem *)(tohead); \
                (tohead)->list_last = (fromhead)->list_last; \
        } \
        SK_LIST_INIT(fromhead)

#define SK_LIST_FOR_EACH(listhead, element, tmp) \
        for ((element) = SK_LIST_FIRST(listhead); \
                (tmp) = SK_LIST_NEXT(element), \
                (element) != (Sk_ListItem *)(listhead); \
                (element) = (tmp))

#define SK_LIST_FOR_EACH_FROM_TO(from, to, element, tmp) \
        for ((element) = (from); \
                (tmp) = SK_LIST_NEXT(element), \
                (element) != (Sk_ListItem *)(to); \
                (element) = (tmp))


/* The rest of the definitions below are to retain source
 * compatibility for the GPX files, which came originally from
 * gpsbabel
 */

typedef struct sk_list_link queue;
#if SK_LISTS_TYPESAFE
typedef struct sk_list_head queue_head;
#else
typedef struct sk_list_link queue_head;
#endif

#define QUEUE_INIT(head)        SK_LIST_INIT(head)
#define QUEUE_FIRST(head)       SK_LIST_FIRST(head) 
#define QUEUE_LAST(head)        SK_LIST_LAST(head)  
#define QUEUE_EMPTY(head)       SK_LIST_EMPTY(head)  
#define QUEUE_NEXT(element)     SK_LIST_NEXT(element) 
#define QUEUE_PREV(element)     SK_LIST_PREV(element)

#define ENQUEUE_TAIL(listhead, newelement) \
                sk_list_append(listhead, newelement)
#define ENQUEUE_HEAD(listhead, newelement) \
                sk_list_insert(listhead, newelement)
#define ENQUEUE_AFTER(element, newelement) \
                sk_list_put_after(element, newelement)
#define ENQUEUE_BEFORE(element, newelement) \
                sk_list_put_before(element, newelement)

#define QUEUE_MOVE(newhead,oldhead) SK_LIST_MOVE(newhead,oldhead) 
#define QUEUE_SPLICE(tohead,fromhead) SK_LIST_SPLICE(tohead,fromhead)

#define QUEUE_FOR_EACH(listhead, element, tmp) \
                SK_LIST_FOR_EACH(listhead, element, tmp)
#define QUEUE_FOR_EACH_FROM_TO(from, to, element, tmp) \
                SK_LIST_FOR_EACH_FROM_TO(from, to, element, tmp)



#endif // INCLUDE__SK_LIST__H
