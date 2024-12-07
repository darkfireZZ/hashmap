
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
    if (name() == FAILURE) { \
        fprintf(stderr, "Test " COLOR_STRING("%s", RED) " in " COLOR_STRING("%s", RED) " failed\n", #name, __FILE__); \
        result = FAILURE; \
    }

#define ASSERT(condition) \
    if (!(condition)) {   \
        fprintf(stderr, "Assertion failed: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
        return FAILURE; \
    }

typedef int result_t;

static result_t create_destroy(void) {
    Hashmap map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    size_t size = hashmap_size(map);
    ASSERT(size == 0);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

static result_t insert_remove_once(void) {
    char *key = "key";
    char *value = "value";

    bool success;
    size_t size;

    Hashmap map = hashmap_create(STRING_HASHER);
    ASSERT(map != NULL);

    Value *value_entry = NULL;
    success = hashmap_insert(map, key, value, &value_entry);
    ASSERT(success);

    ASSERT(value_entry != NULL);
    ASSERT(value_entry == value);

    size = hashmap_size(map);
    ASSERT(size == 1);

    HashmapEntry removed_entry;
    success = hashmap_remove(map, key, &removed_entry);
    ASSERT(success);

    ASSERT(removed_entry.key == key);
    ASSERT(removed_entry.value == value);

    size = hashmap_size(map);
    ASSERT(size == 0);

    hashmap_destroy(map, NULL, NULL);

    return SUCCESS;
}

int main(void) {
    result_t result = SUCCESS;

    TEST(create_destroy);
    TEST(insert_remove_once);

    if (result == SUCCESS) {
        fprintf(stderr, COLOR_STRING("All tests passed\n", GREEN));
        return 0;
    } else {
        fprintf(stderr, COLOR_STRING("Some tests failed\n", RED));
        return 1;
    }
}
