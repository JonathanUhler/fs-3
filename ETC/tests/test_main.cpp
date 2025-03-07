/**
 * @file test_main.cpp
 *
 * The main runner file for ETC unit tests. (This file does not test {@code main.cpp}).
 *
 * Include test file headers in the specified section, then add test cases to {@code run_all_tests}.
 *
 * @author Jonathan Uhler
 */


// Unity allows for a `unity_config.h` header file for programmer-defined configuration. If we
// don't have/use this, we don't need to include it.
#undef UNITY_INCLUDE_CONFIG_H


// Include other test files here. Remember to add test cases to the "run_all_tests" function!
#include "test_check_start_conditions.h"
#include "test_reset_state.h"
#include "test_run_stop_rtds.h"
#include "test_update_mbb_alive.h"
#include "test_update_state_from_can.h"
#include "test_update_state.h"

// Standard headers begin here
#include "test_main.h"
#include "mbed.h"
#include "unity.h"
#include <iostream>


ETCController *etcController;


/**
 * Add programmer-defined tests here.
 */
void run_all_tests() {
    // Use the RUN_TEST(<function_name>) macro here
    RUN_TEST(TestCheckStartConditionsTsReadyOnly);
    RUN_TEST(TestCheckStartConditionsBrakesReadOnly);
    RUN_TEST(TestCheckStartConditionsNeither);
    RUN_TEST(TestCheckStartConditionsBoth);

    RUN_TEST(TestResetStateFromFilled);
    RUN_TEST(TestResetStateFromUnfilled);

    RUN_TEST(TestRunRtds);

    RUN_TEST(TestMbbAliveSet);
    RUN_TEST(TestHE1ReadSet);
    RUN_TEST(TestHE2ReadSet);
    RUN_TEST(TestHE1TravelSet);
    RUN_TEST(TestHE2TravelSet);
    RUN_TEST(TestPedalTravelSet);
    RUN_TEST(TestBrakesReadSet);
    RUN_TEST(TestTsReadySet);
    RUN_TEST(TestMotorEnabledSet);
    RUN_TEST(TestMotorForwardSet);
    RUN_TEST(TestCockpitSet);
    RUN_TEST(TestTorqueDemandSet);

    RUN_TEST(TestHEMismatch);
    RUN_TEST(TestHETemporaryMismatch);
    RUN_TEST(TestHEVoltageRange);
}


/**
 * Set up function for Unity tests.
 *
 * DO NOT MODIFY! If you are just adding new tests, read the header comment in this file.
 */
void setUp() {
    etcController = new ETCController();
}


/**
 * Teardown function for Unity tests.
 *
 * DO NOT MODIFY! If you are just adding new tests, read the header comment in this file.
 */
void tearDown() {
    delete etcController;
}


/**
 * Entry point for running tests.
 *
 * DO NOT MODIFY! If you are just adding new tests, read the header comment in this file.
 *
 * @return A zero status code if all tests pass, and non-zero if any test failed.
 */
int main() {
    UNITY_BEGIN();
    run_all_tests();
    UNITY_END();

    while (true) {
        continue;
    }

    return 0;
}
