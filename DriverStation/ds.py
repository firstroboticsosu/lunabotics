
import window
import gui
from enum import Enum

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
        self.robot_enabled = False
        self.linkage_state = LinkageState.RETRACTED

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

    def run(self):
        while self.state.running:             
            self.window.render(self.state, self.gui)
            self.window.process_events(self.state)

ds = DriverStation()
print("Driver station created. Running application!")
ds.run()
