#include "communicator.h"


Communicator::Communicator(): io(), socket(io),
    acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 2000))
{
    acceptor.accept(socket);

}

void Communicator::readIncomingPacket()
{
    asio::read(socket, asio::buffer(data, MESS_0x01_LEN));
    //asio::read_until( socket, incomingBuffer, "/n" );
    proccesMessage();
    
}

void Communicator::send(asio::ip::tcp::socket &socket, const std::string &message)
{
    const std::string msg = message + "\n";
    asio::write( socket, asio::buffer(message) );
}

RobotState Communicator::getRobotState()
{
    //const std::lock_guard<std::mutex> lock(mutex);
    return rbState;
}

void Communicator::proccesMessage()
{
    std::cout << "Recived from DS!" << std::endl;
    // for (int i = 0; i < MESS_0x01_LEN; i++)
    // {
    //     std::cout <<std::hex<< unsigned(data[i])<<" ";
    // }
    // std::cout << std::endl;

    if(data[0] == 0x01)
    {
        processControllerState(data);
        incomingBuffer.consume(MESS_0x01_LEN);
    }

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
