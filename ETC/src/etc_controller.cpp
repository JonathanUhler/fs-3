//
// Created by wangd on 1/14/2025.
//

#include "etc_controller.h"

// TODO make the function :)))
void ETCController::updatePedalTravel(float he1_read, float he2_read) {
    /* read HE1 and HE2 sensors */

    /* calculate pedal travel using voltage divider ratio */

    /* Implausability check here*/

    /* update relevant values */

    float he2_voltage = (he2_read * MAX_V)/VOLT_SCALE_he2;

    /* convert sensor voltages into travel percentages*/
    float he2_travel_percent = (he2_voltage - 0.5f) / 4.f;

    state.he2_read = he2_read * MAX_V;
    state.he2_travel = he2_travel_percent;
    state.pedal_travel = he2_travel_percent;
    state.torque_demand = static_cast<int16_t>(static_cast<float>(MAX_TORQUE) * state.pedal_travel);
}

void ETCController::updateMBBAlive() {
    state.mbb_alive++;
    state.mbb_alive %= 16;
}

void ETCController::updateStateFromCAN(const ETCState& new_state) { state = new_state; }

// TODO make function : )
void ETCController::checkStartConditions() {
    //No brake check
        if ( state.ts_ready ) {
            state.motor_enabled = true;
        }
}

// TODO make function :)
void ETCController::runRTDS() {
    /* Run RTDS for 3 seconds */
    RTDS.write(true);
    RTDS_Timer.attach(callback([this](){this->stopRTDS();}), 1s);
}

void ETCController::stopRTDS() {
    RTDS.write(false);
    RTDS_Timer.detach();
}

void ETCController::resetState() {
    state.mbb_alive = 0;
    state.he1_read = 0.f;
    state.he2_read = 0.f;
    state.he1_travel = 0.f;
    state.he2_travel = 0.f;
    state.pedal_travel = 0.f;
    state.brakes_read = 0.f;
    state.ts_ready = false;
    state.motor_enabled = false;
    state.motor_forward = true;
    state.cockpit = false;
    state.torque_demand = 0;
}
