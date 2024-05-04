
#include <iostream>
#include <string>

#include "robotActuation.h"
#include "robotControl.h"
#include "robotState.h"
#include "driverStation.h"
#include "util.h"

#define DS_HEARTBEAT_RATE_MS 1000

int main() 
{
  std::cout << "Awaiting connection from DS" << std::endl;
  DsCommunicator dsComms;
  RobotActuation rp2040("/dev/ttyACM0", 115200);
  RobotControl control;
  std::cout << "Robot code initialized!" << std::endl;

  uint64_t lastSentDsHearbeat = 0;

  while (true) {
    uint64_t currentTime = getUnixTimeMs();

    DsPacket dsPacket;
    while(dsComms.receiveMessage(&dsPacket)) {
      DsPacketType type = dsPacket.getType();

      if(type == DS_PACKET_HEARTBEAT) {
        DsHeartbeatPacket heartbeatPacket(dsPacket);
        control.handleDsHeartbeatPacket(heartbeatPacket);
      } else if(type == DS_PACKET_GAMEPAD) {
        GamepadPacket gamepadPacket(dsPacket);
        control.handleGamepadPacket(gamepadPacket);
      }
    }

    if(currentTime - lastSentDsHearbeat > DS_HEARTBEAT_RATE_MS) {
      dsComms.sendHeartbeat(control.getRobotState().isRobotEnabled(), false);
      lastSentDsHearbeat = currentTime;
    }

    control.sendStateToRP2040(&rp2040);
  }

  // rbActuator->sendDriveMotors(0, 0, 0, 0);
  // rbActuator->sendCurrentQueue();
  // rbActuator->run();

  // RobotState currentState;
  // RobotState lastState;

  // int lastTime = get_unix_time_ms();
  // int lastDrive = get_unix_time_ms();
  // int lastDump = get_unix_time_ms();
  // int lastIntake = get_unix_time_ms();
  // int lastDeploy = get_unix_time_ms();
  // int lastDsHeartbeat = get_unix_time_ms();

  // while(true){
  //   int currentTime = get_unix_time_ms();
  //   c->readIncomingPacket();
  //   // RobotState rbstate = c->getRobotState();

  //   if (lastState.flMotor != currentState.flMotor || currentState.frMotor != currentState.frMotor || currentTime - lastDrive > 1000)
  //   {
  //     rbActuator->sendDriveMotors(currentState.flMotor, currentState.frMotor, currentState.blMotor, currentState.brMotor);
  //     std::cout << "Send Drive" << std::endl;
  //     lastDrive = currentTime;
  //   }

  //   if (lastState.dumpMotor != currentState.dumpMotor || currentTime - lastDump > 1000)
  //   {
  //     rbActuator->sendDumpMotor(currentState.dumpMotor);
  //     lastDump = currentTime;
  //     std::cout << "Send Dump" << std::endl;
  //   }

  //   if (lastState.intakeMotor != currentState.intakeMotor || currentTime - lastIntake > 1000)
  //   {
  //     rbActuator->sendIntakeMotor(currentState.intakeMotor);
  //     lastIntake = currentTime;
  //     std::cout << "Send Intake" << std::endl;
  //   }

  //   if(lastState.intakeLocation != currentState.intakeLocation || currentTime - lastDeploy > 1000)
  //   {
  //     rbActuator->sendIntakePosition(currentState.intakeLocation);
  //     lastDeploy = currentTime;
  //     std::cout << "Send Deploy" << std::endl;
  //   }

  //   if (currentTime > lastTime + 1000)
  //   {
  //     rbActuator->sendHeartbeat();
  //     std::cout << "Send Heart beat" << std::endl;
  //     lastTime = currentTime;
  //   }

  //   if(currentTime - lastDsHeartbeat > 500) {
  //     c->sendHeartbeat();
  //     lastDsHeartbeat = currentTime;
  //   }

  //   rbActuator->sendCurrentQueue();
  //   rbActuator->run();

  //   lastState = currentState;

  // }

  return 0;
}


