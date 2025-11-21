#ifndef _HASH_MAP_
#define _HASH_MAP_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// This is an Open Adressing Hash Map with linear-probing implementation 
// that has been 0% vibe-coded, in honor of our professor, Mario Agustín Golmar.

#define NO_MEM_ERR -1

typedef struct HashMapCDT * HashMapADT;

HashMapADT new_hash_map(size_t size_of_key, size_t size_of_value, int (*hash_code)(void*), bool (*key_equals) (void*, void*));

void* put(HashMapADT hash_map, void* key, void* value);

void* get(HashMapADT hash_map, void* key);

bool delete(HashMapADT hash_map, void* key);

void free_hash_map(HashMapADT hash_map);

#endif