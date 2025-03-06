#ifndef _TEST_CHECK_START_CONDITIONS_H_
#define _TEST_CHECK_START_CONDITIONS_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"


void TestCheckStartConditionsTsReadyOnly() {
    ETCState state = {.brakes_read = 0.0f, .ts_ready = true};
    etcController->updateStateFromCAN(state);
    etcController->checkStartConditions();
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
}


void TestCheckStartConditionsBrakesReadOnly() {
    ETCState state = {.brakes_read = etcController->BRAKE_TOL, .ts_ready = false};
    etcController->updateStateFromCAN(state);
    etcController->checkStartConditions();
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
}


void TestCheckStartConditionsNeither() {
    ETCState state = {.brakes_read = 0.0f, .ts_ready = false};
    etcController->updateStateFromCAN(state);
    etcController->checkStartConditions();
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
}


void TestCheckStartConditionsBoth() {
    ETCState state = {.brakes_read = etcController->BRAKE_TOL, .ts_ready = true};
    etcController->updateStateFromCAN(state);
    etcController->checkStartConditions();
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());
}


#endif  // _TEST_CHECK_START_CONDITIONS_H_
