#include "robotActuation.h"

#include <iostream>
#include <string>

#include "communicator.h"

int main() 
{
  std::cout << "Awaiting connection from DS" << std::endl;
  Communicator * c = new Communicator();
  RobotActuation * rbActuator = new RobotActuation("/dev/ttyACM0", 115200);
  std::cout << "connection made" << std::endl;

  rbActuator->sendDriveMotors(0, 0, 0, 0);
  rbActuator->sendCurrentQueue();
  rbActuator->run();

  RobotState lastState;

  int lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  while(true){

    std::cout << std::endl;
    c->readIncomingPacket();
    RobotState rbstate = c->getRobotState();

    if (lastState.flMotor != rbstate.flMotor || lastState.frMotor != rbstate.frMotor)
    {
      rbActuator->sendDriveMotors(rbstate.flMotor, rbstate.frMotor, rbstate.blMotor, rbstate.brMotor);
      lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    if (lastState.dumpMotor != rbstate.dumpMotor)
    {
      rbActuator->sendDumpMotor(rbstate.dumpMotor);
      lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    if (lastState.intakeMotor != rbstate.intakeMotor)
    {
      rbActuator->sendIntakeMotor(rbstate.intakeMotor);
      lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    if(lastState.intakeLocation != rbstate.intakeLocation)
    {
      rbActuator->sendIntakePosition(rbstate.intakeLocation);
      lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int thisTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (thisTime > lastTime + 1000)
    {
      rbActuator->sendHeartbeat();
      lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    rbActuator->sendCurrentQueue();
    rbActuator->run();

    lastState = rbstate;

  }

  return 0;
}


