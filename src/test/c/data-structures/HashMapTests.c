#include "HashMapTestUtils.h"

void unit_test_put_succes() {
    printf("\033[0;34mStarting Put Unit-Test\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int key = 0, value = 10;

    // Execute
    int* result = (int*)hash_map_put(hash_map, &key, &value);
    
    // Validate
    assert_lookup_index_value(hash_map, identity_function(&key), &value, &int_equals);

    // Finish
    printf("\033[0;32mPut Unit-Test passed successfully\n\n");
    hash_map_free(hash_map);
}

void unit_test_get_success() {
    printf("\033[0;34mStarting Get Unit-Test\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int key = 0, value = 10, lookup_index = 0;
    create_and_insert_lookup_entry_by_index(hash_map, lookup_index, &key, &value);

    // Execute
    int* result = hash_map_get(hash_map, &key);

    // Validate
    assert(NULL != result && "\033[0;31mGet operation failed\n");
    assert(10 == *(int*)result && "\033[0;31mGet operation failed\n");

    // Finish
    printf("\033[0;32mGet Unit-Test passed successfully\n\n");
    hash_map_free(hash_map);
}

void unit_test_get_not_found_01() {
    printf("\033[0;34mStarting Get Unit-Test not found N1\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int key = 0;

    // Execute
    int* result = (int*)hash_map_get(hash_map, &key);

    // Validate
    assert(NULL == result && "\033[0;31mGet operation failed");

    // Finish
    printf("\033[0;32mGet not found Unit-Test passed successfully\n\n");
    hash_map_free(hash_map);
}

void unit_test_get_not_found_02() {
    printf("\033[0;34mStarting Get Unit-Test not found N2\n\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int not_found_key = 0;
    int key = 1, value = 10;
    create_and_insert_lookup_entry_by_index(hash_map, 1, &key, &value);

    // Execute
    int* result = (int*)hash_map_get(hash_map, &not_found_key);

    // Validate
    assert(NULL == result && "\033[0;31mGet operation failed");

    // Finish
    printf("\033[0;32mGet not found Unit-Test passed successfully\n\n");
    hash_map_free(hash_map);
}

void unit_test_delete_success() {
    printf("\033[0;34mStarting Delete Unit-Test\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int lookup_index = 0;
    int key = 0, value = 10;

    create_and_insert_lookup_entry_by_index(hash_map, lookup_index, &key, &value);

    // Execute
    bool result = hash_map_remove(hash_map, &key);

    // Validate
    assert(true == result && "\033[0;31mRemove operation failed\n");

    // Finish
    printf("\033[0;32mDelete Unit-Test passed successfully\n\n");
    hash_map_free(hash_map);
}

void end_to_end_test_01() {
    printf("\033[0;34mStarting E2E-Test 01\n");

    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);
    int key = 1, value = 10;

// Start

    // Execute
    int* result1 = hash_map_put(hash_map, &key, &value);

    // Validate
    assert(10 == *(result1) && "\t-\033[0;31mPut operation failed\n");

    // Finish
    printf("\033[0;32m\t-Put operation success\n");

    // Execute
    void* result2 = hash_map_get(hash_map, &key);

    // Validate
    assert(NULL != result2 && "\t-\033[0;31mGet operation failed, element not found\n");
    assert(10 == *((int*)result2) && "\t-\033[0;31mGet operation failed, different result than expected\n");

    // Finish
    printf("\t-\033[0;32mGet operation success\n");

    // Execute
    bool result3 = hash_map_remove(hash_map, &key);

    // Validate
    assert(true == result3 && "\t-\033[0;31mRemove operation failed\n");

    // Finish
    printf("\t-\033[0;32mRemove operation success\n");

// Finish

    hash_map_free(hash_map);
    printf("\033[0;32mE2E-Test passed successfully\n\n");
}

void end_to_end_test_02() {
    printf("\033[0;34mStarting E2E-Test 02\n");
    int loops_amount = 10000;
    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &identity_function, &int_equals);

// Start

    for(int i = 0; i < loops_amount; i++) {
        int key = i, value = i;

        // Execute
        int* result = (int*)hash_map_put(hash_map, &key, &value);

        // Validate
        assert(i == *(result) && "\t-\033[0;31mPut operation failed\n");
        MapEntry expected = {.key = &key, .value = &value};
        assert_lookup_index_entry(hash_map, i, &expected, &int_equals);
    }

    printf("\033[0;32m\t-Put operation success\n");


    for(int i = 0; i < loops_amount; i++) {
        int key = i; int value = i;
        
        // Execute
        int* result = (int*)hash_map_get(hash_map, &key);

        // Validate
        assert(i == *result && "\t-\033[0;31mGet operation failed\n");
    }

    printf("\t-\033[0;32mGet operation success\n");


    for(int i = 0; i < loops_amount; i++) {
        int key = i, value = i;

        // Execute
        bool result = hash_map_remove(hash_map, &key);

        // Assert
        assert(true == result && "\t-\033[0;31mDelete operation failed\n");
        // Would be nice to assert the empty array slots...
    }

    printf("\t-\033[0;32mRemove operation success\n");

// Finalize

    hash_map_free(hash_map);
    printf("\033[0;32mE2E-Test passed successfully\n\n");
}

void end_to_end_test_03() {
    printf("\033[0;34mStarting E2E-Test 02\n");
    int loops_amount = 10000;
    // Setup
    HashMapADT hash_map = hash_map_new(sizeof(int), sizeof(int), &hash_to_0_mock, &int_equals);

// Start

    for(int i = 0; i < loops_amount; i++) {
        int key = i, value = i;

        // Execute
        int* result = (int*)hash_map_put(hash_map, &key, &value);

        // Validate
        assert(i == *(result) && "\t-\033[0;31mPut operation failed\n");
        MapEntry expected = {.key = &key, .value = &value};
        assert_lookup_index_entry(hash_map, i, &expected, &int_equals);
    }

    printf("\033[0;32m\t-Put operation success\n");


    for(int i = 0; i < loops_amount; i++) {
        int key = i; int value = i;
        
        // Execute
        int* result = (int*)hash_map_get(hash_map, &key);

        // Validate
        assert(i == *result && "\t-\033[0;31mGet operation failed\n");
    }

    printf("\t-\033[0;32mGet operation success\n");


    for(int i = 0; i < loops_amount; i++) {
        int key = i, value = i;

        // Execute
        bool result = hash_map_remove(hash_map, &key);

        // Assert
        assert(true == result && "\t-\033[0;31mDelete operation failed\n");
        // Would be nice to assert the empty array slots...
    }

    printf("\t-\033[0;32mRemove operation success\n");

// Finalize

    hash_map_free(hash_map);
    printf("\033[0;32mE2E-Test passed successfully\n\n");
}