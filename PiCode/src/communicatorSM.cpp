/**
 * @file communicatorSM.cpp
 * @author Lance Borden
 * @brief 
 * @version 0.1
 * @date 2024-01-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/communicatorSM.h"
#include <iostream>
//#include <zmq.hpp>

/**
 * @brief Main state machine loop for networking communications
 * 
 */
void CommunicatorSM::runStateMachine() { }
//     // create network vars
//     zmq::context_t ctx;
//     zmq::socket_t sock(ctx, zmq::socket_type::rep);
//     switch (state)
//     {
//     case 0:
//         // initialize
//         state0(&ctx, &sock);
//         break;
//
//     case 1:
//         state1(&sock);
//         break;
//
//     case 2:
//         state2(&sock);
//         break;

//     }
// }

/**
 * @brief Initialize State
 * 
 */
//void CommunicatorSM::state0(zmq::context_t* ctx, zmq::socket_t* sock) {
//     // bind the socket to local ip
//     sock->connect("tcp://192.168.88.64:5555");
//     state = 1;
// }
//
// /**
//  * @brief Read State
//  *
//  */
// void CommunicatorSM::state1(zmq::socket_t* sock) {
//     state = 2;
// }
//
// /**
//  * @brief Write State
//  *
//  */
// void CommunicatorSM::state2(zmq::socket_t* sock) {
//     //compose the struct
//     robotMessage.msgCount = 1;
//     int data[128];
//     sock->send(zmq::buffer(data), zmq::send_flags::dontwait);
//     state = 1;
// }
