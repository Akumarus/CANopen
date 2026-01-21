import canopen
import logging
import struct
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
        print("Testing NMT")
        commands = [
            (NMTCommand.STOP, "STOP"),
            (NMTCommand.START, "START"),
            (NMTCommand.PRE_OPERAT, "PRE-OPERATIONAL"),
            (NMTCommand.RESET_COMM, "RESET COMMUNICATION")
        ]

        for cmd, description in commands:
            print(f"\nSending {description}...")
            self.send_nmt_cmd(cmd)
            time.sleep(1)

            # TODO Need check heartbeat

    def send_nmt_cmd(self, cmd):
        if not self.is_connected:
            print("Not connected!")
            return self.is_connected
        try:
            client_id = 0x0000
            self.network.send_message(client_id,[cmd.value, self.server_id])
            print(f"Sent NMT command: {cmd.name} (0x{cmd.value:x02x})")
            return self.is_connected
        except Exception as e:
            print(f"Failed to send NMT command: {e}")
            return self.is_connected

    def test_tpdo(self):
        print("Testing TPDO")
        tpdo1_id = 0x180 + self.server_id
        print(f"\nSubscribing to TPDO1 (COB-ID: 0x{tpdo1_id:03X})")
        pdo_received = False
        start_time = time.time()

        while time.time() - start_time < 5:
            try:
                msg = self.network.bus.recv(timeout=0.1)
                if msg and msg.arbitration_id == tpdo1_id:
                    print(f"PDO received! Data: {msg.data.hex()}")
                    pdo_received = True

                    if len(msg.data) >= 6:
                        value1 = struct.unpack('<I', msg.data[0:4])[0]
                        value2 = struct.unpack('<H', msg.data[4:6])[0]
                        print(f"Parsed: Value1=0x{value1:08X}, Value2= 0x{value2:0x04}")
            except Exception as e:
                print(f"Error: {e}")
        
        if not pdo_received:
            print("No PDO received within timeout")

        return pdo_received
    
    def test_rpdo(self):
        print("Testing RPDO")
        rpdo1_id = 0x200 + self.server_id
        print(f"\nSending test RPDO to COB-ID 0x{rpdo1_id:03X}...")
        data = bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88])
        try:
            self.network.send_message(rpdo1_id, data)
            print(f"Send RPDO with data: {data.hex()}")
            return True
        except Exception as e:
            print(f"Fail to send RPDO: {e}")
            return False

    def test_sdo_read(self):
        print("Testing SDO read")
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
