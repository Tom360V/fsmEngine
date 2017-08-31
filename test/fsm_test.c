#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <string.h>

#include "fsm.h"

#define VALUE_LENGTH    (10)
#define ARRAY_SIZE      (10)
#define printf

/******************************************************************************
 * SETUPs
 *****************************************************************************/
void setup_NOT_INITIALIZED(void)
{

}

/******************************************************************************
 * TEARDOWNs
 *****************************************************************************/
void teardown(void)
{
    //nothing here
}

void teardown_another(void)
{
    //nothing here
}

/******************************************************************************
 * TESTS
 *****************************************************************************/

/*
 * Description: Check if init initialized the local variable
 */
START_TEST(TEST_app__NotInitialized)
{
    tableIdx_t result;

    result = FSM_Add(NULL, NULL);
    ck_assert_int_eq(result, -1);

    FSM_Init();

    result = FSM_Add(NULL, NULL);
    ck_assert_int_eq(result, 1);
    

    printf("\n***\n***bla***\n***\n");
}
END_TEST


/*
 * Description: Check if the set function only remembers the last set value
 */
START_TEST(TEST_app__set_2)
{
    /*ck_assert_int_eq(app_get(), 2);
    app_set(5);
    app_set(6);
    app_set(7);
    ck_assert_int_eq(app_get(), 7);
*/
}
END_TEST


/******************************************************************************
 *  MAIN FUNCTION
 */
int main(void)
{
    int number_failed = 0;
    Suite *suite   = suite_create("APP_SUITE");

    /***************************************
     * TESTCASE: First testcase
     */
    TCase *tcase1 = tcase_create("APP_CASE1");          //Create new Case
    suite_add_tcase(suite, tcase1);                     //Add Case to Suite
    tcase_add_checked_fixture(tcase1, setup_NOT_INITIALIZED, teardown); //Configure setup and teardown functions
    //tests:
    tcase_add_test(tcase1, TEST_app__NotInitialized);
//    tcase_add_test(tcase1, TEST_app__set);


    /***************************************
     * TESTCASE: another test case
     */
    /*
    TCase *tcase_another = tcase_create("APP_CASE_ANOTHER");
    suite_add_tcase(suite, tcase_another);
    tcase_add_checked_fixture(tcase_another, setup_another, teardown_another);
    //tests:
    tcase_add_test(tcase_another, TEST_app__set_2);*/


    /***************************************
     * Create runner and start the tests
     */
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_VERBOSE);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
