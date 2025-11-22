#ifndef _HASH_MAP_TEST_UTILS_
#define _HASH_MAP_TEST_UTILS_

#include <assert.h>
#include <stdio.h>
#include "../../../../src/main/c/support/data-structures/HashMapADT.h"

// Redefinition of CDT for testing purproses
typedef struct {
    void* key;
    void* value;
    bool deleted;
} MapEntry;

struct HashMapCDT {
    MapEntry** lookup;
    size_t lookup_length, lookup_size, size_of_key, size_of_value;
    double load_factor;
    int (*hash_code)(void*);
    bool (*key_equals)(void*, void*);
};

// Asserts
void assert_lookup_index_entry(HashMapADT hash_map, int lookup_index, MapEntry* expected, bool (*value_equals)(void*, void*));
void assert_lookip_index_key(HashMapADT hash_map, int lookup_index, void* expected);
void assert_lookup_index_value(HashMapADT hash_map, int lookup_index, void* expected, bool (*value_equals)(void*, void*));

// Inserts
void insert_lookup_entry_by_index(HashMapADT hash_map, int lookup_index, MapEntry* entry);
void create_and_insert_lookup_entry_by_index(HashMapADT hash_map, int lookup_index, void* key, void* value);

// Mocks
int hash_to_0_mock(void* elem);
bool equals_true_mock(void* elem1, void* elem2);
bool equals_false_mock(void* elem1, void* elem2);

// Helpers
int identity_function(void* elem);
bool int_equals(void* elem1, void* elem2);

#endif