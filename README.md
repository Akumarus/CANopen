# CANopen

A minimalistic CANopen implementation for embedded systems.

##  Реализовано

✅ **SDO Upload** 
✅ **RPDO**
✅ **TPDO**
✅ **Object dictionary** 
❌ SDO Download
❌ NMT
❌ SYNC, EMCY, TIME 
❌ Heartbeat/Node Guarding  
❌ LSS

## Architecture
```
├── co.c/h           # CANopen core module
├── obj.c/h          # Object dictionary
├── sdo.c/h          # Service data objects
├── pdo.c/h          # Periodic data objects
├── fifo.c/h         # Message buffers
├── port.c/h         # Hardware-dependent layer
├── params.h         # Data structures
└── examples/        # Examples of use
```
## Quick start
