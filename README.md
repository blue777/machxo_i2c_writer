# machxo_i2c_writer
MachXO2 configuration writer via generic I2C.



# How to use

## Linux

In order to use the I2C interface, it is a prerequisite to operate with SBC such as Raspberry Pi and Nano Pi NEO/NEO2.



(1) build

```
apt install g++
g++ -Ofast -std=c++11 MachXO_Writer.cpp -o machxo_writer.o
```



(2) run

```
 ./machxo_writer.o <Hex File>
```



## Windows
Use Microchip MCP2221 for the I2C interface.

IDE: Visual Studio



(1) Get MCP2221 library

https://www.microchip.com/en-us/product/MCP2221A#document-table

MCP2221 DLL (v2.2.1)



(2) Extract and placed there.

vs\_libs/MCP2221_DLL/unmanaged/dll/

​	mcp2221_dll_um.h

​	mcp2221_dll_um_x64.dll

​	mcp2221_dll_um_x64.lib

​	mcp2221_dll_um_x86.dll

​	mcp2221_dll_um_x86.lib



(3) Compile with Visual Studio 2022



(4) Execute

```
MachXO_Writer.exe <HexFile>
```



(5) Execution log

```
# ./machxo_writer.o R2R_DAC.hex

-------------------------------------
     MachXO configuration writer
-------------------------------------

DeviceID    = 0x2b012b01
UserCode    = 0x00000000
Status      = 0x08100810
FeatureBits =     0xffff

Entering Offline Configuration
Erasing Configuration and UFM
Waiting for erase to complete
Loading HEX
Loading HEX 575 pages written
Loading HEX 0 bytes left over
Programming DONE status bit
Refreshing image

DONE.
```





# How to prepare Hex File.



(1) Generate JEDEC file for target LATTICE MachXO.

(2) Run LATTICE DIAMOND / Deployment Tool

(3) Select "JEDEC to Hex" on Output File

(4) Convert JEDEC file.







