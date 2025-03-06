#ifndef _TEST_RUN_STOP_RTDS_H_
#define _TEST_RUN_STOP_RTDS_H_


#include "test_main.h"
#include "unity.h"
#include "etc_controller.h"
#include "mbed.h"


void TestRunRtds() {
    DigitalOut rtds(PC_13);

    etcController->runRTDS();

    // RTDS should begin sounding after the call
    ThisThread::sleep_for(50ms);
    TEST_ASSERT_TRUE(rtds.read());

    // RTDS should sound for at least one second
    ThisThread::sleep_for(900ms);
    TEST_ASSERT_TRUE(rtds.read());

    // RTDS sould sound for no more than three seconds
    ThisThread::sleep_for(2000ms);
    TEST_ASSERT_FALSE(rtds.read());
}


#endif  // _TEST_RUN_STOP_RTDS_H_
