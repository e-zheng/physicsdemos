#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "list.h"

typedef struct list{
    size_t size;
    size_t capacity;
    void ** data;
    free_func_t free_list;
}list_t;

list_t *list_init(size_t initial_size, free_func_t freer){
    list_t *vlist = malloc(sizeof(list_t));
    assert(vlist != NULL);
    vlist->size = 0;
    vlist->capacity = initial_size;
    vlist->data = malloc(initial_size * sizeof(void *));
    assert(vlist->data != NULL);
    vlist->free_list = freer;
    return vlist;
}

void list_free(list_t *list){
    assert(list->data != NULL);
    if (list->free_list != NULL){
        for (size_t i = 0; i < list->size; i++){
            list->free_list(list->data[i]);
        }
    }
    free(list->data);
    free(list);
}

size_t list_size(list_t *list){
    return list->size;
}

void *list_get(list_t *list, size_t index){
    assert(list->size > index);
    return list->data[index];
}

void *list_remove(list_t *list, size_t index){
    assert(list->size != 0);
    assert(list->size > index);
    void *list_pointer = list_get(list, index);
    for (size_t n = index; n < list->size - 1; n++){
        list->data[n] = list_get(list, n + 1);
    }
    list->size--;
    return list_pointer;
}

void list_add(list_t *list, void *value){
    if(list->size + 1 >= list->capacity){
        list->capacity *= 2;
        list->data = realloc(list->data, list->capacity * sizeof(void*));
    }
    assert(value != NULL);
    assert(list->data != NULL);
    list->data[list->size] = value;
    list->size++;
}
