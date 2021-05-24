#ifndef ARRAY_H
#define ARRAY_H

struct ListNode {
    void* val;
    struct ListNode* next;
};

struct ListNode* list_create();
void list_append_back(struct ListNode* start, void* val);
void list_append_front(struct ListNode* start, void* val);
void list_pop_back(struct ListNode* start);
struct ListNode* list_pop_front(struct ListNode* start);
void list_free(struct ListNode* start);

#endif
