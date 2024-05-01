import networking
import window
import gui
from enum import Enum
import time
import pygame
import struct

class LinkageState(Enum):
    RETRACTED = 0
    HEIGHT_CONTROL = 1,
    MANUAL = 2

    def __str__(self):
        if(self == self.RETRACTED):
            return "Retracted"
        elif(self == self.HEIGHT_CONTROL):
            return "Height Control"
        elif(self == self.MANUAL):
            return "Manual"



class DriverStationState: 
    def __init__(self):
        self.running = True
        self.robot_connected = False
        self.robot_enabled = False
        self.linkage_state = LinkageState.RETRACTED

    def set_robot_connected(self, connected):
        self.robot_connected = connected
    
    def is_robot_connected(self):
        return self.robot_connected

    def enable_robot(self):
        self.robot_enabled = True
        print("Robot enabled")

    def disable_robot(self):
        self.robot_enabled = False
        print("Robot disabled")

    def is_robot_enabled(self):
        return self.robot_enabled

    def reset_intake_encoder(self):
        print("Reset intake encoder")

    def run_auto_dig(self):
        print("Run auto dig")

    def restart_robot_code(self):
        print("Restart robot code")

    def reboot_pi(self):
        print("Reboot pi")
    
    def reboot_rp2040(self):
        print("Reboot RP2040")

    def get_linkage_state(self):
        return str(self.linkage_state)
    
    def deploy_intake(self):
        print("Deploy intake")
        self.linkage_state = LinkageState.HEIGHT_CONTROL

    def manual_control_intake(self):
        print("Set deploy to manual")
        self.linkage_state = LinkageState.MANUAL

    def shutdown(self):
        self.running = False

class DriverStation:
    def __init__(self):
        self.window = window.Window()
        self.gui = gui.Gui()
        self.state = DriverStationState()
        self.connection_manager = networking.ConnectionManager()
        self.joystick = pygame.joystick.Joystick(0)

    def get_button_bitset(self):
        """Generate the bitset for buttons according to the specified mapping."""
        x, y = self.joystick.get_hat(0)

        if y > 0.5:
            print("dpad up")

        bitset = 0
        bitset |= self.joystick.get_button(8) << 0  # Left Stick Press
        bitset |= self.joystick.get_button(9) << 1  # Right Stick Press
        bitset |= (y > 0.5) << 2  # D-pad up
        bitset |= (y < -0.5) << 3  # D-pad down
        bitset |= (x > 0.5) << 4  # D-pad right
        bitset |= (x < -0.5) << 5  # D-pad left
        bitset |= self.joystick.get_button(0) << 6  # A button
        bitset |= self.joystick.get_button(1) << 7  # B button
        return bitset

    def get_button_bitset2(self):
        """Generate the second bitset for buttons according to the specified mapping."""
        bitset = 0
        bitset |= self.joystick.get_button(2) << 0  # X button
        bitset |= self.joystick.get_button(3) << 1  # Y button
        bitset |= self.joystick.get_axis(2)>10 << 2  # Left trigger, scaled down
        bitset |= self.joystick.get_axis(5)>10 << 3  # Right Trigger, scaled down
        bitset |= self.joystick.get_button(4) << 4  # Left Bumper
        bitset |= self.joystick.get_button(5) << 5  # Right Bumper
        bitset |= self.joystick.get_button(7) << 6  # Start button
        bitset |= self.joystick.get_button(6) << 7  # Select button
        return bitset

    def send_packet(self):
        button_set_1 = self.get_button_bitset()
        button_set_2 = self.get_button_bitset2()

        packet = struct.pack(
                    '!bbbbbBBbbbb',
                    0x01,  # Message type for controller status
                    int(-self.joystick.get_axis(1)*100),
                    int(-self.joystick.get_axis(0)*100),
                    int(-self.joystick.get_axis(4)*100),
                    int(-self.joystick.get_axis(3)*100),
                    button_set_1,
                    button_set_2,
                    int((self.joystick.get_axis(5)+1)*50),
                    int((self.joystick.get_axis(2)+1)*50), 0, 0,  # Future usage placeholders
                )
        if self.state.is_robot_connected():        
            print("Packet:", packet.hex(' '))
            self.connection_manager.send_packet(packet)

        self.last_packet = time.time()

    def run(self):
        self.last_packet = time.time()
        while self.state.running:             
            self.state.set_robot_connected(self.connection_manager.is_connected())
            self.window.render(self.state, self.gui)
            self.window.process_events(self.state)

            if(time.time() - self.last_packet > 0.01):
                self.send_packet()

        self.connection_manager.shutdown()

ds = DriverStation()
print("Driver station created. Running application!")
ds.run()
