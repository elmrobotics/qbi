# Qbi Demo #

##QBI-1 Branch##

This repository contains the LinkIT and Arduino code running on the Qbi and ActionHub demo units.

[Setting up Qbi 1 - Video](https://www.youtube.com/watch?v=S4AbExmMUKg)

### Qbi ###

* LinkITONE Main Board
* Controller cube.  Orienting the cube causes a side to light up and the ActionHub to turn on/off one of two devices attached to the ActionHub.

### QbiUtil ###

* Adafruit Trinket Pro 5 volt 
* Used for controlling the NeoPixel edge lighting system in the Qbi.  I2C to One Wire

### ActionHub ###

* LinkITONE Main Board
* Server that receives commands from the Qbi Cube

### ActionHubUtil ###

* Adafruit Trinket Pro 5 volt 
* Used as a control unit for the 2 poder relays (opperating at 5 Volts)
* I2C used for communication