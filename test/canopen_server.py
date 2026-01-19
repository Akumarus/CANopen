import canopen

class CanopenServer:
    def __init__(self, node_id=2):
        self.node_id = node_id
        self.network = canopen.Network()

    def start(self):
        print(f"Запуск CANopen Slave Server [Node ID: {self.node_id}]")
        # Connect to CAN-bus with serial can
        self.network.connect(bustype='slcan', channel='COM7', bitrate='250000')
        
        self.node = self.network.create_node(self.node_id, 'server.eds')
        self.s