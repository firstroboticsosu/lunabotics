#pragma once
#include <mutex>
#include <queue>
#include "asio.hpp"
#include <string>
#include <iostream>
#include "structs.h"

// Defines length for each packet type
#define MESS_0x01_LEN 11


class Communicator
{
public:
    Communicator();
    void readIncomingPacket();
    void send(asio::ip::tcp::socket & socket, const std::string& message);
    void sendHeartbeat();
    RobotState getRobotState();
    

private:
    asio::io_service io;
    asio::ip::tcp::socket socket;
    asio::ip::tcp::acceptor acceptor;
    asio::streambuf incomingBuffer;
    uint8_t data[MESS_0x01_LEN];
    std::mutex mutex;
    RobotState rbState;

    void proccesMessage();
    
    void processControllerState(uint8_t* data);

};
