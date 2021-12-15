/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include "SortedList.h"
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int opt_yield = 0;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    
    // Case1. Head is not exist or Element to be inserted is NULL.
    if (list == NULL || element == NULL)
        return;

    // Case2. Only head is exist.
    if (list->next == NULL) {
        if (opt_yield & INSERT_YIELD)
            sched_yield();
        list->next = element;
        element->prev = list;
        element->next = NULL;
        return;
    }
    
    SortedListElement_t *search = list->next;

    while (search->next != NULL && (strcmp(search->key, element->key) < 0))
        search = search->next;

    if (opt_yield & INSERT_YIELD)
        sched_yield();

    // Case3. element is inserted to previous of head.
    if (search->next == NULL) {
        search->next = element;
        element->prev = search;
        element->next = NULL;
    }

    // Case4. element is inserted not previous of head.
    else if (strcmp(search->key, element->key) >= 0) {
        SortedListElement_t *prePtr = search->prev;
        prePtr->next = element;
        search->prev = element;
        element->prev = prePtr;
        element->next = search;
    }
}

int SortedList_delete( SortedListElement_t *element) {

    // Case 1. Element is NULL
    if (element == NULL)
        return 1;
    
    // Check to make sure that next->prev and prev->next both point to this node.
    if (element->prev != NULL) {
        if (element->prev->next != element)
            return 1;
    }
    if (element->next != NULL) {
        if (element->next->prev != element)
            return 1;
    }

    // Delete element
    if (opt_yield & DELETE_YIELD)
        sched_yield();
    if (element->prev != NULL)
        element->prev->next = element->next;
    if (element->next != NULL)
        element->next->prev = element->prev;

    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    
    // Case 1. List is empty
    if (list == NULL) {
        return NULL;
    }
    
    SortedListElement_t *search = list->next;

    while (search != NULL && (strcmp(search->key, key) != 0)) {
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        search = search->next;
    }

    // Case 2. Element is not exist in list.
    if (search == NULL)
        return NULL;
    // Case 3. Element has founded.
    else
        return search;
}

int SortedList_length(SortedList_t *list) {
    
    // Case1. List is empty
    if (list == NULL) {
        printf("\nList is empty!\n");
        return -1;
    }
    
    int len = 0;
    SortedListElement_t *search = list->next;
    while (search != NULL) {
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        search = search->next;
        len += 1;
    }
    return len;
}