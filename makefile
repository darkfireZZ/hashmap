
BUILD_DIR := build
SRC_DIR := src
TEST_SRC_DIR := tests

CC := gcc

BUILD ?= release

CFLAGS := -Werror -std=c99 -Iinclude
CFLAGS_RELEASE := -O3
CFLAGS_DEBUG := -g -O0

ifeq ($(BUILD), release)
	CFLAGS += $(CFLAGS_RELEASE)
else ifeq ($(BUILD), debug)
	CFLAGS += $(CFLAGS_DEBUG)
else
$(error BUILD must be either release or debug)
endif

$(info building in $(BUILD) mode)

include cflax.mk
CFLAGS += $(CFLAX)

.PHONY: build
build: $(BUILD_DIR)/run_tests

.PHONY: test
test: $(BUILD_DIR)/run_tests
	$(BUILD_DIR)/run_tests

$(BUILD_DIR)/run_tests: $(BUILD_DIR)/hashmap_test.o $(BUILD_DIR)/run_tests.o
	$(CC) $^ -o $@

$(BUILD_DIR)/run_tests.o: $(TEST_SRC_DIR)/run_tests.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_INCLUDES) -c $< -o $@

$(BUILD_DIR)/hashmap_test.o: $(SRC_DIR)/hashmap.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
