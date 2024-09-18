#include "FRC_CAN.h"
#include <stdbool.h>

static bool initialized = false;
static struct frc_can_driver **drivers;
static size_t driver_count;

void frc_can_init(struct frc_can_driver **_drivers, size_t _driver_count) {
    if(initialized){
        return;
    }

    drivers = _drivers;
    driver_count = _driver_count;
    initialized = true;

    // TODO: Setup On Recieve
}

hal_ret_code_t frc_can_start() { 
    return HAL_ERR;
}

void frc_can_update() {
    if(!initialized) {
        return;
    }

    for(size_t i = 0; i < driver_count; i++) {
        drivers[i]->on_update();
    }
}

void frc_can_send(frc_can_frame_t *frame) {
    if(!initialized) {
        return;
    }

    if(frame->device_id == CAN_DEVICE_ID_NONE) {
        return;
    }
}
