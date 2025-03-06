#ifndef _TEST_RESET_STATE_H_  /* TODO: Rename the header guard macro to match the file name. */
#define _TEST_RESET_STATE_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"


void TestResetStateFromFilled() {
    ETCState state = {
        .mbb_alive = 1,
        .he1_read = 1.0f,
        .he2_read = 1.0f,
        .he1_travel = 1.0f,
        .he2_travel = 1.0f,
        .pedal_travel = 1.0f,
        .brakes_read = 1.0f,
        .ts_ready = true,
        .motor_enabled = true,
        .motor_forward = false,
        .cockpit = true,
        .torque_demand = 1
    };

    etcController->updateStateFromCAN(state);
    etcController->resetState();

    TEST_ASSERT_EQUAL(etcController->getMBBAlive(),     0);
    TEST_ASSERT_EQUAL(etcController->getHE1Read(),      0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE2Read(),      0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE1Travel(),    0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE2Travel(),    0.0f);
    TEST_ASSERT_EQUAL(etcController->getPedalTravel(),  0);
    TEST_ASSERT_EQUAL(etcController->getBrakes(),       0.0f);
    TEST_ASSERT_FALSE(etcController->isTSReady());
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
    TEST_ASSERT_TRUE(etcController->isMotorForward());
    TEST_ASSERT_FALSE(etcController->isCockpit());
    TEST_ASSERT_EQUAL(etcController->getTorqueDemand(), 0);
}


void TestResetStateFromUnfilled() {
    ETCState state = {0};

    etcController->updateStateFromCAN(state);
    etcController->resetState();

    TEST_ASSERT_EQUAL(etcController->getMBBAlive(),     0);
    TEST_ASSERT_EQUAL(etcController->getHE1Read(),      0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE2Read(),      0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE1Travel(),    0.0f);
    TEST_ASSERT_EQUAL(etcController->getHE2Travel(),    0.0f);
    TEST_ASSERT_EQUAL(etcController->getPedalTravel(),  0);
    TEST_ASSERT_EQUAL(etcController->getBrakes(),       0.0f);
    TEST_ASSERT_FALSE(etcController->isTSReady());
    TEST_ASSERT_FALSE(etcController->isMotorEnabled());
    TEST_ASSERT_TRUE(etcController->isMotorForward());
    TEST_ASSERT_FALSE(etcController->isCockpit());
    TEST_ASSERT_EQUAL(etcController->getTorqueDemand(), 0);
}


#endif  // _TEST_RESET_STATE_H_
