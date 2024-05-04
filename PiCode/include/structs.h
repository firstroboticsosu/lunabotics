#pragma once
#include <cstdint>

struct RobotState {
    //These values are 0 - 4095 (12 bits). Thse actual angle is loc*360/4096
    uint16_t intakeLocation;

    // All motors are -100 to 100
    // Driver motors
    int8_t flMotor;
    int8_t frMotor;
    int8_t brMotor;
    int8_t blMotor;

    int8_t intakeMotor;
    int8_t dumpMotor;
    bool robot_enabled;
};