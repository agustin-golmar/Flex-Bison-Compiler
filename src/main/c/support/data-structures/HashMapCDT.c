#include "HashMapADT.h"

// TODO: malloc error validations! We've to defeat the enemy! (current enemy = memory leaks)

#define KEY_FOUND -2
#define KEY_NOT_FOUND -3

typedef struct {
    void* key;
    void* value;
    bool deleted;
} MapEntry;

struct HashMapCDT {
    MapEntry** lookup;
    size_t lookup_length, lookup_size, size_of_key, size_of_value;
    KeyReference keys;
    double load_factor;
    int (*hash_code)(void*);
    bool (*key_equals)(void*, void*);
};

struct HashMapCDT* hash_map_new(
    size_t size_of_key, 
    size_t size_of_value,
    int (*hash_code)(void*),
    bool (*key_equals)(void*, void*)
) {
    HashMapADT to_return = (struct HashMapCDT*) calloc(1, sizeof(struct HashMapCDT));
    to_return->load_factor = 0.75;
    to_return->lookup_size = 0;
    to_return->size_of_key = size_of_key;
    to_return->size_of_value = size_of_value;
    int initial_size = 50;
    to_return->lookup = (MapEntry**) calloc(initial_size, sizeof(MapEntry*));
    to_return->lookup_length = 50;
    to_return->hash_code = hash_code;
    to_return->key_equals = key_equals;
    return to_return;
}

static size_t hash(HashMapADT hash_map, void* key) {
    return (size_t)hash_map->hash_code(key) % (size_t)hash_map->lookup_length;
}

/* Searchs for the key. Returns:
**      (output)-> return value: search result status (FOUND/NOT_FOUND/ERROR)
**      (input/output)-> key_slot: index to insert at
**      (input/output)-> first_deletion: if any logic deletions found, returns the index of the first appearence
*/
int find_key_index(HashMapADT hash_map, void* key, size_t* key_slot, signed long* first_deletion) {
    size_t current_key_slot = *key_slot;
    while(hash_map->lookup[current_key_slot] != NULL) {
        if(*first_deletion == -1 && hash_map->lookup[current_key_slot]->deleted) {
            *first_deletion = current_key_slot;
        }
        if(hash_map->key_equals(hash_map->lookup[current_key_slot]->key, key)) {
            *key_slot = current_key_slot;
            return KEY_FOUND;
        }
        
        current_key_slot = (current_key_slot + 1) % hash_map->lookup_length;
        
        // Already seen full table and no physical deletions found
        if(current_key_slot == *key_slot) {
            if(*first_deletion != -1) {
                return KEY_NOT_FOUND;
            } else {
                return NO_MEM_ERR; // Should never happen
            }
        }
    }
    *key_slot = current_key_slot;
    return KEY_NOT_FOUND;
}

static void lookup_resize(HashMapADT hash_map) {
    MapEntry** old_lookup = hash_map->lookup;
    size_t old_length = hash_map->lookup_length;
    hash_map->lookup = (MapEntry**) calloc(hash_map->lookup_length * 2, sizeof(MapEntry*));
    hash_map->lookup_length = hash_map->lookup_length * 2;
    for(int i = 0; i < old_length; i++) {
        if(old_lookup[i] != NULL) {
            MapEntry* current_entry = old_lookup[i];
            hash_map->lookup[i] = NULL;
            if(current_entry->deleted) {
                free(current_entry->key);
                free(current_entry->value);
                free(current_entry);
            } else {
                signed long first_deletion = -1, key_slot = hash(hash_map, current_entry->key);
                find_key_index(hash_map, current_entry->key, &key_slot, &first_deletion);
                // No logical deletions in the map, we can directly use the key_slot returned by put_aux
                hash_map->lookup[key_slot] = current_entry;
            }
        }
    }
    free(old_lookup);
}

static void update_value(
    HashMapADT hash_map,
    void* value, 
    size_t key_slot
) {
    memcpy(hash_map->lookup[key_slot]->value, value, hash_map->size_of_value);
}

static void create_entry(HashMapADT hash_map, void* key, void* value, size_t index) {
    MapEntry* new_entry = (MapEntry*)malloc(sizeof(MapEntry));

    new_entry->key = malloc(hash_map->size_of_key);
    memcpy(new_entry->key, key, hash_map->size_of_key);
    
    new_entry->value = malloc(hash_map->size_of_value);
    memcpy(new_entry->value, value, hash_map->size_of_value);
    
    new_entry->deleted = false;
    hash_map->lookup[index] = new_entry;

    KeyReference currentKey = hash_map->keys;    
    while(currentKey != NULL) currentKey = currentKey->next;
    currentKey = calloc(1, sizeof(struct KeyNode));
    currentKey->key = &new_entry->key;
}

static void insert_into_logical_deleted_entry(HashMapADT hash_map, void* key, void* value, size_t index) {
    memcpy(hash_map->lookup[index]->key, key, hash_map->size_of_key);
    memcpy(hash_map->lookup[index]->value, value, hash_map->size_of_value);
    
    hash_map->lookup[index]->deleted = false;

    KeyReference currentKey = hash_map->keys;
    while(currentKey != NULL) currentKey = currentKey->next;
    currentKey = calloc(1, sizeof(struct KeyNode));
    currentKey->key = &hash_map->lookup[index]->key;
}

void* hash_map_put(
    HashMapADT hash_map,
    void* key, 
    void* value
) {
    size_t key_slot = hash(hash_map, key);

    signed long first_deletion = -1, status;
    if((status = find_key_index(hash_map, key, &key_slot, &first_deletion)) == KEY_NOT_FOUND) {
        if(first_deletion == -1) {
            create_entry(hash_map, key, value, key_slot);
        } else {
            insert_into_logical_deleted_entry(hash_map, key, value, first_deletion);
            return value;
        }
    } else if(status == KEY_FOUND) {
        update_value(hash_map, value, key_slot);
        return value;
    } else {
        // TODO: check status return.
        return NULL;
    }

    if((++hash_map->lookup_size/(double)hash_map->lookup_length) > hash_map->load_factor){
        lookup_resize(hash_map);
    }
    return value;
}

void* hash_map_get(HashMapADT hash_map, void* key) {
    size_t key_slot = hash(hash_map, key);

    signed long first_deletion = -1, status;
    if((status = find_key_index(hash_map, key, &key_slot, &first_deletion)) == KEY_FOUND) {
        return hash_map->lookup[key_slot]->value;
    } else {
        return NULL;
    }
}

// I know... you are given write/read access with full controll over the ADT... not enough time/will to improve this
KeyReference hash_map_get_keys(HashMapADT hash_map) {
    return hash_map->keys;
}

static KeyReference remove_key_reference_rec(HashMapADT hash_map, size_t key_slot, KeyReference current_key, bool* found) {
    if(current_key == NULL) {
        return NULL;
    }
    if(hash_map->key_equals(current_key->key, hash_map->lookup[key_slot]->key)) {
        *found = true;
        KeyReference toReturn = current_key->next;
        free(current_key);
        return toReturn;
    }
    current_key = remove_key_reference_rec(hash_map, key_slot, current_key->next, found);
    return current_key;
}

static void remove_entry(HashMapADT hash_map, size_t key_slot) {
    // Remove key from list
    bool found = false;
    hash_map->keys = remove_key_reference_rec(hash_map, key_slot, hash_map->keys, &found);

    // If no next element, physical delete
    if(hash_map->lookup[(key_slot + 1) % hash_map->lookup_length] == NULL) {
        free(hash_map->lookup[key_slot]->key);
        free(hash_map->lookup[key_slot]->value);
        free(hash_map->lookup[key_slot]);
        hash_map->lookup[key_slot] = NULL;
    }
    // Otherwise, logical deletion
    else {
        hash_map->lookup[key_slot]->deleted = true;
    }
}

bool hash_map_remove(HashMapADT hash_map, void* key) {
    size_t key_slot = hash(hash_map, key);

    signed long first_deletion = -1, status;
    if((status = find_key_index(hash_map, key, &key_slot, &first_deletion)) == KEY_FOUND) {
        remove_entry(hash_map, key_slot);
        hash_map->lookup_size--;
        key_slot = (key_slot == 0) ? hash_map->lookup_length - 1 : key_slot - 1;
        while(hash_map->lookup[key_slot] != NULL && hash_map->lookup[key_slot]->deleted) {
            remove_entry(hash_map, key_slot);
            key_slot = (key_slot == 0) ? hash_map->lookup_length - 1 : key_slot - 1;
        }
        return true;
    } else {
        return false;
    }
}

void hash_map_free(HashMapADT hash_map) {
    for(int i = 0; i < hash_map->lookup_length; i++) {
        if(hash_map->lookup[i] != NULL) {
            free(hash_map->lookup[i]->key);
            free(hash_map->lookup[i]->value);
            free(hash_map->lookup[i]);
        }
    }
    free(hash_map->lookup);
    free(hash_map);
}
