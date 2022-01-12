/***********************************
 *      Arduino Irrigation System 3.0
 *      Written by Thomas Dickmann
 *      Started 09.01.2022
 *      Edited: 012.01.2022
 * *********************************/

//Using Platformio requires to include the Arduino framework 
#include <Arduino.h>

/**********************************************************************************************************
 *  OPL project: 
 * 
 * TODO: Flesh out pseudo code for service routine 
 * TODO: Set correct RTC time 
 * TODO: Move SD functionality to setup.cpp 
 * TODO: Open and write sensor Data on SD card every service cycle 
 * TODO: Clean up and understand RTC setup and ISR/interrupt handling 
 * TODO: Resolder LED on in socket -> Ability to take out/switch off when not needed
 * TODO: Fix overflow problem with Seconds >60 values for interrupt alarm timer -> extrpolate to hours
 * TODO: Better understand SD functions 
 * TODO: BP writing multiple variables in constant format string -> Input Master Thesis %D%D%D%D%D w/ const digit space 
 * 
 * Done 09.01.: Mixup with analog pins. Schematic adjusted and prototype resoldered, need to be fixed in PCB V3.1 (reorder) 
 * Done 12.01.: Understand where to declare which variables without conflict and follow BP -> Use of extern and move of declarations into setup.cpp
 * Done 12.01.: When creating Setup.cpp file, the build fails. Error: Multiple definitions of the pin variables.--> Declaration instead of definition in header file (see above) 
 * Done 12.01.: Refactor code and move init functions to setup.cpp 
 * Done 12.01.: Set up basic SD functionality in loop 
 * Doen 12.01.: Added pseudocode for service routine 
 * ********************************************************************************************************/

//Setup and Hardware classes includes 
#include "Setup.h" //including system constants and RTC + SD related routines
#include "LED.h" //class for LEDS
#include "LowSideSwitch.h" //class for LSS drivers
#include "HighSideSwitch.h" //class for HSS stages
#include "AnalogInput.h" //analog sensor class 

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
//TODO: Move to setup.cpp? Possible/correct practise? 
File myData; 

void setup()
{
  //Initializing the hardware objects (pins, default states)
  //Motors off, valves closed, HSS power on
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

  //Powering the HSS 
  HSS.on(); 
  
  //Testing LSS stages --> Remove when actual use planned ;) 
  Valve1.on();
  Valve2.on(); 
  Valve3.on();
  Valve4.on(); 
  Motor1.on(); 
  Motor2.on(); 

  //Starting serial communication
  Serial.begin(115200); 

  //Initializing the peripherals 
  //Initializing RTC
  RTC_setup(); 

  //Initializing SD card 
  SD_setup(); 
  
}

//Program routine: 
void loop()
{
  //Preparing the arduino for power saving sleep state
  RTC_sleep_prepare();
  LED_Status.off();        //turning Status LED off to indicate the controller is going to sleep
  HSS.off();               //disconnecting the vcc from sensor array
  delay(1000);                                                          //giving the LED time to turn off
  sleep_cpu();                                                          //activating sleep mode

  //Code to be executed after arduino has been woken up:
  LED_Status.on(); //turns on status led to indicate wakeup 
  HSS.on(); //connecting power to sensors and SD card 
                                           
  //Get current time stamp from RTC
  get_timestamp(); 
  //Printing wakeup time to the serial monitor
  Serial.println();
  Serial.println("Controller has been woken up");
  Serial.println("Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  

  //Initializing the SD after powering it down 
  SD_setup(); 

  //Allowing the system to settle for a while
  delay(1000); 

  //Reading the soil sensors  
  //TODO: Save values to temp. variables  
  Serial.println("Reading Sensors: "); 
  Serial.println(Sensor1.read()); 
  Serial.println(Sensor2.read()); 
  Serial.println(Sensor3.read()); 
  Serial.println(Sensor4.read()); 

  //TODO: Acting upon the sensor signals: TODO, until then: 
    Valve1.on(); 
    Valve2.on(); 
    Valve3.on();
    Valve4.on();
    delay(100); 
    Valve1.off();
    Valve2.off();
    Valve3.off(); 
    Valve4.off();   

  //Saving cycle data to SD card 
  //TODO: Move to setup.cpp and pass desired string to write as argument 

  //Opening file
  myData = SD.open("data.txt", FILE_WRITE);
  if (myData) {
    Serial.println("Writing to file...");
    // Write to file
    myData.println("Testing text 1, 2 ,3...");
    myData.close(); // close the file
    Serial.println("Done.");
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening data.txt");
  }

  //TODO: Remove for actual use, included just for testing purposes 
  // Reading the file
  myData = SD.open("data.txt");
  if (myData) {
    Serial.println("Read:");
    // Reading the whole file
    while (myData.available()) {
      Serial.write(myData.read());
   }
    myData.close();
  }
  else {
    Serial.println("error opening data.txt");
  }

// Pseudocode for service routine
/*********************************
 * 
 * Fill second reservoir with water
 * pump1.refill();
 * 
 * Read values of all 4 sensors 
 * compare values to threshold (needs to be defined in setup.cpp)
 * if sensorX.value<threshold
 *    close all valves (make that default at closing of loop)
 *    valveX.open 
 *    pump2.giveWater(amount) --> Water flows to plant X
 * 
 * 
 * increase counter for given water by amount 
 * 
 * repeat for all plants 
 * 
 * if counter amount > threshold --> pump1.refill? Dangerous/defeats purpose of second cycle?  
 * 
 * once watering has finished: 
 * create string with 
 * [timestamp. yy/mm/dd_hh:mm; value sensor 1; water given 1; value sensor 2; water given 2; ....]
 * 
 * pass string to write_SD(string/struct/char[x]) function, open file, save data and close file  
 * 
 * short delay ?
 *  
 **********************************/

  Serial.println("Service cycle finished!");

  //Preparing for the next sleep phase
  //Set New Alarm
  RTC_set_alarm();

} 