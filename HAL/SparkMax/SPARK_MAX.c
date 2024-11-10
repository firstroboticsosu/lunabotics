#include "SPARK_MAX.h"
#include <stdbool.h>
#include <stdint.h>
#include "../CAN/FRC_CAN.h"

/**
 * @brief The maximum device ID for a Spark MAX.
 */
#define SPARK_MAX_ID_MAX 31
#define SPARK_MAX_COUNT 2

// Device IDs for different motors
#define CAN_DEVICE_ID_INTAKE_MOTOR 9
#define CAN_DEVICE_ID_DUMP_MOTOR 10
#define CAN_DEVICE_ID_NONE 0

// Define device descriptors for each motor
struct spark_max_desc
{
    frc_can_device_id_t device_id;
    int8_t speed; // speed in range [-100, 100]
};

struct spark_max_desc device_descriptors[SPARK_MAX_COUNT] = {
    {CAN_DEVICE_ID_INTAKE_MOTOR, 0}, // Intake motor
    {CAN_DEVICE_ID_DUMP_MOTOR, 0}    // Dump motor
};

// Enum to represent different SPARK MAX motors
typedef enum
{
    SPARK_MAX_INTAKE = 0,
    SPARK_MAX_DUMP = 1,
} spark_max_t;

// Define the CAN bus driver for SPARK MAX
struct frc_can_driver *spark_max_get_driver();

// Global lock to prevent multiple threads from controlling motors simultaneously
volatile int spark_lock = 0;

/**
 * @brief Initialize Spark MAX devices
 */
void spark_max_init()
{
    // Placeholder for initialization logic if needed
}

/**
 * @brief Set all motors' speed to zero and lock access to motors.
 */
void spark_max_disable()
{
    for (int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++)
    {
        device_descriptors[spark].speed = 0;
    }

    while (__sync_lock_test_and_set(&spark_lock, 1))
        ; // Lock access to motors
}

/**
 * @brief Unlock the motors and allow access.
 */
void spark_max_enable()
{
    __sync_lock_release(&spark_lock); // Release the lock
}

/**
 * @brief Set the speed of a specific motor (SPARK MAX).
 *
 * @param id The motor ID (e.g., SPARK_MAX_INTAKE).
 * @param speed The target speed for the motor in the range [-100, 100].
 */
void spark_max_set_speed(spark_max_t id, int8_t speed)
{
    for (int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++)
    {
        if (device_descriptors[spark].device_id == spark_max_get_id(id))
        {
            device_descriptors[spark].speed = speed;
        }
    }
}

/**
 * @brief Update motor speed (called periodically).
 *
 * This function should be called to send the updated motor speed to the hardware.
 */
static void spark_max_on_update()
{
    for (int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++)
    {
        if (spark_lock == 0)
        {
            int8_t target_speed = device_descriptors[spark].speed;

            // device_type_bits | manufacturer_id_bits | api_class_bits | api_index_bits | device_id_bits;
            int32_t packet_id = CAN_DEVICE_TYPE_MOTOR_CONTROLLER 
                                | CAN_MANUFACTURER_CTRE
                                | SPEED_CONTROL_MODE
                                | (target_speed & 0x0F) // "The API Index is a 4-bit identifier for a particular message within an API Class."
                                | SPARK_MAX_INTAKE;

            frc_can_frame_t frame = {
                .device_id = (packet_id) & 0x3f,
                .data = {0},
                .api_index = (packet_id >> 6) & 0x0F,
                .api_class = (packet_id >> 10) & 0x3F,
                .manufacturer = (packet_id >> 16) & 0xFF};

            frc_can_send(&frame);
        }
    }
}

/**
 * @brief Handle received messages from CAN bus (e.g., status messages, telemetry).
 */
static void spark_max_on_recv()
{
    // Process incoming CAN messages.
}

/**
 * @brief Make one motor "follow" the speed of another motor.
 *
 * @param follower The device ID of the follower motor.
 * @param following The device ID of the motor that will be followed.
 */
void spark_max_follow(uint8_t follower, uint8_t following)
{
    struct spark_max_desc *leader = NULL;
    struct spark_max_desc *follower_motor = NULL;

    for (int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++)
    {
        if (device_descriptors[spark].device_id == following)
        {
            leader = &device_descriptors[spark];
        }
        if (device_descriptors[spark].device_id == follower)
        {
            follower_motor = &device_descriptors[spark];
        }
    }

    if (leader && follower_motor)
    {
        // Set the follower motor's speed to match the leader motor's speed
        follower_motor->speed = leader->speed;
    }
}

/**
 * @brief Get the CAN device ID associated with a Spark MAX motor.
 *
 * @param spark The type of Spark MAX motor (e.g., SPARK_MAX_INTAKE).
 * @return The CAN device ID associated with the motor.
 */
frc_can_device_id_t spark_max_get_id(spark_max_t spark)
{
    switch (spark)
    {
    case SPARK_MAX_INTAKE:
        return CAN_DEVICE_ID_INTAKE_MOTOR;
    case SPARK_MAX_DUMP:
        return CAN_DEVICE_ID_DUMP_MOTOR;
    default:
        return CAN_DEVICE_ID_NONE; // Return an invalid ID if not found
    }
}

/**
 * @brief Get the CAN driver for the SPARK MAX motor controller.
 *
 * @return The pointer to the frc_can_driver struct.
 */
struct frc_can_driver *spark_max_get_driver()
{
    static struct frc_can_driver driver = {
        .device_type = CAN_DEVICE_TYPE_MOTOR_CONTROLLER,
        .manufacturer = CAN_MANUFACTURER_REV,
        .on_recv = spark_max_on_recv,
        .on_update = spark_max_on_update};

    return &driver;
}
