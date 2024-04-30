#include <sys/types.h>
// #include <zmq.hpp>
#pragma once

struct robotMessage_t {
    u_int32_t msgCount;
    u_int8_t leftFrontDrive;
    u_int8_t leftBackDrive;
    u_int8_t rightFrontDrive;
    u_int8_t rightBackDrive;
    u_int8_t intakeMotor;
    u_int8_t dumpMotor;
    u_int8_t lIntakeIndexer;
    u_int8_t rIntakeIndexer;
};

class CommunicatorSM
{
public:
    void runStateMachine();

private:
    robotMessage_t robotMessage;
//     int state = 0;
//
//     // Initilaize
//     void state0(zmq::context_t* ctx, zmq::socket_t* sock);
//
//     // Read
//     void state1(zmq::socket_t* sock);
//
//     // Write
//     void state2(zmq::socket_t* sock);
//
};
