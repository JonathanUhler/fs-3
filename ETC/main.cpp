//
// Created by wangd on 10/8/2024.
//

#if !DEVICE_CAN
#error[NOT_SUPPORTED] CAN not supported for this target
#endif

#include "mbed.h"
#include "src/can_wrapper.h"
#include "src/etc_controller.h"
#include "src/console.h"
#include <string>
#include <sstream>

EventFlags global_events;
ETCController* etc_handle;
CANWrapper* can_handle;

/**
 * Thread function that waits on global_events
 * Processes CAN Events
 * while(there's stuff in EventFlags):
 * Do the corresponding thing (defined in can wrapper class)
 * @return 1 if error
 */
void do_can_processing() {
    while (true) {
        // Wait for any event flag to be set (defined in the can wrapper class)
        uint32_t triggered_flags =
            global_events.wait_any(can_handle->THROTTLE_FLAG | can_handle->STATE_FLAG |
                                   can_handle->SYNC_FLAG | can_handle->RX_FLAG);

        /* Check for every event, process and then clear the corresponding flag */
        if (triggered_flags & can_handle->THROTTLE_FLAG) {
            can_handle->sendThrottle();
            global_events.clear(can_handle->THROTTLE_FLAG);
        }
        if (triggered_flags & can_handle->STATE_FLAG) {
            can_handle->sendState();
            global_events.clear(can_handle->STATE_FLAG);
        }
        if (triggered_flags & can_handle->SYNC_FLAG) {
            can_handle->sendSync();
            global_events.clear(can_handle->SYNC_FLAG);
        }
        if (triggered_flags & can_handle->RX_FLAG) {
            can_handle->processCANRx();
            global_events.clear(can_handle->RX_FLAG);
        }
    }
}

/**
 * Initialize ETC and CAN Wrapper
 * while (true):
 * update state
 * process received messages
 * @return 1 if error
 */
int main() {
    AnalogIn HE1(PA_0);
    AnalogIn HE2(PB_0);

    etc_handle = new ETCController();
    can_handle = new CANWrapper(*etc_handle, global_events);

    Thread high_priority_thread(osPriorityHigh);
    high_priority_thread.start(do_can_processing);

    Console::INSTANCE.Write("> ", false);

    bool read_from_sensors = true;
    float he2_read;
    while (true) {
        if (read_from_sensors) {
            he2_read = HE2.read();
        }

        bool has_message = Console::INSTANCE.Read();
        if (has_message) {
            std::istringstream message_stream(Console::INSTANCE.GetInput());
            std::string opcode;
            message_stream >> opcode;

            if (opcode == "setv") {
                read_from_sensors = false;
                message_stream >> he2_read;
                he2_read = (he2_read * etc_handle->VOLT_SCALE_he2) / etc_handle->MAX_V;
            }
            else if (opcode == "setp") {
                read_from_sensors = false;
                message_stream >> he2_read;
                he2_read = (he2_read / 100.0 * 4.000) + 0.500;
                he2_read = he2_read * etc_handle->VOLT_SCALE_he2 / etc_handle->MAX_V;
            }
            else if (opcode == "start") {
                ETCState state = etc_handle->getState();
                state.ts_ready = true;
                state.brakes_read = etc_handle->BRAKE_TOL;
                etc_handle->updateStateFromCAN(state);
                etc_handle->checkStartConditions();
            }
            else if (opcode == "reset") {
                read_from_sensors = true;
                etc_handle->resetState();
            }
            else if (opcode == "info") {
                Console::INSTANCE.Write("console");
                Console::INSTANCE.Write("  debug: " +
                                        std::to_string(Console::INSTANCE.IsDebugEnabled()));
                Console::INSTANCE.Write("state");
                Console::INSTANCE.Write("  mbb_alive:     " +
                                        std::to_string(etc_handle->getMBBAlive()));
                Console::INSTANCE.Write("  brakes_read:   " +
                                        std::to_string(etc_handle->getBrakes()));
                Console::INSTANCE.Write("  he2_read:      " +
                                        std::to_string(etc_handle->getHE2Read()));
                Console::INSTANCE.Write("  he2_travel:    " +
                                        std::to_string(etc_handle->getHE2Travel()));
                Console::INSTANCE.Write("  pedal_travel:  " +
                                        std::to_string(etc_handle->getPedalTravel()));
                Console::INSTANCE.Write("  torque_demand: " +
                                        std::to_string(etc_handle->getTorqueDemand()));
                Console::INSTANCE.Write("  motor_forward: " +
                                        std::to_string(etc_handle->isMotorForward()));
                Console::INSTANCE.Write("  motor_enabled: " +
                                        std::to_string(etc_handle->isMotorEnabled()));
                Console::INSTANCE.Write("  ts_ready:      " +
                                        std::to_string(etc_handle->isTSReady()));
                Console::INSTANCE.Write("  cockpit:       " +
                                        std::to_string(etc_handle->isCockpit()));
            }
            else if (opcode == "debug") {
                Console::INSTANCE.ToggleDebug();
            }
            else if (opcode == "help") {
                Console::INSTANCE.Write("commands");
                Console::INSTANCE.Write("  setv <he2>  set hall-effect sensor voltages.");
                Console::INSTANCE.Write("  setp <he2>  set hall-effect travel percent.");
                Console::INSTANCE.Write("  start       sets the motor start conditions.");
                Console::INSTANCE.Write("  reset       reset the ETC controller firmware.");
                Console::INSTANCE.Write("  info        print ETC state values.");
                Console::INSTANCE.Write("  debug       toggle debug messages.");
                Console::INSTANCE.Write("  help        print this message.");
            }
            else {
                Console::INSTANCE.Write("error: unknown command. see 'help' for more information");
            }

            Console::INSTANCE.Write("> ", false);
        }

        etc_handle->updatePedalTravel(0.0f, he2_read);
    }

    return 0;
}
