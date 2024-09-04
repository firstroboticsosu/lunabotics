# Lunabotics Software
Software for the [Ohio State Lunabotics Team](https://org.osu.edu/firstrobotics/lunabotics/).

## Architecture
The software is split into three primary components: 
- **DriverStation** (Python): Allows operators to remotely control the robot. This software is run on a laptop that connects to the robot's Raspberry Pi via Wifi. 
- **Robot Code** (C++): High level robot code which handles networking, autonomy, computer vision. Communicates with the Driverstation via a TCP connection and communicates with the RP2040 HAL board via a USB serial port.
- **HAL** (C): the Hardware Abstraction Layer which interacts with hardware such as motors and sensors. Runs on the RP2040 feather board (HAL board).

![Network Architecture](Documentation/network.png)

## File Structure
- Documentation: contains relevant documentation files
- DriverStation: contains the code to run the driver station
- HAL: contains the code to run on the RP2040 Feather board.
- PiResources: contains files that are installed onto the raspberry pi seperate from the software
- RobotCode: contains the code which runs on the raspberry pi. 
- TestScripts: Utility scripts which can be used for testing the robot's software. 