#include "Setup.h"

/*********************************************
* Definition of constants for the system  
**********************************************/

//*** Hardware constants (see Setup.h for declaration) ***

//RTC Pins 
const unsigned char interruptPin = 2; //SQW pin connected to interrupt0 on pin D2 on Nano 
//A4 SDA
//A5 SCL 
//RST not connected
//BAT not connected 

//SD card pins 
const unsigned char CS = 10; //chip select pin 
//D11 MOSI 
//D12 MISO 
//D13 CLK

//Pinout for the PCB/Prototype circuit: 
//Indicator/status LED pin
unsigned char ledpin = A0;

//MOSFET driver pins low side switches
unsigned char valve_1 = 8; 
unsigned char valve_2 = 7; 
unsigned char valve_3 = 5; 
unsigned char valve_4 = 4;
 
unsigned char mot_1 = 3;  
unsigned char mot_2 = 6; 

//MOSFET driver pin high side switch 
unsigned char hss_sig = 9; 

//input pins soil sensors
unsigned char sen_1 = A2;
unsigned char sen_2 = A3; 
unsigned char sen_3 = A6; 
unsigned char sen_4 = A7; 

//input pin battery voltage
unsigned char v_bat = A1; 

//*** Firmware constants/variables (not included in Setup.h) ***

//Time interval for system wake-up cycles 
int wakeup_interval = 3; //TODO: Check, if int necessary

//Variables of the system 
time_t t; //Time variable to be used in RTC functions 
