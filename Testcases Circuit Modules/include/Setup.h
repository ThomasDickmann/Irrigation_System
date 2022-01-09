/**********************************************************
 *      System Setup for Arduino Irrigation System
 *      V3.1
 *      Written by Thomas Dickmann 
 *      H File 
 *      Edited: 09.01.2022
 *
 * *********************************************************/

#ifndef SETUP_H
#define SETUP_H

#include "Arduino.h"

unsigned char ledpin = A6; 

//Pinout for the circuit: 
//MOSFET driver pins low side switches
unsigned char valve_1 = 8; 
unsigned char valve_2 = 7; 
unsigned char valve_3 = 6; 
unsigned char valve_4 = 5;
 
unsigned char mot_1 = 3;  
unsigned char mot_2 = 4; 

//MOSFET driver pin high side switch 
unsigned char hss_sig = 9; 

//input pins soil sensors
unsigned char sen_1 = A0;
unsigned char sen_2 = A1; 
unsigned char sen_3 = A2; 
unsigned char sen_4 = A3; 

//input pin battery voltage
unsigned char v_bat = A7; 

//SD card pins 
unsigned char CS = 10; //chip select pin 
//D11 MOSI 
//D12 MISO 
//D13 CLK

//RTC Pins 
unsigned char SQW = 2; //interrupt0 on pin D2 on Nano 
//A4 SDA
//A5 SCL 
//RST not connected
//BAT not connected 


/*********************************************
 *      function prototypes for system setup
 * ********************************************/

//TODO

#endif //SETUP_H