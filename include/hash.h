
#ifndef HASH_H
#define HASH_H

typedef struct Hashmap *Hashmap;
typedef void Key;
typedef void Value;
typedef uint32_t hash_t;
typedef hash_t (*HashFunction)(Key *key);

typedef struct HashmapEntry {
    Key *key;
    Value *value;
} HashmapEntry;

/**
 * Default hash function for strings that can be used with `hashmap_create`.
 */
hash_t hash_string(Key *key);

/**
 * Create a new hashmap with the given hash function.
 *
 * You should call `hashmap_destroy` when you are done with the hashmap.
 *
 * The hash map does not take ownership of any keys or values
 */
Hashmap hashmap_create(HashFunction hash_function);

/**
 * Insert a key-value pair into the hashmap.
 */
void hashmap_insert(Hashmap map, Key *key, Value *value);

/**
 * Get the value associated with the given key.
 */
Value *hashmap_get(Hashmap map, Key *key);

/**
 * Remove the key-value pair associated with the given key.
 *
 * Returns the entry that was removed.
 * The caller is responsible for freeing the key and value.
 */
HashmapEntry *hashmap_remove(Hashmap map, Key *key);

/**
 * Get the number of key-value pairs in the hashmap.
 */
size_t hashmap_size(Hashmap map);

/**
 * Destroy the hashmap.
 * If `destroy_key` is not NULL, it will be called on each key.
 * If `destroy_value` is not NULL, it will be called on each value.
 */
void hashmap_destroy(Hashmap map, void (*destroy_key)(Key *), void (*destroy_value)(Value *));

#endif

