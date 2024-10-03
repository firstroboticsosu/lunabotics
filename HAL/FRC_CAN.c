#include "FRC_CAN.h"
#include <stdbool.h>

static bool initialized = false;
static frc_can_driver_t **drivers;
static size_t driver_count;

void frc_can_init(frc_can_driver_t **_drivers, size_t _driver_count) {
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

    // Built the CAN Bus ID. See: https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
    uint32_t api_index_bits = (frame->api_index & 0x0F) << 6; // Bits [6-9] are the API index
    uint32_t api_class_bits = (frame->api_class & 0x3F) << 10; // Bits [10-15] are the API class
    uint32_t manufacturer_id_bits = (frame->manufacturer & 0xFF) << 16;
    uint32_t device_type_bits  = (frame->device_id & 0x1F) << 24;
    uint32_t packet_id = device_id_bits | api_index_bits | api_class_bits | manufacturer_id_bits | device_type_bits;
    
    // Write packet
    mcp.beginExtendedPacket(packet_id);
    for(int i = 0; i < CAN_FRAME_DATA_LENGTH; i++) {
        mcp.write(frame->data[i]);
    }
    mcp.endPacket();
}
