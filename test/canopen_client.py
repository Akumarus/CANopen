import canopen
import logging
import time

logging.basicConfig(level=logging.DEBUG)

class CanopenClient:

    def __init__(self, server_id=2):
        self.server_id = server_id
        self.network = canopen.Network()
        logging.basicConfig(level=logging.INFO)

    def start(self):
        print("CANopen client connecting...")
        self.network.connect(bustype='slcan', channel='COM7', bitrate=500000)
        self.server_node = self.network.add_node(self.server_id, None)
        print(f"CANopen client connected. Server waiting {self.server_id}...")
        # self.server_node.object_dictionary.add_object(0x2000, 'VAR', 0x0007)
        # self.server_node.object_dictionary[0x2000].add_member('Device type', 'VAR', 0x0007,0x00000000)

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

    def listen_can_bus(self):

        """Просто слушать CAN шину"""
        print("Listening to CAN bus for 5 seconds...")
        start = time.time()

        while time.time() - start < 5:
            try:
                msg = self.network.bus.recv(timeout=0.1)
                if msg:
                    print(f"ID: 0x{msg.arbitration_id:03X} | Data: {msg.data.hex()}")
            except Exception as e:
                print(f"Receive error: {e}")

if __name__ == "__main__":
    client = CanopenClient(server_id=2)
    client.start()
    # client.listen_can_bus()
    while True:
        client.test_sdo()
        time.sleep(1)
