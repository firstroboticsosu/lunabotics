import socket
import threading
import time
import pygame
import struct
import queue

# TCP_IP = '192.168.0.10' 
TCP_IP = 'localhost'
TCP_PORT = 2000
TIMEOUT=1.0

class ConnectionManager: 
    def __init__(self): 
        self.network_thread = threading.Thread(target=self.run)
        self.connected = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.settimeout(TIMEOUT)
        self.tx_queue = queue.Queue()

        self.network_thread.start()

    def shutdown(self):
        print("Shutting down networking...")
        self.running = False
        self.network_thread.join()
        print("Shutdown network thread")

    def run(self):
        self.running = True
        while self.running:
            self.connected = False
            try:
                self.socket.connect((TCP_IP, TCP_PORT))
                self.handle_connection()
                break
            except: 
                self.connected = False
                print("Failed to connect. Sleep 1 second...")
                time.sleep(1)

    def send_packet(self, packet):
        self.tx_queue.put(packet)

    def clear_tx_queue(self):
        with self.tx_queue.mutex:
            self.tx_queue.queue.clear()

    def handle_connection(self):
        self.connected = True
        self.last_packet = time.time()
        self.clear_tx_queue()
        print("connected!") 
        
        while self.running:
            if self.tx_queue.qsize() > 0:
                msg = self.tx_queue.get_nowait()

                if msg is not None:
                    self.socket.send(msg)
                    self.last_packet = time.time()
            else:
                time.sleep(0.1)

    def is_connected(self):
        return self.connected