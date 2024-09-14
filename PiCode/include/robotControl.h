#pragma once
#include "driverStation.h"
#include "robotActuation.h"
#include "robotState.h"

enum AutoState { 
    AUTO_STATE_NONE = 0,
    AUTO_STATE_SLEEP,
    AUTO_STATE_LOWER_INTAKE,
    AUTO_STATE_DIG,
    AUTO_STATE_RAISE_INTAKE,
    AUTO_STATE_COMPLETE
};

class RobotControl{
private:
    RobotState currentState;
    RobotState lastStateSent;

    // Variables to keep track of how long since we have sent something to the RP2040
    uint64_t lastHeartbeat = 0;
    uint64_t lastDriveCmd = 0;
    uint64_t lastDumpCmd = 0;
    uint64_t lastIntakeCmd = 0;
    uint64_t lastDeployCmd = 0;

    AutoState autoState = AUTO_STATE_NONE;
    uint64_t lastAutoStageStart = 0;

public:

    void update();

    void sendStateToRP2040(RobotActuation *rp2040);

    void handleGamepadPacket(GamepadPacket packet);

    void handleDsHeartbeatPacket(DsHeartbeatPacket packet);

    void startAutoDig();

    void disableRobot();

    RobotState& getRobotState();

    AutoState getAutoState();

private:

    void changeAutoState(AutoState newState);

    // int robotStartup();

    // int robotRunStateMachine();

    // int runStartupState();

    // int runStopState();

    // int runTeleopState();

    // int runAutoState();

    // int runPanicState();

    // int setDriveValues(int8_t flVal, int8_t frVal, int8_t blVal, int8_t brVal);

    // int setIntakeSpeed(int8_t val);

    // int setDumpSpeed(int8_t val); 

    // int setIntakePosition(int8_t val);
};
