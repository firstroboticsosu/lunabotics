#pragma once
#include "communicator.h"
#include "robotActuation.h"
#include "structs.h"




class RobotControl{
private:
    double batteryVoltage;
    //Communicator* communicator;

    RobotActuation* robotActuation;

    RobotState desiedState;
    RobotState trueState;


    /**
     * @brief Holds the state the robot is currently in
     *   0 = robotStartup
     *   1 = stopState
     *   2 = teleopState
     *   3 = autoState (Not yet defined)
     *   -1 = panic!
     */
    int currentState;
    bool running;

public:

    int robotStartup();

    int robotRunStateMachine();

    int runStartupState();

    int runStopState();

    int runTeleopState();

    int runAutoState();

    int runPanicState();

    int setDriveValues(int8_t flVal, int8_t frVal, int8_t blVal, int8_t brVal);

    int setIntakeSpeed(int8_t val);

    int setDumpSpeed(int8_t val); 

    int setIntakePosition(int8_t val);
};
