#include "HashMapTestUtils.h"

// Assertions

void assert_lookup_index_entry(HashMapADT hash_map, int lookup_index, MapEntry* expected, bool (*value_equals)(void*, void*)) {
    assert_lookip_index_key(hash_map, lookup_index, expected->key);
    assert_lookup_index_value(hash_map, lookup_index, expected->value, value_equals);
}

void assert_lookip_index_key(HashMapADT hash_map, int lookup_index, void* expected) {
    assert(true == hash_map->key_equals(hash_map->lookup[lookup_index]->key, expected) && "\033[0;31mKey does not match with expected value");
}

void assert_lookup_index_value(HashMapADT hash_map, int lookup_index, void* expected, bool (*value_equals)(void*, void*)) {
    assert(true == value_equals(hash_map->lookup[lookup_index]->value, expected) && "\033[0;31mValue does not match with expected value");
}


// Inserts

void insert_lookup_entry_by_index(HashMapADT hash_map, int lookup_index, MapEntry* entry) {
    hash_map->lookup[lookup_index] = entry;
}

void create_and_insert_lookup_entry_by_index(HashMapADT hash_map, int lookup_index, void* key, void* value) {
    MapEntry* entry = (MapEntry*)malloc(sizeof(MapEntry));
    void* created_key = malloc(hash_map->size_of_key); void* created_value = malloc(hash_map->size_of_value);
    memcpy(created_key, key, hash_map->size_of_key); memcpy(created_value, value, hash_map->size_of_value);
    entry->key = created_key; entry->value = created_value;
    hash_map->lookup[lookup_index] = entry;
}


// Mocks

int hash_to_0_mock(void* elem) {
    return 0;
}

bool equals_true_mock(void* elem1, void* elem2) {
    return true;
}

bool equals_false_mock(void* elem1, void* elem2) {
    return false;
}

// Helpers

int identity_function(void* elem) {
    return *((int*)elem);
}

bool int_equals(void* elem1, void* elem2) {
    return *((int*)elem1) == *((int*)elem2);
}
