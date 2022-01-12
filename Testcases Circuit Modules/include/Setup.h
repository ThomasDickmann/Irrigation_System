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
**********************************************/

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

//Declaration of constants for the system 

//Pinout for the PCB/Prototype circuit: 
extern unsigned char ledpin;

//Pinout for the circuit: 
//MOSFET driver pins low side switches
extern unsigned char valve_1; 
extern unsigned char valve_2; 
extern unsigned char valve_3; 
extern unsigned char valve_4;
 
extern unsigned char mot_1 ;  
extern unsigned char mot_2; 

//MOSFET driver pin high side switch 
extern unsigned char hss_sig; 

//input pins soil sensors
/* PINTOUT PCB V3.0:
unsigned char sen_1 = A0;
unsigned char sen_2 = A1; 
unsigned char sen_3 = A2; 
unsigned char sen_4 = A3; 
/*/
//Prototype and PCB V3.1
extern unsigned char sen_1;
extern unsigned char sen_2; 
extern unsigned char sen_3; 
extern unsigned char sen_4; 

//input pin battery voltage
extern unsigned char v_bat; 

//IDE internal libraries needed for the RTC and interrupt functionality 
#include <time.h>
#include <SPI.h>
#include <avr/sleep.h>

//SD and RTC libraries
#include <SD.h>
#include <DS3232RTC.h>

//Time variable for RTC
extern time_t t; 

/*********************************************
 *   Function prototypes for system setup and use 
 * ********************************************/

void wakeUp();

void RTC_init();

void RTC_set_alarm();

void RTC_sleep_prepare();

time_t get_timestamp();

void RTC_setup(); 

//TODO: SD functionality

void SD_setup(); 

void SD_writeData(char data); //TODO: struct, char, array char[10]? Write function body 

//TODO: remaining service cycle functions 

#endif //SETUP_H