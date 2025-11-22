#include "./data-structures/HashMapTests.h"
#include <stdio.h>
#include <assert.h>

int RunAllTests() {
    // Unit-Tests
    unit_test_put_succes();
    unit_test_get_success();
    unit_test_get_not_found_01();
    unit_test_get_not_found_02();
    unit_test_delete_success();

    // E2E-Tests
    end_to_end_test_01();
    end_to_end_test_02();
    end_to_end_test_03();

    printf("\033[0;34mFinished tests\033[0m\n");
}

int main(void) {
    return RunAllTests();
}