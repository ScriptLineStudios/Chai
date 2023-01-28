#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../include/hashmap.h"
 
Hashmap init_hashmap() {
    Hashmap hashmap;
    hashmap.size = 0;
    hashmap.array = malloc(sizeof(Element) * hashmap.size);
    return hashmap;
}

int hash(const char *key) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < strlen(key); i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

void grow_hashmap(Hashmap *hashmap) {
    unsigned int old_size = hashmap->size;
    hashmap->size++;
    hashmap->size += hashmap->size * 2;
    hashmap->array = realloc(hashmap->array, sizeof(Element) * hashmap->size);
    for (int i = old_size; i < hashmap->size; i++) {
        Element element;
        element.filled = false;
        element.value = -1;
        hashmap->array[i] = element;
    }
}

int get_element(Hashmap *hashmap, const char *key) {
    int hashed_string = hash(key);
    int position = hashed_string % hashmap->size;
    printf("getting position %d\n", position);
    return hashmap->array[position].value;
}

void add_element(Hashmap *hashmap, const char *key, int value) {
    grow_hashmap(hashmap);
    int hashed_string = hash(key);
    int position = hashed_string % hashmap->size;
    printf("position = %d\n", position);
    Element *element = &hashmap->array[position];
    if (element->filled) {
        printf("Collision!\n");
    }
    else {
        element->filled = true;
        element->value = value;
        element->key = key;
    }
}

void print_hashmap(Hashmap *hashmap) {
    printf("Hashmap Dump:\n");
    for (int i = 0; i < hashmap->size; i++) {
        if (hashmap->array[i].filled) {
            printf("%d: %d (%s)\n", i, hashmap->array[i].value, hashmap->array[i].key);
        }
        else {
            printf("%d: EMPTY\n", i);
        }
    }
}