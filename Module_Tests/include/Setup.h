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

/*********************************************
* Declaration of constants for the system  
* using extern -> Definition in Setup.cpp
* declaration in header necessary to access variables via their names in main.cpp
**********************************************/

//Hardware constants

//RTC Pins 
extern const unsigned char interruptPin; //SQW pin connected to interrupt0 on pin D2 on Nano 
//A4 SDA
//A5 SCL 
//RST not connected
//BAT not connected 

//SD card pins 
extern const unsigned char CS; //chip select pin 
//D11 MOSI 
//D12 MISO 
//D13 CLK

//Pinout for the PCB/Prototype circuit: 
//Indicator/status LED pin
extern unsigned char ledpin;

//MOSFET driver pins low side switches
extern unsigned char valve_1; 
extern unsigned char valve_2; 
extern unsigned char valve_3; 
extern unsigned char valve_4;
 
extern unsigned char mot_1 ;  
extern unsigned char mot_2; 

//MOSFET driver pin high side switch 
extern unsigned char hss_sig; 

//Input soil sensors
extern unsigned char sen_1;
extern unsigned char sen_2; 
extern unsigned char sen_3; 
extern unsigned char sen_4; 

//input pin battery voltage
extern unsigned char v_bat; 


//IDE internal libraries needed for the RTC & interrupt + communications  
#include <time.h>
#include <SPI.h>    //SPI communication with SD card module
#include <avr/sleep.h>
#include <Wire.h>   //I2C communication with BMP280 sensor 

//SD and RTC libraries
#include <SD.h>
#include <DS3232RTC.h>

//Temperature sensor library
#include <Adafruit_BMP280.h>

//Time variable for RTC
extern time_t t; 

// Creation of sensor reading variables soil sensors 
extern unsigned short value1; 
extern unsigned short value2; 
extern unsigned short value3; 
extern unsigned short value4; 

// Creation of sensor reading variables BMP
extern float temperature_current;
extern float pressure_current;

#endif //SETUP_H