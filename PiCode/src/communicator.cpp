#include "communicator.h"


Communicator::Communicator(): io(), socket(io),
    acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 2000))
{
    acceptor.accept(socket);

}

void Communicator::readIncomingPacket()
{
    if(socket.available() > MESS_0x01_LEN) {
        asio::read(socket, asio::buffer(data, MESS_0x01_LEN));
        proccesMessage();
    }    
}

void Communicator::send(asio::ip::tcp::socket &socket, const std::string &message)
{
    const std::string msg = message + "\n";
    asio::write( socket, asio::buffer(message) );
}

RobotState Communicator::getRobotState()
{
    return rbState;
}

void Communicator::proccesMessage()
{

    if(data[0] == 0x01) {
        std::cout << "Recived Heartbeat from DS!" << std::endl;
        rbState.robot_enabled = data[1] != 0;
        std::cout << "Robot Enabled: " << (rbState.robot_enabled ? "Yes" : "No") << std::endl;
    } else if(data[0] == 0x02)
    {
        std::cout << "Recived controller state from DS!" << std::endl;
        processControllerState(data);
        incomingBuffer.consume(MESS_0x01_LEN);
    } else {
        std::cout << "Received unknown message type from DS!" << data[0] << std::endl;
    }
}

void Communicator::sendHeartbeat() {
    uint8_t packet[11] = {0};
    packet[0] = 0x01;
    packet[1] = rbState.robot_enabled;
    
    asio::write(socket, asio::buffer(packet, 11));
}

void Communicator::processControllerState(uint8_t* data)
{
    rbState.flMotor = data[1];
    rbState.blMotor = data[1];

    rbState.frMotor = data[3];
    rbState.brMotor = data[3];

    uint8_t digDriveSpeed = 30;
    uint8_t intakeSpeed = 50;

    if(data[5]&0x04) {
        rbState.flMotor = digDriveSpeed;
        rbState.blMotor = digDriveSpeed;
        rbState.frMotor = digDriveSpeed;
        rbState.brMotor = digDriveSpeed;
    } else if (data[5]&0x08) {
        rbState.flMotor = -digDriveSpeed;
        rbState.blMotor = -digDriveSpeed;
        rbState.frMotor = -digDriveSpeed;
        rbState.brMotor = -digDriveSpeed;
    }

    int8_t intakeLocUp = ((data[6]&0b00010000)>0)*15;
    int8_t intakeLocDown = ((data[6]&0b00100000)>0)*65;

    rbState.intakeLocation = intakeLocUp - intakeLocDown;

    int8_t intake = ((data[5]&0x40)>0)*100;

    int8_t dump = ((data[5]&0x80)>0)*100;

    // if(data[7] > 20) {
    //     dump = 
    // }

    // if(data[8] > 20) {
    //     intake = 
    // }

    rbState.dumpMotor = dump;
    rbState.intakeMotor = intake;
}
