
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Hashmap Hashmap;
typedef void Key;
typedef void Value;
typedef uint32_t hash_t;
typedef hash_t (*HashFunction)(Key *key);
typedef bool (*CompareFunction)(Key *key1, Key *key2);

typedef struct Hasher {
    HashFunction hash;
    CompareFunction equal;
} Hasher;

typedef struct HashmapEntry {
    Key *key;
    Value *value;
} HashmapEntry;

/**
 * Default hash function for strings used in `STRING_HASHER`.
 */
hash_t string_hash(Key *key);

/**
 * Default equality function for strings used in `STRING_HASHER`.
 */
bool string_equal(Key *key1, Key *key2);

/**
 * Default hasher for strings for use in `hashmap_create`.
 */
#define STRING_HASHER ((Hasher) { \
    .hash = string_hash,          \
    .equal = string_equal,        \
})

/**
 * Create a new hashmap with the given hash function.
 *
 * Returns NULL if the hashmap could not be created.
 * You should call `hashmap_destroy` when you are done with the hashmap.
 * The hash map does not take ownership of any keys or values
 */
Hashmap *hashmap_create(Hasher hasher);

/**
 * Insert a key-value pair into the hashmap.
 *
 * `value` may not be NULL.
 * `entry` may be NULL if you do not need the entry.
 *
 * There are 3 possible outcomes:
 *  1. The key-value pair is successfully inserted. In this case `*entry` will
 *     be set to the newly inserted value true will be returned.
 *  2. The key-value pair is not inserted because the key already exists in the
 *     hashmap. In this case `*entry` will be set to the existing value pair and
 *     false will be returned.
 *  3. The insertion failed because the hashmap could not be resized. In this
 *     case `*entry` will be set to NULL and false will be returned.
 *
 *  `entry` will remain valid until the next operation on the hashmap.
 */
bool hashmap_insert(Hashmap *map, Key *key, Value *value, Value **entry);

/**
 * Get the value associated with the given key.
 *
 * Returns NULL if the key is not in the hashmap.
 */
Value *hashmap_get(Hashmap *map, Key *key);

/**
 * Remove the key-value pair associated with the given key.
 *
 * `key` may not be NULL.
 * `entry` may be NULL if you do not need the entry.
 *
 * The caller is responsible for freeing the key and value.
 *
 * Returns true if the key-value pair was removed, false otherwise.
 * If the key-value pair was removed, `entry` will be set to the removed
 * key-value pair. Otherwise, the value of `entry` is undefined and should not
 * be used.
 */
bool hashmap_remove(Hashmap *map, Key *key, HashmapEntry *entry);

/**
 * Get the number of key-value pairs in the hashmap.
 */
size_t hashmap_size(Hashmap *map);

/**
 * Destroy the hashmap.
 *
 * If `destroy_key` is not NULL, it will be called on each key.
 * If `destroy_value` is not NULL, it will be called on each value.
 */
void hashmap_destroy(Hashmap *map, void (*destroy_key)(Key *), void (*destroy_value)(Value *));

#endif

