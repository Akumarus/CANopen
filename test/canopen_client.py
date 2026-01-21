import canopen
import logging
import time
import enum

logging.basicConfig(level=logging.DEBUG)

class NodeState(enum.Enum):
    BOOT_UP = 0x00
    STOPPED = 0x04
    OPERATIONAL = 0x05
    PRE_OPERATIONAL = 0x7F

class NMTCommand(enum.Enum):
    START = 0x01
    STOP = 0x02
    PRE_OPERAT = 0x80
    RESET_NODE = 0x81
    RESET_COMM = 0x82

class CanopenClient:
    def __init__(self, server_id=2, bustype='slcan', channel='COM7', bitrate='500000'):
        self.server_id = server_id
        self.bustype = bustype
        self.channel = channel
        self.bitrate = bitrate
        self.network = None
        self.server_node = None
        self.is_connected = None

        self.standard_objects = {
            0x1000: "Device Type",
            0x1001: "Error Regiser",
            0x1008: "Manufacturer Device Name",
            0x1009: "Manufacturer Hardware Version",
            0x100A: "Manufacturer Software Version",
            0x1018: "Identity Object",
            0x1F80: "NMT Startup",
        }

    def start(self):
        try:
            print(f"Connecting to CAN bus: {self.channel}, {self.bitrate} bps")
            # Connect to CAN bus
            self.network = canopen.Network()
            self.network.connect(
                bustype=self.bustype, 
                channel=self.channel, 
                bitrate=self.bitrate)
            # Add server node 
            self.server_node = self.network.add_node(self.server_id, None)
            self.is_connected = True
            print(f"Connected to CAN bus. Server node ID: {self.server_id}")
            # Monitoring CAN bus in background mode
            self.network.scanner.search()
            return self.is_connected
        
        except Exception as e:
            print(f"Connection failed: {e}")
            self.is_connected = False
            return self.is_connected

    def stop(self):
        if self.network:
            self.network.disconnect()
            self.is_connected = False
            print("Disconnected form CAN bus")

    def test_nmt(self):
        print("Testing NMT protocol (Clinet --> Server)")
        states = {
            0x00: "BOOTUP",
            0x04: "STOPPED",
            0x05: "OPERATIONAL",
            0x7F: "PRE-OPERATIONAL"
        }

        print("Starting server in Operation mode")
        self.network.send_message(0x000, [0x01, self.server_id])
        print(f"Clint --> Server {self.server_id}: START")
        time.sleep(2)

        print("Monitoring Heartbeat:")
        hb_received = False
        start_time = time.time()
        while time.time() - start_time < 3:
            msg = self.network.bus.recv(timeout=0.1)
            if msg and msg.arbitration_id == 0x700 + self.server_id:
                state = states.get(msg.data[0], f"UNKNOWN({msg.data[0]:02X})")
                print(f"Server --> Client: Heartbeat = {state}")
                hb_received = True
                break
        
        # if not hb_received:
        #     print("Heartbeat is not received")

    def test_sdo(self):
        # self.network.send_message(0x000, [0x01, self.server_id])
        print("Testing SDO protocol (Clinet --> Server)")
        
        # print("Reading data from Server")
        # vendor_id = self.server_node.sdo[0x1018][1].raw
        # print(f"Vendor ID: 0x{vendor_id:08X}")

        # product_code = self.server_node.sdo[0x1018][2].raw
        # print(f"Product Code: 0x{product_code}")
        try:  
            # test_val = self.server_node.sdo[0x2000][0].raw
            device_type = self.server_node.sdo.upload(0x1000, 0)
            # print(f"Test variable 0x2000: {test_val} (0x{test_val:04X})")
        except: 
            print("ERROR: Object 0x2000 not found on server!")

if __name__ == "__main__":
    client = CanopenClient()
    client.start()
    # client.listen_can_bus()
    # while True:
    #     client.test_sdo()
    #     time.sleep(1)
