/** List.h
 * Generic List data type.
 * node data must be casted upon retrival
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#include "bool.h"

#ifndef LIST_H
#define LIST_H

typedef struct ListNode ListNode;
struct ListNode {
	void *data;
	ListNode *prev;
	ListNode *next;
};

typedef struct List List;
struct List {
	int size;
	ListNode *head;
	ListNode *tail;
};

/**
 * Creates a new List object. (Constructor)
 */
List *List_new();

/**
 * Cleans up the List and frees resources.
 */
void List_free(List *list);

/**
 * Adds a new item to the end of the list.
 */
bool List_append(List *list, void *data);

/**
 * Inserts a new item at the given position.
 */
bool List_insert(List *list, int pos, void *data);

/**
 * Removes an item from the list at the given position.
 */
bool List_remove(List *list, int pos);

/**
 * Gets the item at the given position.
 */
void *List_get(List *list, int pos);

#endif
