#include "SPARK_MAX.h"
#include <stdbool.h>

/**
 * @brief The maximum device ID for a Spark MAX.
 */
#define SPARK_MAX_ID_MAX 31
#define SPARK_MAX_COUNT 2

struct spark_max_desc {
    frc_can_device_id_t device_id;
    int8_t speed;
};



struct spark_max_desc device_descriptors[SPARK_MAX_COUNT] = {{CAN_DEVICE_ID_INTAKE_MOTOR, 0}};

frc_can_device_id_t spark_max_get_id(spark_max_t spark) {
    switch(spark) {
        case SPARK_MAX_INTAKE:
            return CAN_DEVICE_ID_INTAKE_MOTOR;
        case SPARK_MAX_DUMP:
            return CAN_DEVICE_ID_DUMP_MOTOR;
        default:
            return CAN_DEVICE_ID_NONE;
    }
} 

void spark_max_init() {
    // Nothing have to happen ... sleep zzz
}

volatile int spark_lock = 0;


/**
 * @brief Set all motor's speed to zero and add a software lock to prevent other threads from using it
*/
void spark_max_disable() {

    for(int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++) {
        device_descriptors[spark].speed = 0;
    }

    while (__sync_lock_test_and_set(&spark_lock, 1));
}


void spark_max_enable() {
    __sync_lock_release(&spark_lock);
}

void spark_max_set_speed(spark_max_t id, int8_t speed)
{
    
    for(int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++) {
        struct spark_max_desc motor = device_descriptors[spark];
        if(motor.device_id == id){
            motor.speed = speed;
        }
    }
}

static void spark_max_on_update() {
    for (int8_t spark = 0; spark < SPARK_MAX_COUNT && spark_lock == 0; spark++) {
        int8_t target_speed = device_descriptors[spark].speed;

    }

}

static void spark_max_on_recieve() {

}

// follower follows following?
void spark_max_follow(uint8_t follower, uint8_t following)
{

    struct spark_max_desc* leader = NULL;
    struct spark_max_desc* follower_motor = NULL;

    // would be better if we have some hashmap to cache the following relationship
    for(int8_t spark = 0; spark < SPARK_MAX_COUNT; spark++) {
        if(device_descriptors[spark].device_id == following) {
            leader = &device_descriptors[spark];
        }
        if(device_descriptors[spark].device_id == follower) {
            follower_motor = &device_descriptors[spark];
        }
    }

    if (leader && follower_motor) {
        follower_motor->speed = leader->speed;
    }
}

struct frc_can_driver *spark_max_get_driver()
{
    static struct frc_can_driver driver = {
        .device_type = CAN_DEVICE_TYPE_MOTOR_CONTROLLER,
        .manufacturer = CAN_MANUFACTURER_REV,
        .on_recv = spark_max_on_recieve,
        .on_update = spark_max_on_update
    };

    return &driver;
}