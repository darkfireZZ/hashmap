
#include <hashmap.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * The initial capacity of the hash map.
 */
#define INITIAL_CAPACITY 8

/**
 * The reciprocal of the maximum load factor of the hash map.
 *
 * The load factor (alpha) is the ratio of the number of entries to the capacity.
 * The reciprocal of the load factor is (1 / alpha).
 *
 * When the load factor exceeds this value, the hash map will be resized.
 */
#define RECIPROCAL_LOAD_FACTOR 2

#define UNREACHABLE(msg)      \
    do {                      \
        assert(false && msg); \
        abort();              \
    } while (0)

typedef struct HashmapEntryInternal {
    HashmapEntry entry;
    hash_t hash;
} HashmapEntryInternal;

struct Hashmap {
    size_t size;
    size_t capacity;
    HashFunction hash;
    CompareFunction equal;
    HashmapEntryInternal *entries;
};

static void mark_uninitialized(HashmapEntryInternal *entry) {
    entry->entry.key = NULL;
    entry->entry.value = NULL;
}

static bool is_initialized(HashmapEntryInternal *entry) {
    bool initialized = (entry->entry.key != NULL);
    assert((initialized || entry->entry.value == NULL)
        && "When an entry is uninitialized, both key and value should be NULL");
    return initialized;
}

/**
 * Initialize a hash map with a given capacity.
 *
 * Returns true if the initialization was successful, false otherwise.
 * If the initialization fails, the hash map is left in an undefined state.
 */
static bool hashmap_init_with_capacity(Hashmap *hashmap, Hasher hasher, size_t capacity) {
    hashmap->size = 0;
    hashmap->capacity = capacity;
    hashmap->hash = hasher.hash;
    hashmap->equal = hasher.equal;

    if (capacity == 0) {
        hashmap->entries = NULL;
    } else {
        hashmap->entries = malloc(hashmap->capacity * sizeof(*hashmap->entries));
        if (hashmap->entries == NULL) {
            return false;
        }
        for (size_t i = 0; i < hashmap->capacity; ++i) {
            mark_uninitialized(&hashmap->entries[i]);
        }
    }

    return true;
}

static void hashmap_init(Hashmap *map, Hasher hasher) {
    bool success = hashmap_init_with_capacity(map, hasher, 0);
    assert(success && "initializing a hashmap with capacity 0 should always succeed");
}

static HashmapEntryInternal *hashmap_entry_find(Hashmap *map, Key *key) {
    assert(key != NULL);

    if (map->capacity == 0) {
        return NULL;
    }

    hash_t hash = map->hash(key);
    size_t start_index = hash % map->capacity;

#define LOOP_BODY                                       \
        HashmapEntryInternal *entry = &map->entries[i]; \
        if (!is_initialized(entry)) {                   \
            return NULL;                                \
        }                                               \
        if (hash == entry->hash                         \
                && map->equal(key, entry->entry.key)) { \
            return entry;                               \
        }

    for (size_t i = start_index; i < map->capacity; ++i) {
        LOOP_BODY
    }

    for (size_t i = 0; i < start_index; ++i) {
        LOOP_BODY
    }

#undef LOOP_BODY

    return NULL;
}

static bool increase_capacity_if_necessary(Hashmap *map) {
    if ((map->size + 1) * RECIPROCAL_LOAD_FACTOR > map->capacity) {
        size_t old_size = map->size;
        size_t old_capacity = map->capacity;
        HashmapEntryInternal *old_entries = map->entries;

        Hasher hasher = {
            .hash = map->hash,
            .equal = map->equal,
        };
        size_t new_capacity;
        if (old_capacity == 0) {
            new_capacity = INITIAL_CAPACITY;
        } else {
            new_capacity = 2 * old_capacity;
        }
        bool success = hashmap_init_with_capacity(map, hasher, new_capacity);
        if (!success) {
            /* If the initialization fails, we revert to the old state */
            map->size = old_size;
            map->capacity = old_capacity;
            map->hash = hasher.hash;
            map->equal = hasher.equal;
            map->entries = old_entries;
            return false;
        }

        /* And then copy all the elements into the newly allocated memory */
        for (size_t i = 0; i < old_capacity; ++i) {
            HashmapEntryInternal *entry = &old_entries[i];
            if (is_initialized(entry)) {
                bool inserted = hashmap_insert(map, entry->entry.key, entry->entry.value, NULL);
                assert(inserted && "inserting an element into a larger map should always succeed");
            }
        }

        assert(map->size == old_size
                && "after moving, the size should still be the same");

        free(old_entries);
    }

    return true;
}

/**
 * This is the djb2 string hash function
 * from http://www.cse.yorku.ca/~oz/hash.html
 */
hash_t string_hash(void *key) {
    unsigned char *str = (unsigned char *)key;

    hash_t hash = 5381;
    hash_t c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

bool string_equal(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

Hashmap *hashmap_create(Hasher hasher) {
    Hashmap *hashmap = malloc(sizeof(*hashmap));
    if (hashmap == NULL) {
        return NULL;
    }
    hashmap_init(hashmap, hasher);
    return hashmap;
}

bool hashmap_insert(Hashmap *map, Key *key, Value *value, Value **entry) {
    assert(key != NULL);
    assert(value != NULL);

    bool success = increase_capacity_if_necessary(map);
    if (!success) {
        if (entry != NULL) {
            *entry = NULL;
        }
        return false;
    }

    hash_t hash = map->hash(key);
    size_t start_index = hash % map->capacity;

#define INSERT_IF_POSSIBLE                                                \
        HashmapEntryInternal *bucket = &map->entries[i];                  \
                                                                          \
        if (!is_initialized(bucket)) {                                    \
            /* We found a free slot to insert our new entry */            \
                                                                          \
            bucket->entry.key = key;                                      \
            bucket->entry.value = value;                                  \
            bucket->hash = hash;                                          \
            map->size += 1;                                               \
                                                                          \
            if (entry != NULL) {                                          \
                *entry = bucket->entry.value;                             \
            }                                                             \
            return true;                                                  \
        }                                                                 \
                                                                          \
        if (hash == bucket->hash && map->equal(key, bucket->entry.key)) { \
            /* An entry with the same key already exists */               \
            if (entry != NULL) {                                          \
                *entry = bucket->entry.value;                             \
            }                                                             \
            return false;                                                 \
        }

    for (size_t i = start_index; i < map->capacity; ++i) {
        INSERT_IF_POSSIBLE
    }

    for (size_t i = 0; i < start_index; ++i) {
        INSERT_IF_POSSIBLE
    }

#undef INSERT_IF_POSSIBLE

    UNREACHABLE("Unreachable: There should always be some capacity left");
}

Value *hashmap_get(Hashmap *map, Key *key) {
    assert(key != NULL);

    HashmapEntryInternal *entry = hashmap_entry_find(map, key);
    if (entry == NULL) {
        return NULL;
    } else {
        return entry->entry.value;
    }
}

bool hashmap_remove(Hashmap *map, Key *key, HashmapEntry *entry) {
    HashmapEntryInternal *to_remove = hashmap_entry_find(map, key);
    if (to_remove == NULL) {
        return false;
    }

    if (entry != NULL) {
        *entry = to_remove->entry;
    }

    HashmapEntryInternal *replacement = to_remove;

#define LOOP_BODY                               \
        if (!is_initialized(current)) {         \
            *to_remove = *replacement;          \
            mark_uninitialized(replacement);    \
            map->size -= 1;                     \
            return true;                        \
        }                                       \
                                                \
        if (current->hash == to_remove->hash) { \
            replacement = current;              \
        }                                       \

    HashmapEntryInternal *end = map->entries + map->capacity;
    for (HashmapEntryInternal *current = to_remove; current != end; ++current) {
        LOOP_BODY
    }

    end = to_remove;
    for (HashmapEntryInternal *current = map->entries; current != end; ++current) {
        LOOP_BODY
    }
#undef LOOP_BODY
    
    UNREACHABLE("Unreachable: The hashmap is never completely full");
}

size_t hashmap_size(Hashmap *map) {
    return map->size;
}

void hashmap_destroy(Hashmap *map, void (*destroy_key)(Key *), void (*destroy_value)(Value *)) {

    /* We first clean up all the keys and values */
    for (size_t i = 0; i < map->capacity; ++i) {
        HashmapEntryInternal *entry = &map->entries[i];
        if (is_initialized(entry)) {
            if (destroy_key != NULL) {
                destroy_key(entry->entry.key);
            }
            if (destroy_value != NULL) {
                destroy_value(entry->entry.value);
            }
        }
    }

    /* Then we clean up the hash map itself */
    free(map->entries);
    free(map);
}

