/***********************************
 *      Arduino Irrigation System 3.0
 *      Written by Thomas Dickmann 
 *      Edited: 09.01.2022
 * *********************************/

/**********************************************************************************************************
 *    OPL project: 
 * 
 *  TODO: Solve: When creating Setup.cpp file, the build fails. Error: Multiple definitions of the pin variables. 
 *  Solved: Mixup with analog pins. Schematic adjusted and prototype resoldered, need to be fixed in PCB V3.1 
 *  TODO: Resolder LED on in socket -> Ability to take out/switch off when not needed
 *  TODO: Fix overflow problem with Seconds >60 values for interrupt alarm timer 
 *  TODO: Set correct system time on RTC
 *  TODO: Open and write sensor Data on SD card
 *  TODO: Reformat code and clean up RTC setup/ISR 
 *  TODO: Create actual service cycle (read data, interpret, act, store)
 * **********************************************************************************************************/


//Using Platformio requires to include the Arduino framework 
#include <Arduino.h>

//IDE internal libraries needed for the RTC and interrupt functionality 
#include <time.h>
#include <SPI.h>
#include <avr/sleep.h>

//SD and RTC libraries
#include <SD.h>
#include <DS3232RTC.h>

//Hardware classes 
#include "Setup.h"
#include "LED.h"
#include "LowSideSwitch.h"
#include "HighSideSwitch.h"
#include "AnalogInput.h"

//Creating hardware objects 
LED LED_Status(ledpin); 

LowSideSwitch Valve1(valve_1); 
LowSideSwitch Valve2(valve_2); 
LowSideSwitch Valve3(valve_3); 
LowSideSwitch Valve4(valve_4); 

LowSideSwitch Motor1(mot_1); 
LowSideSwitch Motor2(mot_2);

HighSideSwitch HSS(hss_sig); 
 
AnalogInput Sensor1(sen_1); 
AnalogInput Sensor2(sen_2); 
AnalogInput Sensor3(sen_3); 
AnalogInput Sensor4(sen_4); 

//Creation of File object for SD card
File myData; 

//TODO: Check and move to setup/system functions, function name is declared in "attach interrupt": 
//Function triggered when interrupt has fired, wakes up the arduino and detatches the interrupt
void wakeUp(){
  Serial.println("Interrupt has been triggered");          //Print message to serial monitor
  sleep_disable();                                         //Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(interruptPin));    //Removes the interrupt from pin 2;
}

void setup()
{
  //Initializing the hardware objects (pins, default states):
  LED_Status.init(); 

  Valve1.init(); 
  Valve2.init();
  Valve3.init(); 
  Valve4.init(); 

  Motor1.init();
  Motor2.init();

  HSS.init(); 

  Sensor1.init(); 
  Sensor2.init(); 
  Sensor3.init(); 
  Sensor4.init(); 

  Serial.begin(115200); 

  //Setup of peripherals 
  /* TODO: -> Move to Setup.cpp */
  pinMode(interruptPin, INPUT_PULLUP);  
  
  //Function to initialize the RTC, clear the alarm etc (unmodified copy from tutorial)
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);
    
    time_t t; //create a temporary time variable so we can set the time and read the time from the RTC
    t=RTC.get();//Gets the current time of the RTC
    RTC.setAlarm(ALM1_MATCH_SECONDS , second(t) + wakeup_interval, 0, 0, 0);      // Setting alarm 1 to go off every wakeup_interval seconds 
    // RTC.setAlarm(ALM1_MATCH_HOURS , 0, 0, hour(t) + wakeup_interval, 0);       // Later: Setting alarm 1 to go off once per hour for ramp up phase
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 1
    RTC.alarmInterrupt(ALARM_1, true);



  //Powering the HSS and LSS:
  HSS.on(); 
  
  Valve1.on();
  Valve2.on(); 
  Valve3.on();
  Valve4.on(); 
  Motor1.on(); 
  Motor2.on(); 
  
}

//Program routine: 
void loop()
{
  
  //preparing the arduino to go to sleep:
  sleep_enable(); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, LOW);    //attaching the interrupt, function "wakeUp" is executed when triggered ---------------> TODO: here: Function/ISR definition!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                  //setting the deepest sleep mode
  time_t t;                                                             //creates temporary variable with information on time
  t=RTC.get();                                                          //gets current time from RTC
  Serial.println("Going to sleep at: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  //prints sleep time on serial monitor
  LED_Status.off();        //turning Status LED off to indicate the controller is going to sleep
  HSS.off();               //disconnecting the vcc from sensor array
  delay(1000);                                                          //giving the LED time to turn off
  sleep_cpu();                                                          //activating sleep mode


  //code to be executed after arduino has been woken up:

LED_Status.on(); //turns on status led to indicate wakeup 
HSS.on(); //connecting power to sensors and SD card 
                                           
t=RTC.get();  //pulls the new time stamp from RTC

//printing wakeup time to the serial monitor
Serial.println();
Serial.println("Controller has been woken up");
Serial.println("Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  

//Initializing devices
delay(2500);  //allows the sensors to settle

//Initialize and check SD card 
  Serial.print("Initializing SD card...");
    if (!SD.begin(CS)) {
        Serial.println("initialization failed!");
       // return; 
    }
    else {Serial.println("initialization done.");
      }
  
Serial.println("Reading Sensors: "); 
Serial.println(Sensor1.read()); 
Serial.println(Sensor2.read()); 
Serial.println(Sensor3.read()); 
Serial.println(Sensor4.read()); 

//Acting upon the sensor signals: TODO, until then: 
  Valve1.on(); 
  Valve2.on(); 
  Valve3.on();
  Valve4.on();
  delay(100); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();   
  delay(100); 
  Valve1.on(); 
  Valve2.on(); 
  Valve3.on();
  Valve4.on();
  delay(100); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();   
  delay(100);
  Valve1.on(); 
  Valve2.on(); 
  Valve3.on();
  Valve4.on();
  delay(100); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();   
  delay(100); 
  Valve1.on(); 
  Valve2.on(); 
  Valve3.on();
  Valve4.on();
  delay(100); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();   
  delay(100); 

Serial.println("Service cycle finished!");

//Preparing for the next sleep phase

//Set New Alarm
t=RTC.get();  //pulls the new time stamp from RTC
RTC.setAlarm(ALM1_MATCH_SECONDS , second(t)+wakeup_interval, 0, 0, 0);      // Setting alarm 1 to go off once per 10 seconds for testing
// RTC.setAlarm(ALM1_MATCH_HOURS , 0, 0, hour(t)+wakeup_interval, 0);       // Setting alarm 1 to go off once per hour for ramp up phase
Serial.println("New alarm at second: " +String(second(t)+wakeup_interval));          //prints new alarm time on serial monitor
// clear the alarm flag
RTC.alarm(ALARM_1);

} 