
#include <hashmap.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_STRING(string, color) "\033[" color "m" string "\033[0m"

#define BLACK "30"
#define RED "31"
#define GREEN "32"
#define YELLOW "33"
#define BLUE "34"
#define MAGENTA "35"
#define CYAN "36"
#define WHITE "37"

#define SUCCESS 0
#define FAILURE 1

#define TEST(test_function_call) \
    num_total += 1; \
    if ((test_function_call) == SUCCESS) { \
        num_successful += 1; \
    } else { \
        fprintf(stderr, "Test " COLOR_STRING("%s", RED) " in " COLOR_STRING("%s", RED) " failed\n", #test_function_call, __FILE__); \
    }

#define ASSERT(condition) \
    if (!(condition)) {   \
        fprintf(stderr, "Assertion failed: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
        return FAILURE; \
    }

typedef int result_t;

static result_t create_destroy(void) {
    Hashmap *map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    size_t size = hashmap_size(map);
    ASSERT(size == 0);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

static result_t insert_get_remove_once(void) {
    char *key = "key";
    char *value = "value";

    bool success;
    size_t size;
    Value *value_entry;

    Hashmap *map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    /* Insert a key-value pair */
    value_entry = NULL;
    success = hashmap_insert(map, key, value, &value_entry);
    ASSERT(success);
    ASSERT(value_entry != NULL);
    ASSERT(value_entry == value);

    /* Now the map should have one entry */
    size = hashmap_size(map);
    ASSERT(size == 1);

    /* Check that the value is in the map */
    value_entry = hashmap_get(map, key);
    ASSERT(value_entry != NULL);
    ASSERT(value_entry == value);

    /* Remove the entry */
    HashmapEntry removed_entry;
    success = hashmap_remove(map, key, &removed_entry);
    ASSERT(success);
    ASSERT(removed_entry.key == key);
    ASSERT(removed_entry.value == value);

    /* Now the map should be empty */
    size = hashmap_size(map);
    ASSERT(size == 0);

    /* And the value should no longer be in the map */
    value_entry = hashmap_get(map, key);
    ASSERT(value_entry == NULL);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

static result_t get_from_empty(void) {
    Hashmap *map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    /* Get a key that is not in the map */
    Value *value = hashmap_get(map, "non-existent");
    ASSERT(value == NULL);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

static result_t remove_from_empty(void) {
    Hashmap *map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    /* Remove a key that is not in the map */
    bool success = hashmap_remove(map, "non-existent", NULL);
    ASSERT(!success);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

static hash_t return_0(void *key) {
    (void)key;
    return 0;
}

static bool uint_equals(void *key1, void *key2) {
    return *(unsigned int *)key1 == *(unsigned int *)key2;
}

/**
 * Insert n key-value pairs with the same hash and remove them again.
 */
static result_t insert_remove_colliding(unsigned int n) {
    Hasher hasher = {
        .hash = return_0,
        .equal = uint_equals
    };

    Hashmap *map = hashmap_create(hasher);
    ASSERT(map != NULL);

    /* Insert n key-value pairs with the same hash */
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int *key_val = malloc(sizeof(i));
        ASSERT(key_val != NULL);
        *key_val = i;

        /* Insert the i-th key-value pair */
        Value *entry = NULL;
        bool success = hashmap_insert(map, key_val, key_val, &entry);
        ASSERT(success);
        ASSERT(entry != NULL);
        ASSERT(entry == key_val);

        /* Verify that the entry is indeed in the map */
        entry = hashmap_get(map, key_val);
        ASSERT(entry != NULL);
        ASSERT(entry == key_val);

        /* Verfiy the size of the map */
        size_t size = hashmap_size(map);
        ASSERT(size == i + 1);
    }

    /* Remove the entries again */
    for (unsigned int i = 0; i < n; ++i) {
        /* Remove the i-th key-value pair */
        HashmapEntry removed_entry;
        bool success = hashmap_remove(map, &i, &removed_entry);
        ASSERT(success);
        ASSERT(removed_entry.key != NULL);
        ASSERT(*(unsigned int *)removed_entry.key == i);
        ASSERT(removed_entry.value != NULL);
        ASSERT(*(unsigned int *)removed_entry.value == i);
        ASSERT(removed_entry.key == removed_entry.value);

        /* We only need to free once because the key and value are the same */
        free(removed_entry.key);

        /* Verify that the entry is no longer in the map */
        Value *got = hashmap_get(map, &i);
        ASSERT(got == NULL);

        /* Verify the size of the map */
        size_t size = hashmap_size(map);
        ASSERT(size == n - i - 1);
    }

    /* We don't need to free because there are no entries left */
    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

/**
 * Convert an unsigned integer to a string.
 *
 * The caller is responsible for freeing the returned string.
 * If the conversion fails, NULL is returned.
 */
static char *uint_to_string(unsigned int n) {
    int length = snprintf(NULL, 0, "%d", n);
    assert(length > 0);
    char *key = malloc((size_t)length + 1);
    if (key == NULL) {
        return NULL;
    }
    int num_written = snprintf(key, (size_t)length + 1, "%d", n);
    assert(num_written == length);

    return key;
}

static result_t insert_get_remove_n(unsigned int n) {
    Hashmap *map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    /* First, we insert n key-value pairs */
    for (unsigned int i = 0; i < n; ++i) {
        char *key = uint_to_string(i);
        ASSERT(key != NULL);
        unsigned int *value = malloc(sizeof(i));
        ASSERT(value != NULL);
        *value = i;

        /* Insert the i-th key-value pair */
        Value *entry = NULL;
        bool success = hashmap_insert(map, key, value, &entry);
        ASSERT(success);
        ASSERT(entry != NULL);
        ASSERT(entry == value);

        /* We check that the entry is indeed in the map */
        entry = hashmap_get(map, key);
        ASSERT(entry != NULL);
        ASSERT(entry == value);

        /* And also the size of the map */
        size_t size = hashmap_size(map);
        ASSERT(size == i + 1);
    }

    /* The following to keys should not be in the map */
    char *n_key = uint_to_string(n);
    ASSERT(n_key != NULL);
    Value *n_val = hashmap_get(map, n_key);
    ASSERT(n_val == NULL);
    free(n_key);

    char *non_existent_key = "non-existent";
    Value *non_existent_val = hashmap_get(map, non_existent_key);
    ASSERT(non_existent_val == NULL);

    /* We check that we can get all the keys we inserted */
    for (unsigned int i = 0; i < n; ++i) {
        char *key = uint_to_string(i);
        ASSERT(key != NULL);

        Value *got = hashmap_get(map, key);
        ASSERT(got != NULL);
        ASSERT(*(unsigned int *)got == i);

        free(key);
    }

    /* We try to insert the keys again */
    unsigned int *value = malloc(sizeof(n));
    ASSERT(value != NULL);
    *value = n;
    for (unsigned int i = 0; i < n; ++i) {
        char *key = uint_to_string(i);
        ASSERT(key != NULL);

        /* Insert the i-th key-value pair.
         * This should fail because the key is already in the map. */
        unsigned int *entry = NULL;
        bool success = hashmap_insert(map, key, value, (Value **)&entry);
        ASSERT(!success);
        ASSERT(entry != NULL);
        ASSERT(entry != value);
        ASSERT(*entry == i);

        free(key);

        /* This should not change the size of the map */
        size_t size = hashmap_size(map);
        ASSERT(size == n);
    }
    free(value);

    /* We remove every second key from the map */
    for (unsigned int i = 0; i < n; i += 2) {
        char *key = uint_to_string(i);
        ASSERT(key != NULL);
        HashmapEntry removed_entry;

        /* Remove the i-th key-value pair */
        bool success = hashmap_remove(map, key, &removed_entry);
        ASSERT(success);

        ASSERT(removed_entry.key != NULL);
        ASSERT(removed_entry.key != key);
        ASSERT(strcmp(removed_entry.key, key) == 0);

        ASSERT(removed_entry.value != NULL);
        ASSERT(*(unsigned int *)removed_entry.value == i);

        free(removed_entry.key);
        free(removed_entry.value);

        /* After removing the key-value pair, we should not be able to get the value */
        Value *get_val = hashmap_get(map, key);
        ASSERT(get_val == NULL);

        /* The size of the map should be decreased by one */
        size_t size = hashmap_size(map);
        ASSERT(size == n - (i / 2) - 1);

        free(key);
    }

    /* Then, we free the map */
    hashmap_destroy(map, free, free);

    return SUCCESS;
}

int main(void) {
    unsigned int num_successful = 0;
    unsigned int num_total = 0;

    TEST(create_destroy());
    TEST(insert_get_remove_once());
    TEST(get_from_empty());
    TEST(remove_from_empty());

    TEST(insert_remove_colliding(0));
    TEST(insert_remove_colliding(1));
    TEST(insert_remove_colliding(2));
    TEST(insert_remove_colliding(3));
    TEST(insert_remove_colliding(7));
    TEST(insert_remove_colliding(20));
    TEST(insert_remove_colliding(100));
#ifndef CONSISTENCY_CHECKS
    /* These tests are very slow with consistency checks enabled */
    TEST(insert_remove_colliding(500));
#endif

    TEST(insert_get_remove_n(0));
    TEST(insert_get_remove_n(1));
    TEST(insert_get_remove_n(10));
    TEST(insert_get_remove_n(100));
    TEST(insert_get_remove_n(500));
#ifndef CONSISTENCY_CHECKS
    /* These tests are very slow with consistency checks enabled */
    TEST(insert_get_remove_n(1000));
    TEST(insert_get_remove_n(10000));
    TEST(insert_get_remove_n(100000));
#endif

    if (num_successful == num_total) {
        fprintf(stderr, COLOR_STRING("All tests passed (%d/%d)\n", GREEN), num_successful, num_total);
        return 0;
    } else {
        fprintf(stderr, COLOR_STRING("Some tests failed (%d/%d passed)\n", RED), num_successful, num_total);
        return 1;
    }
}
