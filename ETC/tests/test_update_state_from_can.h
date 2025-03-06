#ifndef _TEST_UPDATE_STATE_FROM_CAN_H_
#define _TEST_UPDATE_STATE_FROM_CAN_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"


void TestMbbAliveSet() {
    ETCState state = {.mbb_alive = 1};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getMBBAlive(), 1);
}


void TestHE1ReadSet() {
    ETCState state = {.he1_read = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getHE1Read(), 1.0f);
}


void TestHE2ReadSet() {
    ETCState state = {.he2_read = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getHE2Read(), 1.0f);
}


void TestHE1TravelSet() {
    ETCState state = {.he1_travel = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getHE1Travel(), 1.0f);
}


void TestHE2TravelSet() {
    ETCState state = {.he2_travel = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getHE2Travel(), 1.0f);
}


void TestPedalTravelSet() {
    ETCState state = {.pedal_travel = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getPedalTravel(), 1.0f);
}


void TestBrakesReadSet() {
    ETCState state = {.brakes_read = 1.0f};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_EQUAL(etcController->getBrakes(), 1.0f);
}


void TestTsReadySet() {
    ETCState state = {.ts_ready = true};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_TRUE(etcController->isTSReady());
}


void TestMotorEnabledSet() {
    ETCState state = {.motor_enabled = true};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_TRUE(etcController->isMotorEnabled());
}


void TestMotorForwardSet() {
    ETCState state = {.motor_forward = true};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_TRUE(etcController->isMotorForward());
}


void TestCockpitSet() {
    ETCState state = {.cockpit = true};
    etcController->updateStateFromCAN(state);
    TEST_ASSERT_TRUE(etcController->isCockpit());
}


void TestTorqueDemandSet() {
    ETCState disabled_state = {.motor_enabled = false, .torque_demand = 1};
    etcController->updateStateFromCAN(disabled_state);
    TEST_ASSERT_EQUAL(etcController->getTorqueDemand(), 0);

    ETCState enabled_state = {.motor_enabled = true, .torque_demand = 1};
    etcController->updateStateFromCAN(enabled_state);
    TEST_ASSERT_EQUAL(etcController->getTorqueDemand(), 1);
}

#endif  // _TEST_UPDATE_STATE_FROM_CAN_H_
