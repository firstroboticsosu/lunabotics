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

class GamepadState: 
    def __init__(self):
        self.set_connected(False)

    def set_connected(self, connected):
        self.connected = connected
        self.left_stick_x = 0.0
        self.left_stick_y = 0.0
        self.right_stick_x = 0.0
        self.right_stick_y = 0.0
        self.dpad_up = False
        self.dpad_right = False
        self.dpad_left = False
        self.dpad_down = False
        self.button_a = False
        self.button_b = False
        self.button_x = False
        self.button_y = False
        self.left_bumper = False
        self.right_bumper = False
        self.left_trigger = 0.0
        self.right_trigger = 0.0

    def is_connected(self):
        return self.connected

    def set_joysticks(self, left_x, left_y, right_x, right_y):
        self.left_stick_x = left_x
        self.left_stick_y = left_y
        self.right_stick_x = right_x
        self.right_stick_y = right_y

    def get_left_stick(self):
        return [self.left_stick_x, self.left_stick_y]
    
    def get_right_stick(self):
        return [self.right_stick_x, self.right_stick_y]
    
    def set_buttons(self, a, b, x, y):
        self.button_a = a
        self.button_b = b
        self.button_x = x
        self.button_y = y

    def get_button_x(self):
        return self.button_x
    
    def get_button_y(self):
        return self.button_y

    def get_button_a(self):
        return self.button_a

    def get_button_b(self):
        return self.button_b

    def set_bumpers(self, left, right):
        self.left_bumper = left
        self.right_bumper = right

    def get_left_bumper(self):
        return self.left_bumper
    
    def get_right_bumper(self):
        return self.right_bumper
    
    def set_triggers(self, left, right):
        self.left_trigger = left
        self.right_trigger = right
 
    def get_left_trigger(self):
        return self.left_trigger
    
    def get_right_trigger(self):
        return self.right_trigger
    
    def set_dpad(self, up, down, left, right):
        self.dpad_up = up
        self.dpad_down = down
        self.dpad_left = left
        self.dpad_right = right

    def get_dpad_up(self):
        return self.dpad_up
    
    def get_dpad_down(self):
        return self.dpad_down
    
    def get_dpad_left(self):
        return self.dpad_left
    
    def get_dpad_right(self):
        return self.dpad_right

class DriverStationState: 
    def __init__(self):
        self.running = True
        self.robot_connected = False
        self.robot_enabled = False
        self.linkage_state = LinkageState.RETRACTED
        self.gamepad = GamepadState()

    def set_robot_connected(self, connected):
        self.robot_connected = connected
    
    def is_robot_connected(self):
        return self.robot_connected
    
    def get_gamepad(self):
        return self.gamepad

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

class DriverStationInput:
    def __init__(self, ds_state, connection_manager):
        self.joystick = None
        self.update(ds_state, connection_manager)

    def connect_joystick(self):
        if pygame.joystick.get_count() > 0:
            self.joystick = pygame.joystick.Joystick(0)

    def update(self, ds_state, connection_manager):
        if self.joystick is None:
            self.connect_joystick()
        elif pygame.joystick.get_count() is 0:
            self.joystick.quit()
            self.joystick = None
            
        gamepad_state = ds_state.get_gamepad()
        if self.joystick is not None:
            gamepad_state.set_connected(True)
            gamepad_state.set_joysticks(
                self.joystick.get_axis(0),
                -self.joystick.get_axis(1),
                self.joystick.get_axis(3),
                -self.joystick.get_axis(4)
            )
            gamepad_state.set_triggers(
                self.joystick.get_axis(2) / 2.0 + 0.5,
                self.joystick.get_axis(5) / 2.0 + 0.5
            )
            dpad_x, dpad_y = self.joystick.get_hat(0)
            gamepad_state.set_dpad(
                dpad_y > 0,
                dpad_y < 0,
                dpad_x > 0,
                dpad_x < 0
            )
            gamepad_state.set_buttons(
                self.joystick.get_button(0),
                self.joystick.get_button(1),
                self.joystick.get_button(2),
                self.joystick.get_button(3),
            )
            gamepad_state.set_bumpers(
                self.joystick.get_button(4),
                self.joystick.get_button(5)
            )
        else:
            gamepad_state.set_connected(False)


class DriverStation:
    def __init__(self):
        self.window = window.Window()
        self.gui = gui.Gui()
        self.state = DriverStationState()
        self.connection_manager = networking.ConnectionManager()
        self.input = DriverStationInput(self.state, self.connection_manager)

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
            self.input.update(self.state, self.connection_manager)
            self.window.render(self.state, self.gui)
            self.window.process_events(self.state)

            # if(time.time() - self.last_packet > 0.01):
            #     self.send_packet()

        self.connection_manager.shutdown()

ds = DriverStation()
print("Driver station created. Running application!")
ds.run()
