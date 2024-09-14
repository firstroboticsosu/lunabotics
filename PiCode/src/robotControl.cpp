#include "robotControl.h"
#include "util.h"

#define HEARTBEAT_RATE_MS 1000
#define MOTOR_UPDATE_RATE_MS 1000
#define INTAKE_SPEED 75
#define DUMP_SPEED 100
#define CRAWL_SPEED 40
#define DEPLOY_LOWER_SPEED -15
#define DEPLOY_RAISE_SPEED 65
#define TRIGGER_DEADZONE 10

#define AUTO_SLEEP_TIME_MS 5000

#define AUTO_DROP_INTAKE_TIME_MS 200
#define AUTO_DROP_INTAKE_SPEED 40

#define AUTO_DIG_TIME 5000
#define AUTO_DIG_INTAKE_SPEED 75
#define AUTO_DIG_DELAY 500

#define AUTO_RAISE_INTAKE_TIME 1000
#define AUTO_RAISE_INTAKE_SPEED -100

#define AUTO_COMPLETE_TIME 5000
#define AUTO_COMPLETE_INTAKE_SPEED 50

void RobotControl::sendStateToRP2040(RobotActuation *rp2040) {
    uint64_t currentTime = getUnixTimeMs();

    if (currentTime - this->lastHeartbeat > HEARTBEAT_RATE_MS) {
        std::cout << "Send Heartbeat to RP2040" << std::endl;
        rp2040->sendHeartbeat();
        lastHeartbeat = currentTime;
    }

    bool frontLeftChange = currentState.getDriveFrontLeft() != lastStateSent.getDriveFrontLeft();
    bool frontRightChange = currentState.getDriveFrontRight() != lastStateSent.getDriveFrontRight();
    bool backLeftChange = currentState.getDriveBackLeft() != lastStateSent.getDriveBackLeft();
    bool backRightChange = currentState.getDriveBackRight() != lastStateSent.getDriveBackRight();

    if (frontLeftChange || frontRightChange || backLeftChange || backRightChange ||
        currentTime - this->lastDriveCmd > MOTOR_UPDATE_RATE_MS) {
        // std::cout << "Send Drive to RP2040 (FL=" << +currentState.getDriveFrontLeft() << ", FR=" <<
        // +currentState.getDriveFrontRight()
        //           << ", BL=" << +currentState.getDriveBackLeft() << ", BR=" << +currentState.getDriveBackRight() <<
        //           ")" << std::endl;
        rp2040->sendDriveMotors(currentState.getDriveFrontLeft(), currentState.getDriveFrontRight(),
                                currentState.getDriveBackLeft(), currentState.getDriveBackRight());
        lastDriveCmd = currentTime;
    }

    if (currentState.getIntake() != lastStateSent.getIntake() || currentTime - lastIntakeCmd > MOTOR_UPDATE_RATE_MS) {
        // std::cout << "Send Intake to RP2040: " << +currentState.getIntake() << std::endl;
        rp2040->sendIntakeMotor(currentState.getIntake());
        lastIntakeCmd = currentTime;
    }

    if (currentState.getDump() != lastStateSent.getDump() || currentTime - lastDumpCmd > MOTOR_UPDATE_RATE_MS) {
        // std::cout << "Send Dump to RP2040: " << +currentState.getDump() << std::endl;
        rp2040->sendDumpMotor(currentState.getDump());
        lastDumpCmd = currentTime;
    }

    if (currentState.getDeploy() != lastStateSent.getDeploy() || currentTime - lastDeployCmd > MOTOR_UPDATE_RATE_MS) {
        // std::cout << "Send Deploy to RP2040: " << +currentState.getDeploy() << std::endl;
        rp2040->sendDeployControl(currentState.getDeploy());
        lastDeployCmd = currentTime;
    }

    lastStateSent = currentState;
}

void RobotControl::handleGamepadPacket(GamepadPacket packet) {
    if (currentState.getRobotMode() == ROBOT_MODE_AUTO_EXCAVATE) {
        if (packet.isButtonAPressed() || packet.isButtonBPressed() || packet.isButtonXPressed() ||
            packet.isButtonYPressed()) {
            std::cout << "Switch to TeleOp" << std::endl;
            currentState.setMode(ROBOT_MODE_TELEOP);
            currentState.stopAll();
        } else {
            return;
        }
    }

    // Drive
    int leftDriveSpeed = packet.getLeftStickY();
    int rightDriveSpeed = packet.getRightStickY();
    if (packet.isDpadUp()) {
        leftDriveSpeed = CRAWL_SPEED;
        rightDriveSpeed = CRAWL_SPEED;
    } else if (packet.isDpadDown()) {
        leftDriveSpeed = -CRAWL_SPEED;
        rightDriveSpeed = -CRAWL_SPEED;
    }
    currentState.setDrive(leftDriveSpeed, rightDriveSpeed);

    // Intake
    if (packet.isButtonAPressed()) {
        currentState.setIntake(INTAKE_SPEED);
    } else {
        currentState.setIntake(0);
    }

    // Dump
    if (packet.isButtonBPressed()) {
        currentState.setDump(DUMP_SPEED);
    } else {
        currentState.setDump(0);
    }

    // Deploy
    int deploySpeed = 0;

    if (packet.isLeftBumperPressed()) {
        deploySpeed = DEPLOY_RAISE_SPEED;
    } else if (packet.issRightBumperPressed()) {
        deploySpeed = DEPLOY_LOWER_SPEED;
    } else if (packet.getLeftTrigger() > TRIGGER_DEADZONE) { // Lower (loosen)
        deploySpeed = packet.getLeftTrigger();
    } else if (packet.getRightTrigger() > TRIGGER_DEADZONE) { // Lift (Tighten)
        deploySpeed = -packet.getRightTrigger();
    }

    currentState.setDeploy((int8_t)deploySpeed);
}

void RobotControl::update() {
    if (currentState.getRobotMode() != ROBOT_MODE_AUTO_EXCAVATE) {
        return;
    }

    // Stop all motors
    currentState.stopAll();
    uint64_t duration = getUnixTimeMs() - lastAutoStageStart;

    switch (autoState) {
    case AUTO_STATE_SLEEP:
        if (duration > AUTO_SLEEP_TIME_MS) {
            changeAutoState(AUTO_STATE_LOWER_INTAKE);
        }
        break;
    case AUTO_STATE_LOWER_INTAKE:
        if (duration < AUTO_DROP_INTAKE_TIME_MS) {
            currentState.setDeploy(AUTO_DROP_INTAKE_SPEED);
        } else {
            changeAutoState(AUTO_STATE_DIG);
        }
        break;
    case AUTO_STATE_DIG:
        if (duration < AUTO_DIG_TIME) {
            if (duration > AUTO_DIG_DELAY) {
                currentState.setIntake(AUTO_DIG_INTAKE_SPEED);
            } else {
                currentState.setIntake(0);
            }
        } else {
            changeAutoState(AUTO_STATE_RAISE_INTAKE);
        }
        break;
    case AUTO_STATE_RAISE_INTAKE:
        if (duration < AUTO_RAISE_INTAKE_TIME) {
            currentState.setDeploy(AUTO_RAISE_INTAKE_SPEED);
        } else if (duration < AUTO_COMPLETE_TIME + AUTO_RAISE_INTAKE_TIME) {
            currentState.setIntake(AUTO_COMPLETE_INTAKE_SPEED);
        } else {
            changeAutoState(AUTO_STATE_COMPLETE);
        }
        break;
    case AUTO_STATE_COMPLETE:
    default:
        std::cout << "Auto complete" << std::endl;
        autoState = AUTO_STATE_NONE;
        currentState.stopAll();
        currentState.setMode(ROBOT_MODE_TELEOP);
    }
}

void RobotControl::changeAutoState(AutoState newState) {
    autoState = newState;
    lastAutoStageStart = getUnixTimeMs();
    std::cout << "Enter auto state: " << newState << std::endl;
}

void RobotControl::handleDsHeartbeatPacket(DsHeartbeatPacket packet) {
    currentState.setEnabled(packet.IsRobotEnabled());
}

void RobotControl::disableRobot() { currentState.setEnabled(false); }

void RobotControl::startAutoDig() {
    currentState.setMode(ROBOT_MODE_AUTO_EXCAVATE);
    changeAutoState(AUTO_STATE_SLEEP);
    std::cout << "Switch to Auto Dig" << std::endl;
}

RobotState &RobotControl::getRobotState() { return currentState; }

AutoState RobotControl::getAutoState() { return autoState; }

// int RobotControl::robotRunStateMachine()
// {
//     while(running) {
//         switch (currentState)
//         {
//             case 0:
//                 runStartupState();
//                 break;
//             case 1:
//                 runStopState();
//                 break;
//             case 2:
//                 runTeleopState();
//                 break;
//             case 3:
//                 runAutoState();
//             default:
//                 runPanicState();

//         }
//     }
//     return 1;
// }

// //Heart beat every seond

// int RobotControl::robotStartup()
// {
//     return robotRunStateMachine();
// }

// int RobotControl::runStartupState()
// {
//     return 1;
// }

// int RobotControl::runAutoState()
// {
//     currentState = -1;
//     return 1;
// }

// int RobotControl::runStopState()
// {
//     setDriveValues(0,0,0,0);
//     setIntakeSpeed(0);
//     setDumpSpeed(0);
//     setIntakePosition(trueState.intakeLocation);

//     return 1;
// }

// int RobotControl::runTeleopState()
// {
//     return 1;
// }

// int RobotControl::runPanicState()
// {
//     // Robot is paniced!
//     setDriveValues(0,0,0,0);
//     setIntakeSpeed(0);
//     setDumpSpeed(0);
//     setIntakePosition(trueState.intakeLocation);
//     currentState = -1;

//     return -1;
// }

// int RobotControl::setDumpSpeed(int8_t val)
// {
//     return 0;
// }

// int RobotControl::setIntakePosition(int8_t val)
// {
//     return 0;
// }

// int RobotControl::setIntakeSpeed(int8_t val)
// {
//     return 0;
// }

// int RobotControl::setDriveValues(int8_t flVal, int8_t frVal, int8_t blVal, int8_t brVal)
// {
//     return 0;
// }