#include "driverStation.h"
#include <asio.hpp>

DsCommunicator::DsCommunicator() : io(), socket(io), acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 2000)) {
    std::cout << "Connecting to DS..." << std::endl;
    startAccepting();
}

void DsCommunicator::acceptSocketHandler(const asio::error_code &ec) {
    if (ec.value() != 0) {
        std::cout << "Failed to accept DS: " << ec.message() << std::endl;
    } else {
        std::cout << "Connected to DS!" << std::endl;
    }
    accepting = false;
}

void DsCommunicator::startAccepting() {
    if (accepting) {
        return;
    }

    acceptor.async_accept(socket, [this](const asio::error_code &ec) { this->acceptSocketHandler(ec); });
    accepting = true;
}

void DsCommunicator::update() {
    if (io.stopped()) {
        io.reset();
    }
    io.poll();

    if (!socket.is_open() && !accepting) {
        std::cout << "Reconnecting to DS..." << std::endl;
        startAccepting();
    }
}

bool DsCommunicator::receiveMessage(DsPacket *packet) {
    if (!socket.is_open()) {
        return false;
    }

    try {
        if (socket.available() > MESS_0x01_LEN) {
            asio::read(socket, asio::buffer(packet->data, MESS_0x01_LEN));
            return true;
        }
    } catch (std::exception &e) {
        std::cout << "Failed to read packet from DS: " << e.what() << std::endl;
        close();
    }

    return false;
}

void DsCommunicator::sendPacket(uint8_t *data, size_t data_len) {
    if (!socket.is_open()) {
        return;
    }

    try {
        asio::write(socket, asio::buffer(data, data_len));
    } catch (std::exception &e) {
        std::cout << "Failed to send packet to DS: " << e.what() << std::endl;
        close();
    }
}

void DsCommunicator::sendHeartbeat(bool robotEnabled, bool rp2040Connected) {
    uint8_t packet[11] = {0};
    packet[0] = 0x01;
    packet[1] = robotEnabled;
    packet[2] = rp2040Connected;

    sendPacket(packet, 11);
}

void DsCommunicator::sendIntakePos(int pos) {
    uint8_t packet[11] = {0};
    packet[0] = 0x03;
    packet[1] = pos & 0xFF;
    packet[2] = (pos >> 8) & 0xFF;
    packet[3] = (pos >> 16) & 0xFF;
    packet[4] = (pos >> 24) & 0xFF;

    sendPacket(packet, 11);
}

void DsCommunicator::close() {
    std::cout << "Closing connection to DS" << std::endl;
    socket.close();
}

bool DsCommunicator::isConnected() {
    return socket.is_open();
}
 
DsPacketType DsPacket::getType() { return (DsPacketType)data[0]; }

GamepadPacket::GamepadPacket(DsPacket _packet) : packet(_packet) {}

int8_t GamepadPacket::getLeftStickY() { return packet.data[1]; }

int8_t GamepadPacket::getRightStickY() { return packet.data[3]; }

int8_t GamepadPacket::getLeftTrigger() { return packet.data[7]; }

int8_t GamepadPacket::getRightTrigger() { return packet.data[8]; }

bool GamepadPacket::isButtonAPressed() { return packet.data[5] & 0x40; }

bool GamepadPacket::isButtonBPressed() { return packet.data[5] & 0x80; }

bool GamepadPacket::isDpadUp() { return packet.data[5] & 0x04; }

bool GamepadPacket::isDpadDown() { return packet.data[5] & 0x08; }

bool GamepadPacket::isLeftBumperPressed() { return packet.data[6] & 0x10; }

bool GamepadPacket::issRightBumperPressed() { return packet.data[6] & 0x20; }

DsHeartbeatPacket::DsHeartbeatPacket(DsPacket _packet) : packet(_packet) {}

bool DsHeartbeatPacket::IsRobotEnabled() { return packet.data[1] != 0; }

// void Communicator::readIncomingPacket()
// {
//     if(socket.available() > MESS_0x01_LEN) {
//         asio::read(socket, asio::buffer(data, MESS_0x01_LEN));
//     }
// }

// void Communicator::send(asio::ip::tcp::socket &socket, const std::string &message)
// {
//     const std::string msg = message + "\n";
//     asio::write( socket, asio::buffer(message) );
// }

// void Communicator::proccesMessage()
// {

//     if(data[0] == 0x01) {
//         std::cout << "Recived Heartbeat from DS!" << std::endl;
//         rbState.robot_enabled = data[1] != 0;
//         std::cout << "Robot Enabled: " << (rbState.robot_enabled ? "Yes" : "No") << std::endl;
//     } else if(data[0] == 0x02)
//     {
//         std::cout << "Recived controller state from DS!" << std::endl;
//         processControllerState(data);
//         incomingBuffer.consume(MESS_0x01_LEN);
//     } else {
//         std::cout << "Received unknown message type from DS!" << data[0] << std::endl;
//     }
// }

// void Communicator::processControllerState(uint8_t* data)
// {
//     rbState.flMotor = data[1];
//     rbState.blMotor = data[1];

//     rbState.frMotor = data[3];
//     rbState.brMotor = data[3];

//     uint8_t digDriveSpeed = 30;
//     uint8_t intakeSpeed = 50;

//     if(data[5]&0x04) {
//         rbState.flMotor = digDriveSpeed;
//         rbState.blMotor = digDriveSpeed;
//         rbState.frMotor = digDriveSpeed;
//         rbState.brMotor = digDriveSpeed;
//     } else if (data[5]&0x08) {
//         rbState.flMotor = -digDriveSpeed;
//         rbState.blMotor = -digDriveSpeed;
//         rbState.frMotor = -digDriveSpeed;
//         rbState.brMotor = -digDriveSpeed;
//     }

//     int8_t intakeLocUp = ((data[6]&0b00010000)>0)*15;
//     int8_t intakeLocDown = ((data[6]&0b00100000)>0)*65;

//     rbState.intakeLocation = intakeLocUp - intakeLocDown;

//     int8_t intake = ((data[5]&0x40)>0)*100;

//     int8_t dump = ((data[5]&0x80)>0)*100;

//     // if(data[7] > 20) {
//     //     dump =
//     // }

//     // if(data[8] > 20) {
//     //     intake =
//     // }

//     rbState.dumpMotor = dump;
//     rbState.intakeMotor = intake;
// }
