#include "Setup.h"

/*********************************************
* Definition of constants for the system  
**********************************************/

//RTC Pins 
const unsigned char interruptPin = 2; //SQW pin connected to interrupt0 on pin D2 on Nano 
//A4 SDA
//A5 SCL 
//RST not connected
//BAT not connected 

//Time interval for system wake-up cycles 
int wakeup_interval = 5; //TODO: Check, if int necessary

//SD card pins 
const unsigned char CS = 10; //chip select pin 
//D11 MOSI 
//D12 MISO 
//D13 CLK

//Pinout for the PCB/Prototype circuit: 
unsigned char ledpin = A0;

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
/* PINTOUT PCB V3.0:
unsigned char sen_1 = A0;
unsigned char sen_2 = A1; 
unsigned char sen_3 = A2; 
unsigned char sen_4 = A3; 
/*/
//Prototype circuit and PCB V3.1
unsigned char sen_1 = A2;
unsigned char sen_2 = A3; 
unsigned char sen_3 = A6; 
unsigned char sen_4 = A7; 

//input pin battery voltage
unsigned char v_bat = A1; 

//Variables of the system 
time_t t; //Time variable to be used in RTC functions 


/*********************************************
* Function bodies for the system setup 
**********************************************/

//Real time clock functionality

//Function triggered when interrupt has fired (used in function "attachInterrupt") 
//Wakes up the arduino from sleep state and detatches the interrupt
void wakeUp()
{
  Serial.println("Interrupt has been triggered");          //Print message to serial monitor
  sleep_disable();                                         //Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(interruptPin));    //Removes the interrupt from pin 2;
}


//Function to initialize the RTC, clear the alarm etc (unmodified copy from tutorial)
// initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
 void RTC_init()
 {
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
 }

//Set an Alarm 
void RTC_set_alarm()
{
  t=RTC.get();//Gets the current time of the RTC
  RTC.setAlarm(ALM1_MATCH_SECONDS , second(t) + wakeup_interval, 0, 0, 0);      // Setting alarm 1 to go off every wakeup_interval seconds 
    // RTC.setAlarm(ALM1_MATCH_HOURS , 0, 0, hour(t) + wakeup_interval, 0);     // Later: Setting alarm 1 to go off once per hour for ramp up phase
    // clear the alarm flag
  RTC.alarm(ALARM_1);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
  RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 1
  RTC.alarmInterrupt(ALARM_1, true);
  Serial.println("Alarm has been set for: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));
}

//preparing the arduino to go to sleep:
void RTC_sleep_prepare()
  {
  sleep_enable(); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, LOW);    //attaching the interrupt, function "wakeUp" is executed when triggered---> here: Function/ISR definition!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                  //setting the deepest sleep mode                                                          //creates temporary variable with information on time
  t=RTC.get();                                                          //gets current time from RTC
  Serial.println("Going to sleep at: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  //prints sleep time on serial monitor
  }

  time_t get_timestamp()
  {
    t=RTC.get();  //pulls the new time stamp from RTC
    return t; 
  }

void RTC_setup() //single function to set up the RTC in the setup loop of main.cpp
  {
    pinMode(interruptPin, INPUT_PULLUP);  
    RTC_init();
    RTC_set_alarm(); 
  }


//SD data logging functionality

void SD_setup()
  {
  Serial.print("Initializing SD card...");
    if (!SD.begin(CS)) {
        Serial.println("initialization failed!");
    }
    else {Serial.println("initialization done.");
      }
  }

void SD_writeData(char data)
  {
    //TODO: Write combined data array of soil moisture and watering activity per plant to SD card 
  }