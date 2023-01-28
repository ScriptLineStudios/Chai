#ifndef HASHMAP
#define HASHMAP

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    bool filled;
    const char *key;
    int value;
} Element;

typedef struct {
    Element *array;
    unsigned int size;
} Hashmap;

Hashmap init_hashmap();
void add_element(Hashmap *hashmap, const char *key, int value);
void add_element(Hashmap *hashmap, const char *key, int value);
int get_element(Hashmap *hashmap, const char *key);
void print_hashmap(Hashmap *hashmap);

#endif