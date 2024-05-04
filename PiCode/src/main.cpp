
#include <iostream>
#include <string>

#include "driverStation.h"
#include "robotActuation.h"
#include "robotControl.h"
#include "robotState.h"
#include "util.h"

#define DS_HEARTBEAT_RATE_MS 500
#define DS_TIMEOUT_MS 1000

bool shutdown_flag = false;

void handle_shutdown_signal(int signal) { shutdown_flag = true; }

bool handleDsMessages(DsCommunicator &dsComms, RobotControl &control) {
    DsPacket dsPacket;
    bool anyMessageRecv = false;
    while (dsComms.receiveMessage(&dsPacket)) {
        anyMessageRecv = true;
        DsPacketType type = dsPacket.getType();

        if (type == DS_PACKET_HEARTBEAT) {
            DsHeartbeatPacket heartbeatPacket(dsPacket);
            control.handleDsHeartbeatPacket(heartbeatPacket);
        } else if (type == DS_PACKET_GAMEPAD) {
            GamepadPacket gamepadPacket(dsPacket);
            control.handleGamepadPacket(gamepadPacket);
        }
    }

    return anyMessageRecv;
}

int main() {
    std::cout << "Initializing robot code " << std::endl;
    DsCommunicator dsComms;
    RobotActuation rp2040("/dev/ttyACM0", 115200);
    RobotControl control;
    std::cout << "Robot code initialized!" << std::endl;

    struct sigaction act;
    act.sa_handler = handle_shutdown_signal;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    uint64_t lastSentDsHearbeat = 0;
    uint64_t lastDsMessageRx = getUnixTimeMs();
    while (!shutdown_flag) {
        uint64_t currentTime = getUnixTimeMs();

        // Driver Station Comms
        dsComms.update();
        if (dsComms.isConnected()) {
            if (currentTime - lastSentDsHearbeat > DS_HEARTBEAT_RATE_MS) {
                dsComms.sendHeartbeat(control.getRobotState().isRobotEnabled(), rp2040.isConnected());
                lastSentDsHearbeat = currentTime;
            }

            if (handleDsMessages(dsComms, control)) {
                lastDsMessageRx = getUnixTimeMs();
            } else if (currentTime - lastDsMessageRx > DS_TIMEOUT_MS) {
                std::cout << "DS has not sent a message for " << DS_TIMEOUT_MS << "ms. Killing connection" << std::endl;
                dsComms.close();
            }
        } else {
            control.disableRobot();
            lastDsMessageRx = currentTime;
        }

        // RP2040 Comms
        control.sendStateToRP2040(&rp2040);
    }

    std::cout << "Shutting down robot code..." << std::endl;
    dsComms.close();
    std::cout << "Robot code shutdown. Goodbye!" << std::endl;

    return 0;
}
