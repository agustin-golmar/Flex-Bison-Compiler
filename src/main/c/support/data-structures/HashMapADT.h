#ifndef _HASH_MAP_
#define _HASH_MAP_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// This is an Open Adressing Hash Map with linear-probing implementation 
// that has been 0% vibe-coded, in honor of our professor, Mario Agustín Golmar.

#define NO_MEM_ERR -1

typedef struct HashMapCDT * HashMapADT;

struct KeyNode{
    void** key;
    struct KeyNode* next;
};

typedef struct KeyNode* KeyReference;

HashMapADT hash_map_new(size_t size_of_key, size_t size_of_value, int (*hash_code)(void*), bool (*key_equals) (void*, void*));

void* hash_map_put(HashMapADT hash_map, void* key, void* value);

void* hash_map_get(HashMapADT hash_map, void* key);

KeyReference hash_map_get_keys(HashMapADT hash_map);

bool hash_map_remove(HashMapADT hash_map, void* key);

void hash_map_free(HashMapADT hash_map);

#endif