#include "FRC_CAN.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <Adafruit_MCP2515.h>

// Flag to track if CAN is initialized
static bool initialized = false;

// List of CAN device drivers
static frc_can_driver_t **drivers;
static size_t driver_count;

Adafruit_MCP2515 mcp(PIN_CAN_CS);

#define CAN_BAUDRATE (1000000)

/**
 * @brief Initialize the CAN drivers.
 *
 * @param _drivers The array of CAN device drivers.
 * @param _driver_count The number of drivers in the array.
 */
void frc_can_init(frc_can_driver_t **_drivers, size_t _driver_count)
{
    if (initialized)
    {
        return;
    }

    drivers = _drivers;
    driver_count = _driver_count;
    initialized = true;
}

/**
 * @brief Start CAN bus communication.
 *
 * @return HAL error code (HAL_ERR if unable to start).
 */
hal_ret_code_t frc_can_start()
{
    if (!initialized)
    {
        return HAL_ERR; // Can't start if not initialized
    }

    // Initialization of CAN Bus hardware
    // This step might include setting the bitrate, enabling CAN controllers, etc.
    mcp.begin();
    mcp.setBitrate(CAN_BAUDRATE); // Set bitrate
    return HAL_OK;
}

/**
 * @brief Periodic function to update all CAN drivers.
 *
 * This function should be called periodically to update all devices.
 */
void frc_can_update()
{
    if (!initialized)
    {
        return;
    }

    for (size_t i = 0; i < driver_count; i++)
    {
        // Call the `on_update` function for each registered driver
        if (drivers[i] != NULL && drivers[i]->on_update != NULL)
        {
            drivers[i]->on_update();
        }
    }

    // parse all the remaining packets
    int packetSize = mcp.parsePacket(); 
    while(packetSize) {
        if(mcp.packetExtended()) {
            frc_can_recv_packet();
        }
        packetSize = mcp.parsePacket();
    }
}

/**
 * @brief Send a CAN frame.
 *
 * @param frame The CAN frame to send.
 */
void frc_can_send(frc_can_frame_t *frame)
{
    if (!initialized || frame == NULL)
    {
        return;
    }

    if (frame->device_id == CAN_DEVICE_ID_NONE)
    {
        return; // Invalid device ID
    }

    // Build the CAN Bus ID (see WPILib docs for CAN addressing)
    uint32_t api_index_bits = (frame->api_index & 0x0F) << 6;           // Bits [6-9] are the API index
    uint32_t api_class_bits = (frame->api_class & 0x3F) << 10;          // Bits [10-15] are the API class
    uint32_t manufacturer_id_bits = (frame->manufacturer & 0xFF) << 16; // Manufacturer ID
    uint32_t device_type_bits = (frame->device_type & 0x1F) << 24;      // Device Type (lower 5 bits)
    uint32_t device_id_bits = frame->device_id & 0x3f;                  // Device ID Bits [0-5] are the Device Number

    uint32_t packet_id = device_type_bits | api_index_bits | api_class_bits | manufacturer_id_bits;
    // uint32_t packet_id = device_type_bits | manufacturer_id_bits | api_class_bits | api_index_bits | device_id_bits;

    // Send the packet over the CAN bus
    mcp.beginExtendedPacket(packet_id); // Start packet with the extended identifier
    for (int i = 0; i < CAN_FRAME_DATA_LENGTH; i++)
    {
        mcp.write(frame->data[i]); // Write data byte by byte
    }
    mcp.endPacket(); // End the packet transmission
}

/**
 * @brief Handle the reception of CAN frames.
 *
 * This function is called when CAN frames are received.
 * It processes the incoming frames and routes them to the appropriate driver.
 */
void frc_can_recv_packet()
{
    if (!initialized)
    {
        return;
    }

    // ReadpacketSize the incoming packet from MCP2515
    uint32_t packet_id = mcp.packetId(); // Read the extended CAN ID
    uint8_t data[CAN_FRAME_DATA_LENGTH];

    for (int i = 0; i < CAN_FRAME_DATA_LENGTH; i++)
    {
        data[i] = mcp.read(); // Read the data bytes
    }

    // Find the driver associated with this packet ID
    for (size_t i = 0; i < driver_count; i++)
    {
        uint32_t packet_device_type = (packet_id >> 24) & 0x1F;

        if (drivers[i]->device_type == packet_device_type)
        {
            // Call the on_recv callback for the appropriate driver
            frc_can_frame_t frame = {
                .device_id = (packet_id) & 0x3f,
                .data = {0},
                .api_index = (packet_id >> 6) & 0x0F,
                .api_class = (packet_id >> 10) & 0x3F,
                .manufacturer = (packet_id >> 16) & 0xFF};

            memcpy(frame.data, data, CAN_FRAME_DATA_LENGTH);

            if (drivers[i]->on_recv != NULL)
            {
                drivers[i]->on_recv(&frame); // Invoke the driver's receive callback
            }
            break;
        }
    }
}
