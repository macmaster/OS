/** List.c
 * Generic List data type.
 * node data must be casted upon retrival
 *
 * Author : Ronald Macmaster
 * EID : rpm953
 */

#include "List.h"
#include <stdlib.h>

// private helper function to fetch a node.
static ListNode *List_getNode(List * list, int pos);

/* typedef struct ListNode ListNode;
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
}; */

/**
 * Creates a new List object. (Constructor)
 */
List *List_new() {
	List *list = (List *) malloc(sizeof(List));
	if (list) {
		list->size = 0;
		list->head = NULL;
		list->tail = NULL;
		return list;
	} else {
		return NULL;
	}
}

/**
 * Cleans up the List and frees resources.
 */
void List_free(List * list, void (*freeData) (void *)) {
	ListNode *curr = list->head;
	ListNode *next = list->head;

	// free list nodes
	while (curr) {
		next = curr->next;
		freeData(curr->data);
		free(curr);
		curr = next;
	}
	free(list);
}

/**
 * Adds a new item to the end of the list.
 */
bool List_append(List * list, void *data) {
	if (!list)
		return false;
	return List_insert(list, list->size, data);
}

/**
 * Inserts a new item at the given position.
 */
bool List_insert(List * list, int pos, void *data) {
	if (!list || pos < 0 || pos > list->size)
		return false;

	ListNode *node = (ListNode *) malloc(sizeof(ListNode));
	if (!node) {
		return false;
	} else {
		node->data = data;
		node->prev = NULL;
		node->next = NULL;
	}

	// add node to the list.
	if (list->size == 0) {
		list->head = node;
		list->tail = node;
	} else if (pos == 0) {
		list->head->prev = node;
		node->next = list->head;
		list->head = node;
	} else if (pos == list->size) {
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	} else {
		ListNode *curr = List_getNode(list, pos);
		node->prev = curr->prev;
		node->next = curr;
		curr->prev->next = node;
		curr->prev = node;
	}

	list->size += 1;
	return true;
}

/**
 * Removes an item from the list at the given position.
 */
bool List_remove(List * list, int pos, void (*freeData) (void *)) {
	if (!list || pos < 0 || pos >= list->size)
		return false;

	ListNode *node = NULL;

	// find node in the list.
	if (list->size == 1) {
		node = list->head;
		list->head = NULL;
		list->tail = NULL;
	} else if (pos == 0) {
		node = list->head;
		node->next->prev = NULL;
		list->head = node->next;
	} else if (pos == (list->size - 1)) {
		node = list->tail;
		node->prev->next = NULL;
		list->tail = node->prev;
	} else {
		node = List_getNode(list, pos);
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	// free node
	freeData(node->data);
	free(node);
	list->size -= 1;
	return true;
}

/**
 * Gets the item at the given position.
 */
void *List_get(List * list, int pos) {
	ListNode *node = List_getNode(list, pos);
	return node != NULL ? node->data : NULL;
}

/**
 * Gets the node at the given position.
 */
static ListNode *List_getNode(List * list, int pos) {
	if (!list || pos < 0 || pos >= list->size)
		return NULL;

	if (pos < (list->size / 2)) {
		ListNode *curr = list->head;
		for (int i = 0; i < pos; ++i)
			curr = curr->next;
		return curr;
	} else {		// reverse search
		ListNode *curr = list->tail;
		for (int i = list->size - 1; i > pos; --i)
			curr = curr->prev;
		return curr;
	}
}
