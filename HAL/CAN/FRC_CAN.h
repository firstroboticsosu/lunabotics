#pragma once

/**
 * @file FRC_CAN.h
 * @author Noah Charlton (charlton.78@osu.edu)
 * @brief Library to send FRC CAN frames with the MCP 2515 chip (used on the RP2040 Feather board)
 * 
 * A reference for the FRC CAN specification is located here: https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
 */

#include <stdint.h>
#include <stddef.h>
#include "../HAL.h"

#define CAN_FRAME_DATA_LENGTH 8

typedef enum frc_can_device_id { 
    CAN_DEVICE_ID_INTAKE_MOTOR = 10,
    CAN_DEVICE_ID_DUMP_MOTOR = 11,
    CAN_DEVICE_ID_PDP = 12,
    CAN_DEVICE_ID_NONE = 255,
} frc_can_device_id_t;

typedef enum frc_can_manufacturer {
    CAN_MANUFACTURER_CTRE = 4,
    CAN_MANUFACTURER_REV = 5,
} frc_can_manufacturer_t;

typedef enum frc_can_device_type { 
    CAN_DEVICE_TYPE_BROADCAST = 1,
    CAN_DEVICE_TYPE_MOTOR_CONTROLLER = 2,
    CAN_DEVICE_TYPE_POWER_DISTRIBUTION = 8,
} frc_can_device_type_t;

typedef struct frc_can_frame {
    enum frc_can_manufacturer manufacturer;
    enum frc_can_device_type device_type;
    enum frc_can_device_id device_id;
    uint8_t api_class;
    uint8_t api_index;
    uint8_t data[CAN_FRAME_DATA_LENGTH];
} frc_can_frame_t;

typedef void (*frc_can_driver_on_update)();
typedef void (*frc_can_driver_on_receive)();

typedef struct frc_can_driver {
    enum frc_can_manufacturer manufacturer;
    enum frc_can_device_type device_type;
    frc_can_driver_on_update on_update;
    frc_can_driver_on_receive on_recv;
} frc_can_driver_t;

/**
 * @brief Initializes the CAN driver
 * 
 * @param drivers 
 * @param driver_count 
 */
void frc_can_init(frc_can_driver_t **drivers, size_t driver_count);

/**
 * @brief Establishes a connection on the CAN bus. 
 * 
 * @return `HAL_OK` if the CAN bus is established, `HAL_ERR` otherwise. 
 */
hal_ret_code_t frc_can_start();

/**
 * @brief Updates the CAN drivers  
 */
void frc_can_update();

/**
 * @brief Sends a CAN frame using FRC.
 * 
 * @param frame the frame to send
 */
void frc_can_send(frc_can_frame_t *frame);