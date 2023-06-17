#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */

struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *pos, *next;
    list_for_each_entry_safe (pos, next, l, list) {
        q_release_element(container_of(&pos->list, element_t, list));
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *n = malloc(1 * sizeof(element_t));
    if (!n)
        return false;
    char *value = malloc((strlen(s) + 1) * sizeof(char));
    if (!value) {
        free(n);
        return false;
    }
    memcpy(value, s, strlen(s) + 1);
    n->value = value;
    list_add(&n->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *n = malloc(1 * sizeof(element_t));
    if (!n)
        return false;
    char *value = malloc((strlen(s) + 1) * sizeof(char));
    if (!value) {
        free(n);
        return false;
    }
    memcpy(value, s, strlen(s) + 1);
    n->value = value;
    list_add_tail(&n->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    list_del_init(&tmp->list);
    if (sp)
        snprintf(sp, bufsize, "%s", tmp->value);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    list_del_init(&tmp->list);
    if (sp)
        snprintf(sp, bufsize, "%s", tmp->value);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *right, *left;
    right = head->next;
    left = head->prev;

    while (right != left && left->prev != right) {
        right = right->next;
        left = left->prev;
    }

    list_del(left);
    q_release_element(list_entry(left, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head || list_empty(head))
        return false;

    bool flag = false;
    element_t *curr, *next;

    list_for_each_entry_safe (curr, next, head, list) {
        if (&next->list != head && strcmp(curr->value, next->value) == 0) {
            flag = true;
            list_del(&curr->list);
            q_release_element(curr);
        } else if (flag) {
            flag = false;
            list_del(&curr->list);
            q_release_element(curr);
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

    bool flag = false;
    struct list_head *node, *tmp = head;

    list_for_each (node, head) {
        if (flag == true) {
            list_move(node, tmp);
            node = node->next;
            tmp = node;
        }
        flag = !flag;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;

    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    if (!head || list_empty(head))
        return;

    int countK = 0;
    int count = q_size(head) / k;
    struct list_head *node, *safe, *tmp = head;

    list_for_each_safe (node, safe, head) {
        countK++;
        if (count)
            list_move(node, tmp);
        if (countK % k == 0) {
            tmp = safe->prev;
            count--;
        }
    }
}


void mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    if (!L1 || !L2)
        return;

    struct list_head *Lnode = L1->next, *Rnode = L2->next;

    while (Lnode != L1 && Rnode != L2) {
        if (strcmp(list_entry(Lnode, element_t, list)->value,
                   list_entry(Rnode, element_t, list)->value) > 0) {
            struct list_head *tmp = Rnode->next;
            list_move_tail(Rnode, Lnode);
            Rnode = tmp;
        } else {
            Lnode = Lnode->next;
        }
    }

    if (q_size(L2) != 0) {
        list_splice_tail(L2, L1);
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || q_size(head) == 1)
        return;

    struct list_head *tmp, *mid;
    tmp = head->next;
    mid = head->prev;

    while (tmp != mid && mid->prev != tmp) {
        tmp = tmp->next;
        mid = mid->prev;
    }

    LIST_HEAD(right);
    list_cut_position(&right, head, mid->prev);

    q_sort(head);
    q_sort(&right);
    mergeTwoLists(head, &right);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    if (!head || list_empty(head))
        return 0;

    struct list_head *curr = head->prev;
    while (curr->prev != head) {
        if (strcmp(list_entry(curr, element_t, list)->value,
                   list_entry(curr->prev, element_t, list)->value) > 0) {
            struct list_head *tmp = curr->prev;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        } else {
            curr = curr->prev;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if (!head)
        return 0;

    queue_contex_t *queue_head =
        container_of(head->next, queue_contex_t, chain);

    for (struct list_head *curr = head->next->next; curr != head;
         curr = curr->next) {
        queue_contex_t *queue = container_of(curr, queue_contex_t, chain);
        mergeTwoLists(queue_head->q, queue->q);
        INIT_LIST_HEAD(queue->q);
        queue->size = 0;
    }

    return q_size(queue_head->q);
}
