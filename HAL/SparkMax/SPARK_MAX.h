#pragma once
/**
 * @file SPARK_MAX.h
 * @brief A driver for controlling the SPARK MAX motor controller via the CAN BUS. 
 */

#include "../CAN/FRC_CAN.h"

typedef enum spark_max {
    SPARK_MAX_INTAKE,
    SPARK_MAX_DUMP,
    SPARK_MAX_COUNT,
} spark_max_t;

/**
 * Voltage Control Mode :       0
 * Speed Control Mode :         1
 * Voltage Compensation Mode :  2
 * Position Control Mode :      3
 * Current Control Mode :       4
 * Status :                     5
 * Periodic Status :            6
 * Configuration :              7
 * Ack :                        8
 */
typedef enum motor_controller_api_class {
    VOLTAGE_CONTROL_MODE = 0,
    SPEED_CONTROL_MODE = 1,
    VOLTAGE_COMPENSATION_MODE = 2,
    POSITION_CONTROL_MODE = 3,
    CURRENT_CONTROL_MODE = 4,
    STATUS = 5,
    PERIODIC_STATUS = 6,
    CONFIGURATION = 7,
    ACK = 8
} motor_controller_api_class_t;

/**
 * Enable Control               : 0
 * Disable Control              : 1
 * Set Setpoint                 : 2
 * P Constant                   : 3
 * I Constant                   : 4
 * Set Reference                : 5
 * D Constant                   : 6
 * Trusted Enable               : 7
 * Trusted Set No Ack           : 8
 * Trusted Set Setpoint No Ack  : 10
 * Set Setpoint No Ack          : 11
 */
typedef enum motor_controller_api_index {
    ENABLE_CONTROL = 0,
    DISABLE_CONTROL = 1,
    SET_SETPOINT = 2,
    P_CONSTANT = 3,
    I_CONSTANT = 4,
    D_CONSTANT = 5,
    SET_REFERENCE = 6,
    TRUSTED_ENABLE = 7,
    TRUSTED_SET_NO_ACK = 8,
    // somehow there is no 9
    TRUSTED_SET_SETPOINT_NO_ACK = 10,
    SET_SETPOINT_NO_ACK = 11
} motor_controller_api_index_t;

void spark_max_disable();
void spark_max_enable();

/**
 * @brief Sets the speed of a Spark MAX motor
 * 
 * @param id Which SPARK MAX controller to set the speed of
 * @param speed The speed for the motor controller (100=Full forward, -100=Full reverse, 0=Stop)
 */
void spark_max_set_speed(spark_max_t id, int8_t speed);

/**
 * @brief Sets the speed for the SPARK MAX to follow. 
 * 
 * @param follow the ID of the SPARK max to follow
 */
void spark_max_follow(uint8_t follower, uint8_t following);

struct frc_can_driver *spark_max_get_driver();