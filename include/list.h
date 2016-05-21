#ifndef LIST_H
#define LIST_H

struct list {
    struct list *prev;
    struct list *next;
};

#define LIST_INIT(x) (struct list){ .prev = &(x), .next = &(x) }
#define LIST(x) struct list x = { &x, &x }

/* B must be an empty list */
static inline void _list_add(struct list *prev, struct list *next, struct list *new) {
    prev->next = new;
    new->prev = prev;
    new->next = next;
    next->prev = new;
}

static inline int list_empty(struct list *l) {
    return l->prev == l;
}

static inline void list_add(struct list *head, struct list *new) {
    _list_add( head, head->next, new);
}

static inline void list_del(struct list * a) {
    a->prev->next = a->next;
    a->next->prev = a->prev;
}

#define LIST_FOREACH(i, list) \
    for (i = (list)->next; i != (list); i = i->next)

#define LIST_FOREACH_SAFE(i, list, tmp) \
    for (i = (list)->next, (tmp) = i->next; \
            i != (list); \
            i = (tmp), (tmp) = (tmp)->next)
#endif

