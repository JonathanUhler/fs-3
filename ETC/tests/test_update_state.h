#ifndef _TEST_UPDATE_STATE_H_
#define _TEST_UPDATE_STATE_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"


void ImplausibilityTestHelper(float he1_read, float he2_read) {
    ETCState state = {.motor_enabled = true};
    etcController->updateStateFromCAN(state);

    etcController->updateState(he1_read, he2_read);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());

    ThisThread::sleep_for(50ms);
    etcController->updateState(he1_read, he2_read);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());

    ThisThread::sleep_for(60ms);
    etcController->updateState(he1_read, he2_read);
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
}


void TestHEMismatch() {
    ImplausibilityTestHelper(0.0f, 1.0f);
}


void TestHETemporaryMismatch() {
    ETCState state = {.motor_enabled = true};
    etcController->updateStateFromCAN(state);

    etcController->updateState(0.0f, 1.0f);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());

    ThisThread::sleep_for(50ms);
    etcController->updateState(etcController->VOLT_SCALE_he1 / 2.0,
                               etcController->VOLT_SCALE_he2 / 2.0);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());

    ThisThread::sleep_for(60ms);
    etcController->updateState(etcController->VOLT_SCALE_he1 / 2.0,
                               etcController->VOLT_SCALE_he2 / 2.0);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());
}


void TestHEVoltageRange() {
    ImplausibilityTestHelper(0.0f, 0.0f);
    ImplausibilityTestHelper(1.0f, 1.0f);
}


#endif  // _TEST_UPDATE_STATE_H_
