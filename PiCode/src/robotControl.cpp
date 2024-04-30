#include "robotControl.h"

int RobotControl::robotRunStateMachine()
{
    while(running) {
        switch (currentState)
        {
            case 0:
                runStartupState();
                break;
            case 1:
                runStopState();
                break;
            case 2:
                runTeleopState();
                break;
            case 3:
                runAutoState();
            default:
                runPanicState();

        }
    }
    return 1;
}

//Heart beat every seond

int RobotControl::robotStartup()
{
    return robotRunStateMachine();
}

int RobotControl::runStartupState()
{
    return 1;
}


int RobotControl::runAutoState()
{
    currentState = -1;
    return 1;
}

int RobotControl::runStopState()
{
    setDriveValues(0,0,0,0);
    setIntakeSpeed(0);
    setDumpSpeed(0);
    setIntakePosition(trueState.intakeLocation);


    return 1;
}

int RobotControl::runTeleopState()
{
    return 1;
}

int RobotControl::runPanicState()
{
    // Robot is paniced!
    setDriveValues(0,0,0,0);
    setIntakeSpeed(0);
    setDumpSpeed(0);
    setIntakePosition(trueState.intakeLocation);
    currentState = -1;

    return -1;
}

int RobotControl::setDumpSpeed(int8_t val)
{
    return 0;
}

int RobotControl::setIntakePosition(int8_t val)
{
    return 0;
}

int RobotControl::setIntakeSpeed(int8_t val)
{
    return 0;
}

int RobotControl::setDriveValues(int8_t flVal, int8_t frVal, int8_t blVal, int8_t brVal)
{
    return 0;
}