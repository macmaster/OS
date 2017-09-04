#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"

void test1() {
	printf("test 1...............\n");
	List *list = List_new();
	char *dprk[] = {"Our", "Comrade", "Kim", "Jong", "Un", NULL};
	for(char **cstr = dprk; *cstr != NULL; ++cstr){
		printf("cstr: %s\n", strdup(*cstr));
		List_append(list, (void *) strdup(*cstr));
	}

	printf ("results [%d]\n", list->size);
	for(int i = 1; i <= list->size; ++i){
		printf("%s\n", (char *)List_get(list, list->size - i));
	}
	printf ("\n");

	List_free(list);
}

void test2(){
	printf("test 2...............\n");
	List *list = List_new();
	char *dprk[] = {"Our", "Comrade", "Kim", "Jong", "Un", NULL};
	for(char **cstr = dprk; *cstr != NULL; ++cstr){
		printf("cstr: %s\n", strdup(*cstr));
		List_insert(list, 0, (void *) strdup(*cstr));
	}

	List_insert(list, 0, (void *) strdup("start"));
	List_insert(list, list->size / 2, (void *) strdup("middle"));
	List_insert(list, list->size, (void *) strdup("end"));
	printf ("results insert[%d]\n", list->size);
	for(int i = 1; i <= list->size; ++i){
		printf("%s\n", (char *)List_get(list, list->size - i));
	}
	printf ("\n");

	List_remove(list, list->size - 1);
	List_remove(list, list->size / 2);
	List_remove(list, 0);
	printf ("results insert[%d]\n", list->size);
	for(int i = 1; i <= list->size; ++i){
		printf("%s\n", (char *)List_get(list, list->size - i));
	}
	printf ("\n");


	List_free(list);
}

void test3() {
	printf("test 3...............\n");
	List *list = List_new();
	char *dprk[] = {"Our", "Comrade", "Kim", "Jong", "Un", NULL};
	for(char **cstr = dprk; *cstr != NULL; ++cstr){
		printf("cstr: %s\n", strdup(*cstr));
		List_insert(list, 0, (void *) strdup(*cstr));
	}
	for(char **cstr = dprk; *cstr != NULL; ++cstr){
		printf("cstr: %s\n", List_get(list, 0));
		List_remove(list, 0);
	}

	printf ("results clear[%d]\n", list->size);
	for(int i = 1; i <= list->size; ++i){
		printf("%s\n", (char *)List_get(list, list->size - i));
	}

	List_free(list);

}

int main(){
	test1();
	test2();
	test3();

}

