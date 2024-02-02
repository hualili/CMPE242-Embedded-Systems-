#!/usr/bin/env python3
import asyncio
from pymodbus import pymodbus_apply_logging_config # Not used
from pymodbus.client import AsyncModbusSerialClient

from pymodbus.exceptions import ModbusException
from pymodbus.pdu import ExceptionResponse # Not used
from pymodbus.transaction import ModbusRtuFramer

client = AsyncModbusSerialClient(
    port="/dev/ttyTHS1", # J41-8TX; J41-10RX
    framer=ModbusRtuFramer,
    baudrate=115200, # SZ controller defualt
    parity="N", # SZ controller default
    stopbits=1, # SZ controller default
)

await client.connect()
assert client.connected

try:
    wr = await client.write_register(address=0x20_0D, value=0x00_03, slave=1) # Set velocity mode
    wr = await client.write_register(address=0x20_0E, value=0x00_08, slave=1) # Enable
    wr = await client.write_registers(address=0x20_88, values=[0x00_0A, 0x_00_0A], slave=1) # Set left and right motors target speed to +10RPM

except KeyboardInterrupt:
    print("Exiting Program")

except ModbusException as exc:
    print(f"Received ModbusException({exc}) from library")

finally:
    client.close()