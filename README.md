![Use Case Diagram](https://github.com/ThomasDickmann/Irrigation_System/blob/main/images/Watering_System_ASM%20v13.png)

# Arduino Irrigation System
The purpose of this project was to learn basic electronic design, PCB layout, and embedded software development using VS code and PlatformIO.

Written in 2022 by Thomas Dickmann. 

## System architecture and intended use 
The goal is to create a fully automated irrigation system to be used in a home automation context. The hardware is capable of controlling two 12V pumps and four 12V solenoid valves to independently give water to up to four plants. The dual pump setup is used to create a redundant safety feature to prevent flooding upon a controller malfunction. The soil moisture level is obtained using capacitive analog sensors. Additionally, a BMP280 temperature and pressure sensor can provide ambient information. Finally, the connection of a SD card module allows for data logging and a Real Time Clock (RTC) provides time stamps and allows the system to operate in an energy efficient deep sleep state in between the service routines.     

## Content 
This repo currently contains the arduino code for the controller. The project also comprises Autodesk EAGLE files for the schematic and PCB as well as a CAD package with Fusion360 and .stl files for the various mechanical parts to put together the final product. Additionally, of-the-shelf components make up a substantial part of the system, therefore a list will be published containing links to the various components that were used. 
Finally, a manual will be created after everything is wrapped up and tested. 

### Features
+ 6 MOSFET Low side switches (LSS) with indicator LEDs for valves and motors, providing 12V input power to the peripherals 
+ PWM enabled motor control for power modulation 
+ 4 analog sensor inputs with filter capacitors 
+ High Side Switch (HSS) to power analog sensors and SD card only when in use (to reduce energy consumption) 
+ Data logging of soil moisture levels, temperature and air pressure with time stamps for long term observation  
