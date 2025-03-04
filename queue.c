#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
    list_for_each_entry_safe (item, is, head, list) {
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
    list_for_each (node, head)
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
    list_for_each_entry_safe (item, is, head, list) {
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
    list_for_each_safe (item, is, head) {
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
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
