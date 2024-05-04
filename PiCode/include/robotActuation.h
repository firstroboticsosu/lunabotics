#pragma once

#include "asio.hpp"
#include <queue>
#include <cstdint>

#define SERIAL_MES_LEN 13

 // This class will use the Adafruit feather to acutaly control the robot based on Robot state. This is TBD

union SerialPacket
{
    uint8_t packet[SERIAL_MES_LEN];
    struct {
        private: const uint16_t syncBytes = 0xBEEF;
        public:
        uint8_t messageType = 0x00;
        uint8_t data[8] = {0};
        uint8_t checksumHigh;
        uint8_t checksumLow;
    } portions;
};

class RobotActuation 
{

public:
    RobotActuation(std::string port, unsigned int baud_rate);
    void readNextMessage();
    void sendDriveMotors(int8_t frontLeftMotor, int8_t frontRightMotor, int8_t backLeftMotor, int8_t backRightMotor);
    void sendIntakeMotor(int8_t intakeMotorSpeed);
    void sendDumpMotor(int8_t dumpMotorSpeed);
    void sendDeployControl(int8_t angle);
    void sendHeartbeat();


    int sendCurrentQueue();
    void run();

private:
    asio::io_service io;
    asio::serial_port serial;

    std::queue<SerialPacket> outgoingQueue;
    uint8_t outgoingBytes[13];
    int positonOfNextOutgoingByte;
    bool byteQueueFull= false;
    bool serialTransmit = false;

    void sendBytesHandler(const asio::error_code& error, std::size_t bytes_transferred);
    
    void reciveMessageHandler();

    void enqueueMessage(SerialPacket * mess);

    void addChecksum(SerialPacket * packet);

    uint16_t fletcher16(const uint8_t *data, size_t len);

};
