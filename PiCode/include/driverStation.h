#pragma once
#include <queue>
#include <asio.hpp>
#include <string>
#include <iostream>

// Defines length for each packet type
#define MESS_0x01_LEN 11

enum DsPacketType {
    DS_PACKET_HEARTBEAT = 0x01,
    DS_PACKET_GAMEPAD = 0x02,
};

struct DsPacket {
    uint8_t data[MESS_0x01_LEN];

    DsPacketType getType();
};

struct GamepadPacket { 
    DsPacket packet;

    GamepadPacket(DsPacket _packet);

    int8_t getLeftStickY();
    int8_t getRightStickY();
    int8_t getLeftTrigger();
    int8_t getRightTrigger();

    bool isButtonAPressed();
    bool isButtonBPressed();
    bool isDpadUp();
    bool isDpadDown();
    bool isLeftBumperPressed();
    bool issRightBumperPressed();
};

struct DsHeartbeatPacket { 
    DsPacket packet;

    DsHeartbeatPacket(DsPacket _packet);

    bool IsRobotEnabled();
};

class DsCommunicator
{
public:
    DsCommunicator();

    void sendHeartbeat(bool robotEnabled, bool rp2040Connected, int robotMode);
    void sendIntakePos(int pos);
    bool receiveMessage(DsPacket *packet);
    void update();
    void close();
    bool isConnected();
    
private:
    asio::io_service io;
    asio::ip::tcp::socket socket;
    asio::ip::tcp::acceptor acceptor;
    bool accepting = false;

    void startAccepting();
    void acceptSocketHandler(const asio::error_code& ec);
    void sendPacket(uint8_t *data, size_t data_len);
};
