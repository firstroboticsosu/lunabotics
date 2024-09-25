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

struct spark_max_desc device_descriptors[SPARK_MAX_COUNT] = {0};

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
    for(spark_max_t spark = 0; spark < SPARK_MAX_COUNT; spark++) {
        device_descriptors[spark] = (struct spark_max_desc) {
            .device_id = spark,
            .speed = 0,
        };
    }
}

void spark_max_disable() {

}

void spark_max_enable() {

}

static void spark_max_on_update() {
    // TODO
}

static void spark_max_on_recieve() {
    // TODO
}

struct frc_can_driver *spark_max_get_driver() {
    static struct frc_can_driver driver = {
        .device_type = CAN_DEVICE_TYPE_MOTOR_CONTROLLER,
        .manufacturer = CAN_MANUFACTURER_REV,
        .on_recv = spark_max_on_recieve,
        .on_update = spark_max_on_update
    };

    return &driver;
}