#ifndef _TEST_UPDATE_MBB_ALIVE_H_
#define _TEST_UPDATE_MBB_ALIVE_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"
#include "mbed.h"
#include <cstdint>


void TestUpdateMbbAlive() {
    for (size_t i = 0; i < 16; i++) {
        uint8_t curr = etcController->getMBBAlive();
        etcController->updateMBBAlive();
        uint8_t next = etcController->getMBBAlive();

        TEST_ASSERT_EQUAL(next, curr + 1);
    }

    etcController->updateMBBAlive();
    TEST_ASSERT_EQUAL(etcController->getMBBAlive(), 0);
}


#endif  // _TEST_UPDATE_MBB_ALIVE_H_
