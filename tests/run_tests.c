
#include <hashmap.h>

#include <stdio.h>

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

#define TEST(name) \
    num_total += 1; \
    if (name() == SUCCESS) { \
        num_successful += 1; \
    } else { \
        fprintf(stderr, "Test " COLOR_STRING("%s", RED) " in " COLOR_STRING("%s", RED) " failed\n", #name, __FILE__); \
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

int main(void) {
    unsigned int num_successful = 0;
    unsigned int num_total = 0;

    TEST(create_destroy);
    TEST(insert_get_remove_once);
    TEST(get_from_empty);
    TEST(remove_from_empty);

    if (num_successful == num_total) {
        fprintf(stderr, COLOR_STRING("All tests passed (%d/%d)\n", GREEN), num_successful, num_total);
        return 0;
    } else {
        fprintf(stderr, COLOR_STRING("Some tests failed (%d/%d passed)\n", RED), num_successful, num_total);
        return 1;
    }
}
