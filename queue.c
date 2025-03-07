#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (!new_head) {
        return NULL;
    }
    INIT_LIST_HEAD(new_head);
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    if (list_empty(head)) {
        free(head);
        return;
    }
    element_t *node = NULL;
    element_t *safe = NULL;
    list_for_each_entry_safe(node, safe, head, list) {
        q_release_element(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }
    new_element->value = malloc(strlen(s) + 1);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    strncpy(new_element->value, s, strlen(s));
    new_element->value[strlen(s)] = '\0';
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }
    new_element->value = malloc(strlen(s) + 1);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    strncpy(new_element->value, s, strlen(s));
    new_element->value[strlen(s)] = '\0';
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_elem = list_first_entry(head, element_t, list);
    list_del_init(&remove_elem->list);
    if (sp) {
        strncpy(sp, remove_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_elem = list_last_entry(head, element_t, list);
    list_del_init(&remove_elem->list);
    if (sp) {
        strncpy(sp, remove_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int length = 0;
    struct list_head *current;
    list_for_each(current, head) {
        length++;
    }
    return length;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del_init(slow);
    element_t *to_delete = list_entry(slow, element_t, list);
    q_release_element(to_delete);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *start = head->next;
    while (start != head) {
        struct list_head *end = start->next;
        const element_t *start_e = list_entry(start, element_t, list);
        bool found_dup = false;
        while (end != head) {
            const element_t *end_e = list_entry(end, element_t, list);
            if (!strcmp(start_e->value, end_e->value)) {
                found_dup = true;
                end = end->next;
            } else {
                break;
            }
        }
        if (found_dup) {
            while (start != end) {
                struct list_head *tmp = start->next;
                element_t *to_delete = list_entry(start, element_t, list);
                list_del_init(start);
                q_release_element(to_delete);
                start = tmp;
            }
        } else {
            start = end;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *prev, *current;
    for (prev = head->next, current = prev->next;
         prev != head && current != head;
         prev = prev->next, current = prev->next) {
        list_del(prev);
        list_add(prev, current);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *front = head->prev;
    struct list_head *last = head->next;
    struct list_head *current = head;
    do {
        current->next = front;
        current->prev = last;
        front = current;
        current = last;
        last = current->next;
    } while (current != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1) {
        return;
    }
    int length = q_size(head);
    if (length < k) {
        return;
    }
    LIST_HEAD(result);
    while (length >= k) {
        LIST_HEAD(tmp);
        for (int i = 0; i < k; i++) {
            struct list_head *node = head->next;
            list_del(node);
            list_add(node, &tmp);
        }
        list_splice_tail_init(&tmp, &result);
        length -= k;
    }
    if (!list_empty(head)) {
        list_splice_tail_init(head, &result);
    }
    list_splice_tail_init(&result, head);
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
