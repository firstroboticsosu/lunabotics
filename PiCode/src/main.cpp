#include "robotActuation.h"

#include <iostream>
#include <string>

#include "communicator.h"

int get_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

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

  int lastTime = get_time_ms();
  int lastDrive = get_time_ms();
  int lastDump = get_time_ms();
  int lastIntake = get_time_ms();
  int lastDeploy = get_time_ms();
  int lastDsHeartbeat = get_time_ms();

  while(true){
    int currentTime = get_time_ms();
    c->readIncomingPacket();
    RobotState rbstate = c->getRobotState();

    if (lastState.flMotor != rbstate.flMotor || lastState.frMotor != rbstate.frMotor || currentTime - lastDrive > 1000)
    {
      rbActuator->sendDriveMotors(rbstate.flMotor, rbstate.frMotor, rbstate.blMotor, rbstate.brMotor);
      std::cout << "Send Drive" << std::endl;
      lastDrive = currentTime;
    }

    if (lastState.dumpMotor != rbstate.dumpMotor || currentTime - lastDump > 1000)
    {
      rbActuator->sendDumpMotor(rbstate.dumpMotor);
      lastDump = currentTime;
      std::cout << "Send Dump" << std::endl;
    }

    if (lastState.intakeMotor != rbstate.intakeMotor || currentTime - lastIntake > 1000)
    {
      rbActuator->sendIntakeMotor(rbstate.intakeMotor);
      lastIntake = currentTime;
      std::cout << "Send Intake" << std::endl;
    }

    if(lastState.intakeLocation != rbstate.intakeLocation || currentTime - lastDeploy > 1000)
    {
      rbActuator->sendIntakePosition(rbstate.intakeLocation);
      lastDeploy = currentTime;
      std::cout << "Send Deploy" << std::endl;
    }

    if (currentTime > lastTime + 1000)
    {
      rbActuator->sendHeartbeat();
      std::cout << "Send Heart beat" << std::endl;
      lastTime = currentTime;
    }

    if(currentTime - lastDsHeartbeat > 500) {
      c->sendHeartbeat();
      lastDsHeartbeat = currentTime;
    }

    rbActuator->sendCurrentQueue();
    rbActuator->run();

    lastState = rbstate;

  }

  return 0;
}


