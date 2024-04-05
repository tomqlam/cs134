#include "SortedList.h"

#include <sched.h>
#include <stdio.h>
#include <string.h>

void SortedList_insert(SortedList_t* list, SortedListElement_t* element) {
    SortedListElement_t* it = list->next; // assumes that if list is empty, it will have a head node pointing to itself
    while (it != list) { // keep on going to next node until reaches last node, meaning element's key is > all keys in list or...
        if (strcmp(it->key, element->key) > 0) { // we find a node where the value is > than the element we want to insert
            break;
        }
        // yield after the strcmp check is done, if no synchronization should result in unsorted list
        if (opt_yield & INSERT_YIELD) {
            sched_yield();
        }
        it = it->next;
    }
    // we modify the element next and prev pointers and the pointers of the nodes around it such that it 
    // inserts before the one greatest node.
    element->next = it;
    element->prev = it->prev;
    it->prev->next = element;
    it->prev = element;
}

int SortedList_delete(SortedListElement_t* element) {
    // verify the the element is not correupted
    if (element->prev->next == element && element->next->prev == element) {
        element->prev->next = element->next;
        // yield after next pointer of previous is modified, if unprotected another thread could add 
        // another node next to the deleted element resulting in corruption
        if (opt_yield & DELETE_YIELD) {
            sched_yield();
        }
        element->next->prev = element->prev;
        element->next = NULL;
        element->prev = NULL;
        return 0;
    } else {
        return 1;
    }
}

SortedListElement_t* SortedList_lookup(SortedList_t* list, const char* key) {
    SortedListElement_t* it = list->next;
    while (it != list) { // keep on going until it reaches head of list
        // scheduling yield here might cause an unprotected version to ruin the list and the lookup to 
        // not return an expected value
        if (opt_yield & LOOKUP_YIELD) {
            sched_yield();
        }
        // when key is found, return pointer to the node containing it
        if (strcmp(it->key, key) == 0) {
            return it;
        }
        it = it->next;
    }
    return NULL; // if end of list reached, key is not in list
}

int SortedList_length(SortedList_t* list) {
    SortedListElement_t* it = list->next;
    int counter = 0;
    while (it != list) { // increment counter until it reaches the head again
        if (it->prev->next != it || it->next->prev != it) { // check for corruptions
            return -1;
        }
        // scheduling a yield here might cause an error for a unprotected version to report an incorrect value for length
        if (opt_yield & LOOKUP_YIELD) {
            sched_yield();
        }
        it = it->next;
        counter += 1;
    }
    return counter;
}