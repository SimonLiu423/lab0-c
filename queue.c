#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#define STACKSIZE 32

typedef struct {
    struct list_head *arr[STACKSIZE];
    int size;
} stack_t;

static inline void s_push(stack_t *s, struct list_head *ptr)
{
    s->arr[s->size++] = ptr;
}

static inline struct list_head *s_pop(stack_t *s)
{
    if (s->size == 0)
        return NULL;

    s->size--;
    return s->arr[s->size];
}

/**
 * create_element() - Create an element
 * @s: string to be copied to the element's value
 *
 * Return: the pointer to the element, NULL if allocation failed
 */
static inline element_t *create_element(char *s)
{
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return NULL;

    size_t len = strlen(s) + 1;
    char *val = malloc(len * sizeof(char));
    if (!val) {
        free(node);
        return NULL;
    }

    strncpy(val, s, len);
    node->value = val;

    return node;
}

typedef enum _order { NON_DECREASING = 1, NON_INCREASING = -1 } Order;

/**
 * monotonic_from_right() - Make the queue monotonically
 * non-increasing/non-decreasing from right
 * @head: header of the queue
 * @order: non-increasing or non-decreasing order
 *
 * Returns: the number of elements in queue after performing operation
 */
static inline int monotonic_from_right(struct list_head *head, Order order)
{
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    int cnt = 1;

    struct list_head *left = head->prev->prev, *right = head->prev;
    while (right != head && right != head->next) {
        element_t *l_item = list_entry(left, element_t, list);
        const element_t *r_item = list_entry(right, element_t, list);
        while (left != head &&
               order * strcmp(r_item->value, l_item->value) > 0) {
            left = left->prev;
            list_del(&l_item->list);
            q_release_element(l_item);
            l_item = list_entry(left, element_t, list);
        }
        right = left;
        left = right->prev;
        if (right != head)
            cnt++;
    }
    return cnt;
}

/**
 * merge_two_lists() - Merge two lists
 * @left: left list
 * @right: right list
 * @descend: descending order
 *
 * Returns: the head of the merged list
 */
static struct list_head *merge_two_lists(struct list_head *left,
                                         struct list_head *right,
                                         bool descend)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    int flag = descend ? -1 : 1;

    for (node = NULL; left && right; *node = (*node)->next) {
        const element_t *l_item = list_entry(left, element_t, list);
        const element_t *r_item = list_entry(right, element_t, list);

        node =
            (flag * strcmp(l_item->value, r_item->value) <= 0) ? &left : &right;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) (((size_t) left) | ((size_t) right));
    return head;
}

/**
 * rebuild_list() - Rebuild the list to make it circular
 * @head: head of the list
 *
 * Returns: void
 */
static inline void rebuild_list(struct list_head *head)
{
    struct list_head *node = head;
    while (node->next) {
        node->next->prev = node;
        node = node->next;
    }
    node->next = head;
    head->prev = node;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *item, *is;

    /* cppcheck-suppress uninitvar */
    list_for_each_entry_safe(item, is, head, list) {
        free(item->value);
        free(item);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = create_element(s);
    if (!node)
        return false;

    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = create_element(s);
    if (!node)
        return false;

    list_add_tail(&node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_first_entry(head, element_t, list);
    if (!node)
        return NULL;

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&node->list);

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_last_entry(head, element_t, list);
    if (!node)
        return NULL;

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&node->list);

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int count = 0;
    struct list_head *node;
    list_for_each(node, head)
        count++;

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    /* Use fast & slow pointer to find the middle node */
    struct list_head *tortoise, *hare;
    tortoise = hare = head;

    do {
        tortoise = tortoise->next;
        hare = hare->next->next;
    } while (hare != head && hare->next != head);

    /* Delete the element */
    list_del(tortoise);
    q_release_element(list_entry(tortoise, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool is_dup = false;
    element_t *prev = NULL, *item, *is;

    /* cppcheck-suppress uninitvar */
    list_for_each_entry_safe(item, is, head, list) {
        if (prev && strcmp(prev->value, item->value) == 0) {
            is_dup = true;
            list_del(&item->list);
            q_release_element(item);
        } else {
            if (is_dup) {
                list_del(&prev->list);
                q_release_element(prev);
                is_dup = false;
            }
            prev = item;
        }
    }
    if (is_dup) {
        list_del(&prev->list);
        q_release_element(prev);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *node = head->next;
    while (node != head && node->next != head) {
        list_del(node);
        list_add(node, node->next);

        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    /* cppcheck-suppress uninitvar */
    struct list_head *item, *is, *tmp;
    list_for_each_safe(item, is, head) {
        tmp = item->next;
        item->next = item->prev;
        item->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    if (k == 2) {
        q_swap(head);
        return;
    }

    LIST_HEAD(tmp_head);
    struct list_head *prev = NULL, *first = NULL, *node, *next;
    int cnt = 0;
    /* cppcheck-suppress uninitvar */
    list_for_each_safe(node, next, head) {
        cnt++;
        if (cnt == 1) {
            first = node;
            prev = node->prev;
        } else if (cnt == k) {
            /* Form a temporary circular list by inserting tmp_head as the head
             * of the k elements */
            list_add(&tmp_head, node);
            tmp_head.next = first;
            first->prev = &tmp_head;

            /* Utilize q_reverse to reverse the k elements */
            q_reverse(&tmp_head);

            /* Link the temporary list back to the original */
            prev->next = node;
            node->prev = prev;
            first->next = next;
            next->prev = first;

            cnt = 0;
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    stack_t stack = {.size = 0};

    unsigned int count = 0;
    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        node->next = NULL;
        s_push(&stack, node);
        unsigned int next_count = count + 1;
        for (int i = 0; i < sizeof(int) * 8; i++) {
            /* Merge if there are 2 sorted list with 2^i nodes */
            if (!((count & (1U << i)) && !(next_count & (1U << i))))
                break;
            struct list_head *right = s_pop(&stack);
            struct list_head *left = s_pop(&stack);
            s_push(&stack, merge_two_lists(left, right, descend));
        }
        count = next_count;
    }
    /* Merge the rest */
    while (stack.size > 1) {
        struct list_head *s1 = s_pop(&stack);
        struct list_head *s2 = s_pop(&stack);
        s_push(&stack, merge_two_lists(s2, s1, descend));
    }

    struct list_head *first = s_pop(&stack);
    head->next = first;
    first->prev = head;
    rebuild_list(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    /* Ascend from the left means descend from the right */
    return monotonic_from_right(head, NON_INCREASING);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    /* Descend from the left means ascend from the right */
    return monotonic_from_right(head, NON_DECREASING);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *qctx;
    struct list_head *first_q = NULL;
    list_for_each_entry(qctx, head, chain) {
        if (first_q)
            list_splice_tail_init(qctx->q, first_q);
        else
            first_q = qctx->q;
    }

    int size = q_size(first_q);

    /* The queues guaranteed sorted before calling this function, if the chain
     * is singular, no need to sort the first queue again.
     */
    if (list_is_singular(head))
        return size;

    q_sort(first_q, descend);

    return size;
}
