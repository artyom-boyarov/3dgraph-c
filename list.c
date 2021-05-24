#include "list.h"
#include "stdlib.h"

struct ListNode* get_last_node(struct ListNode* start) {
    struct ListNode* current = start;
    while (current->next) {
        current = current->next;
    }
    return current;
}

struct ListNode* list_create() {
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->next = NULL;
    return node;
}
void list_append_back(struct ListNode* start, void* val) {
    struct ListNode* new = malloc(sizeof(struct ListNode));
    new->val = val;
    get_last_node(start)->next = new;

}
void list_append_front(struct ListNode* start, void* val) {
    struct ListNode* new = malloc(sizeof(struct ListNode));
    new->val = val;
    new->next = start;
}
void list_pop_back(struct ListNode* start) {
    struct ListNode* current = start, *prev;
    while (current->next) {
        prev = current;
        current = current->next;
    }
    prev->next = NULL;
    free(current->next);
}
struct ListNode* list_pop_front(struct ListNode* start) {
    struct ListNode* current = start;
    start = start->next;
    free(current);
    return start;
}
void list_free(struct ListNode* start) {
    struct ListNode* current = start;
    while (current->next) {
        struct ListNode* next = current->next;
        free(current);
        current = next;
    }
    free(current);
}
