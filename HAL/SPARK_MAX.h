#pragma once
/**
 * @file SPARK_MAX.h
 * @brief A driver for controlling the SPARK MAX motor controller via the CAN BUS. 
 */

#include "FRC_CAN.h"

typedef enum spark_max {
    SPARK_MAX_INTAKE,
    SPARK_MAX_DUMP,
    SPARK_MAX_COUNT,
} spark_max_t;

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