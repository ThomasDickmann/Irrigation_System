/***********************************
 *      Arduino Irrigation System 3.0
 *      Written by Thomas Dickmann
 *      Started 09.01.2022
 *      Edited: 16.01.2022
 * *********************************/

//Using Platformio requires to include the Arduino framework 
#include <Arduino.h>

/**********************************************************************************************************
 *  OPL project: 
 * 
 * TODO: Flesh out pseudo code for service routine 
 * TODO: aggressive behavior invoked error with loop over object array
 * TODO: Add array of water amounts per plant
 * TODO: warning comparison of signed and unsigned int with sizeof function in for loop 
 * TODO: correct value of ml_per_sec constant in setup.cpp
 * TODO: Include BMP280 functionality 
 * TODO: Set correct RTC time 
 * TODO: Move SD functionality to setup.cpp 
 * TODO: Use list of objects to cycle over for initialization? Less repeatative code
 * TODO: Open and write sensor Data on SD card every service cycle 
 * TODO: Clean up and understand RTC setup and ISR/interrupt handling 
 * TODO: Move time offset out of set alarm function, make input for function (clearer concept)
 * TODO: Resolder LED on in socket -> Ability to take out/switch off when not needed
 * TODO: Fix overflow problem with Seconds >60 values for interrupt alarm timer -> extrpolate to hours
 * TODO: Better understand SD functions 
 * TODO: BP writing multiple variables in constant format string -> Input Master Thesis %D%D%D%D%D w/ const digit space 
 * TODO: How to use the LED object and LED.cpp functions in SD setup function? 
 * 
 * Done 09.01.: Mixup with analog pins. Schematic adjusted and prototype resoldered, need to be fixed in PCB V3.1 (reorder) 
 * Done 12.01.: Understand where to declare which variables without conflict and follow BP -> Use of extern and move of declarations into setup.cpp
 * Done 12.01.: When creating Setup.cpp file, the build fails. Error: Multiple definitions of the pin variables.--> Declaration instead of definition in header file (see above) 
 * Done 12.01.: Refactor code and move init functions to setup.cpp 
 * Done 12.01.: Set up basic SD functionality in loop 
 * Done 12.01.: Added pseudocode for service routine 
 * Done 16.01.: Added blinking LED in SD setup function (indicating setup failure)
 * Done 16.01.: Created object arrays for valves and sensors 
 * Done 16.01.: Created watering routines using for loop over object arrays (DRY-logic)
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

//Creating a list of objects to loop over in the service cylce 
AnalogInput MySensors[] = {Sensor1, Sensor2, Sensor3, Sensor4};
LowSideSwitch MyValves[] = {Valve1, Valve2, Valve3, Valve4};

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
  SD_setup(ledpin); 
  
}

//Program routine: 
void loop()
{
  //Preparing the arduino for power saving sleep state ... TODO: Move sleep_cpu to RTC_sleep_prepare fct?
  RTC_sleep_prepare();
  delay(150);
  sleep_cpu();                                                          //activating sleep mode

  //Code to be executed after arduino has been woken up:
  LED_Status.on(); //turns on status led to indicate wakeup/begin of service cycle
  HSS.on();        //connecting power to sensors and SD card 
                                           
  //Get current time stamp from RTC
  get_timestamp(); 
  //Printing wakeup time to the serial monitor
  Serial.println();
  Serial.println("Controller has been woken up");
  Serial.println("Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  

  //Initializing the SD after powering it down (error indication by status led, won't work on PCB V3.0)
  SD_setup(ledpin); 

  //Allowing the system to settle for a while
  delay(1000); 

//Prototype code, replace with proper routine (below)

  //Reading the soil sensors  
  //TODO: Save values to temp. variables  
  Serial.println("Reading Sensors: "); 
  Serial.println(Sensor1.read()); 
  Serial.println(Sensor2.read()); 
  Serial.println(Sensor3.read()); 
  Serial.println(Sensor4.read()); 

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
/*
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
*/

//Code for service routine

//Read all sensor data - TODO: Better change variable definitions directly in the for loop (no repitition)
int val1 = Sensor1.read(); 
int val2 = Sensor2.read(); 
int val3 = Sensor3.read(); 
int val4 = Sensor4.read(); 

 //Fill secondary reservoir with water
 Motor1.on(); 
 delay(2000); //TODO pass fuction to give 15ml of water 
 Motor1.off();
 
 //Loop over sensors and compare values to threshold (needs to be defined in setup.cpp) 
for (int i=1; i <=4; i++) 
//warning regarding comparison of unsigned int and int? What needs to be changed? 
//sizeof MySensors equal to sizeof MyValves - not very clean, how to improve the adressing?
  {
    Serial.println("Soil moisture level plant "  + String(i) + " = "); //TODO: add value to string
    if(MySensors[i].read()  <  soil_threshold)
    {
      Serial.println("Watering need detected. Watering plant " + String(i));
      MyValves[i-1].on(); //open valve i
      Motor2.on(); //switch on pump secondary reservoir
      delay(1000); //TODO pass function to give 5ml of water, optimize with plant specific value through var. list of amount constants 
      Motor2.off(); //turn off pump secondary reservoir 
      MyValves[i-1].off(); //close valve i
    }
    else 
    { 
      Serial.println("No watering need detected for plant "  + String(i)) + ", moving on to next sensor";
      Serial.println(""); //extra line break 
    }
  }

  //TODO: Save moisture level after routine to variable for logging 
  //TODO: Log amount of water consumed by each plant in seperare variable 

  //Close all valves and swtich off both pumps again as a safety measure 
  Motor1.off(); 
  Motor2.off(); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();  

  //Create logging entry for SD card 
  Serial.println("creating data entry for SD card:");
  //Create timestamp 
  get_timestamp(); 
  String timestamp = (String(year(t)) +"/" +String(month(t)) +"/" +String(day(t)) +" - " +String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  
  Serial.println(timestamp);
  //TODO: create object in setup and only overwrite the value here each time the service cycle is performed
  String sensor_values = (String(val1));



 /* PSEUDOCODE Service Routine:
 * 
 * once watering has finished: 
 * create string with 
 * [timestamp. yy/mm/dd_hh:mm:ss; value sensor 1; water given 1; value sensor 2; water given 2; ....]
 * 
 * pass string to write_SD(string/struct/char[x]) function, open file, save data and close file  
 * 
 * short delay ?
 *  
 **********************************/
  LED_Status.off(); //turns off status led to indicate finished service cycle
  HSS.off();        //disconnecting power to sensors and SD card 

  Serial.println("Service cycle finished!");

  //Preparing for the next sleep phase, set new alarm with defined time offset 
  RTC_set_alarm();
  delay(150);

} 