/***************************************
 *      Arduino Irrigation System V3.1
 *      Written by Thomas Dickmann
 *      Started 09.01.2022
 ***************************************/

//Using Platformio requires to include the Arduino framework 
#include <Arduino.h>

/**********************************************************************************************************
 *  OPL project: 
 * 
 * TODO: Resolve conflict with RTC and BMP. When taking RTC out of the picture completely, things seem to work fine 
 * No address conflict - checked via I2C scanner:
 * RTC I2C address is 0x68 
 * BMP I2C address is 0x76
 * TODO: Sleep.h not found when using 33IoT envorionment in Platformio.ini (build): https://www.arduino.cc/reference/en/libraries/arduino-low-power/
 * TODO: When putting multiple environments in the platformio.ini file, the serial monitor doesn't work anymore 
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
 * TODO: Fix overflow problem with Seconds >60 values for interrupt alarm timer -> extrpolate to hours
 * TODO: Better understand SD functions 
 * TODO: BP writing multiple variables in constant format string -> Input Master Thesis %D%D%D%D%D w/ const digit space 
 * TODO: How to use the LED object and LED.cpp functions in SD setup function? 
 * 
 * TODO: Reserach target specific code sections (IoT33 vs. regular nano): https://community.platformio.org/t/changing-destination-boards/4751/2
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

//Creating a list of objects to loop over in the service cylce 
AnalogInput MySensors[] = {Sensor1, Sensor2, Sensor3, Sensor4};
LowSideSwitch MyValves[] = {Valve1, Valve2, Valve3, Valve4};

//Creation of File object for SD card 
//TODO: Move to setup.cpp? Possible/correct practise? 
File myData; 

//Creation of BMP sensor object
Adafruit_BMP280 bmp; 
//CHECK - object creation doesn't fuck up the serial communication 

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

  //Starting serial communication
  Serial.begin(115200); 
  
/********************************************   
*   Initial test of hardware and peripherals
*********************************************/

  //  *** Valves and motors check *** 
  //Open the valves sequentially 
  Serial.println("Checking valves and motors... "); 
  for (int i=0; i <4; i++){
    MyValves[i].on();
    delay(150);
  }
  //start both motors, fill up reservoir and lines 
  Motor1.on();
  delay(2000); //reservoir buffer needs to be bigger to prevent dry run on pump 2
  Motor2.on();
  delay(500); //TODO: Adjust, so that lines will be filled but little to no water makes it through the valves
  //stop pumps
  Motor1.off(); 
  Motor2.off(); 
  //close valves 
  for (int i=0; i <4; i++){
    MyValves[i].off();
  }
  Serial.println("Valve and motor check finished. \n"); 

   //  *** Sensors and SD card check *** 
  //Powering the HSS 
  HSS.on(); 

//Adding the sensor check leads to SD card not being able to open the file... wtf. 
/* 
  //Checking the analog sensor readings: 
  Serial.println("Checking soil sensors and water demand... "); 
  for (int i=0; i <4; i++){
  Serial.print("Sensor "); 
  Serial.print(i+1);
  Serial.print(": ");
  Serial.println(MySensors[i].read());
  if(MySensors[i].read()  <  soil_threshold){
      Serial.print("Watering need detected. Watering plant ");
      Serial.println(i+1); 
    }
  else{ 
      Serial.print("No watering need detected for plant ");
      Serial.println(i+1); 
    }
  }
Serial.println("Analog sensor check finished. \n"); 
*/

 
  //When using the BMP, the code compiles, but it fucks up the RTC timestamps on the serial monitor 
  bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID); //adressing BMP with correct bus adress 
  //CHECK - Object initialization fucks up writing the timestamp data entry for the SD card to the monitor
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
          Adafruit_BMP280::SAMPLING_X2,               // Temp. oversampling 
          Adafruit_BMP280::SAMPLING_X16,              // Pressure oversampling 
          Adafruit_BMP280::FILTER_X16,                // Filtering. 
          Adafruit_BMP280::STANDBY_MS_500);           // Standby time. 
  //CHECK - Settings don't seem to fuck up more stuff 

  //  *** RTC setup and check *** 
/*
  //Initializing RTC (only needed once)
  RTC_setup(); 
*/ 

 //flashing the status LED to indicate to signal finish of setup function 
  for (int i=0; i <4; i++){
    LED_Status.on();
    delay(150);
    LED_Status.off(); 
    delay(150);
  }

  Serial.println("System check and setup cycle finished. \n"); 
  //End of setup function 
}


void loop()
{
/*************************************************
* Code for the sleep state routine between loops *
**************************************************/

Serial.println("--- Set alarm and initialize SD card --- ");  
  /*
  //Set new alarm with defined time offset 
  RTC_set_alarm(); //Writes the new alarm time to the serial monitor
  delay(100); 
  //Hypothesis: Serial was doing weird shit because the final step in set_alarm is to write the time, while the first step in the next function was to get the time.
  //probably timestap changed while trying to write it to the serial when calling set_alarm right right after calling it before in setup, or after sleep prepare with get timestamp

  //Preparing the arduino for power saving sleep state ... TODO: Move sleep_cpu to RTC_sleep_prepare fct?
  RTC_sleep_prepare(); //writes the time of going to sleep to the serial monitor 
  delay(100); 
  sleep_cpu();  //activating sleep mode
*/

  //Code to be executed after arduino has been woken up:
  LED_Status.on(); //turns on status led to indicate wakeup/begin of service cycle
  HSS.on();        //connecting power to sensors and SD card 

/*                                        
  //Get current time stamp from RTC
  get_timestamp(); 
  //Printing wakeup time to the serial monitor
  Serial.println("Controller has been woken up");
  Serial.print("Time: ");
  Serial.print(String(hour(t)));
  Serial.print(":");
  Serial.print(String(minute(t)));
  Serial.print(":");
  Serial.println(String(second(t)));  
*/
  //Initializing the SD after powering it down (error indication by status led, won't work on PCB V3.0)
  SD_setup(ledpin); 

  //Allowing the system to settle for a while
  delay(300); 

  Serial.println("--- Service cycle begin --- \n"); 

  /****************************************
  *** Code for reading all sensor data ****
  *****************************************/

  //BMP 280 temperature and pressure sensor 
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" hPa");
  //CHECK - Using these functions in Loop fucks up the RTC and Serial real good. No more Strings before timestamps in set_alarm and sleep_prepare, timestamp SD only consists of seconds


  //Create logging entry for SD card 
  Serial.println("Creating data entry for SD card:");
  //timestamp = (String(year(t)) +"/" +String(month(t)) +"/" +String(day(t)) +" - " +String(hour(t))+":"+String(minute(t))+":"+String(second(t)));  
  //String sensor_data = (String sensor 1-4, string temperature, string pressure, string amount)
  Serial.println("testprint"); //+ sensor_data
  //TODO: create string object in setup and only overwrite the value here each time the service cycle is performed
  //TODO: Write newly overwritten string object to sd card with function (input string, function details in setup.cpp)

  //Test code for SD card functionality 
  myData = SD.open("data.txt", FILE_WRITE);
  if (myData) {
    Serial.print("Writing to file... ");
    // Write to file
    myData.print(String(hour(t)));
    myData.print(":");
    myData.print(String(minute(t)));
    myData.print(":");
    myData.println(String(second(t))); 
   
    myData.print("Data Sensor 1"); 
    myData.print("Data Sensor 2"); 
    myData.print("Data Sensor 3"); 
    myData.print("Data Sensor 4");

    myData.print("Bool Pump 1");
    myData.print("Bool Pump 2");

    myData.close(); // close the file

    Serial.println("Data written successfully! \n");
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("Error opening data.txt \n");
  }



/****************************************
** Code for watering service routine ****
*****************************************/

 //Fill secondary reservoir with water
 Motor1.on(); 
 delay(2000); //TODO pass fuction to give 15ml of water 
 Motor1.off();
 
 //Loop over sensors and compare values to threshold (needs to be defined in setup.cpp) 
  Serial.println("Reading soil sensors, checking for water demand: "); 
  for (int i=0; i <4; i++){
  Serial.print("Sensor "); 
  Serial.print(i+1);
  Serial.print(": ");
  Serial.println(MySensors[i].read());

  if(MySensors[i].read()  <  soil_threshold){
      Serial.print("Watering need detected. Watering plant ");
      Serial.println(i+1); 
      MyValves[i].on(); //open valve i
      Motor2.on(); //switch on pump secondary reservoir
      delay(1000); //TODO pass function to give 5ml of water, optimize with plant specific value through var. list of amount constants 
      Motor2.off(); //turn off pump secondary reservoir 
      MyValves[i].off(); //close valve i
    }

  else{ 
      Serial.print("No watering need detected for plant ");
      Serial.println(i+1); 
    }
  }


  //TODO: Create entry for amount in data entry for routine 

  //Redundant security measure: Close all valves and swtich off both pumps again
  Motor1.off(); 
  Motor2.off(); 
  Valve1.off();
  Valve2.off();
  Valve3.off(); 
  Valve4.off();  

  LED_Status.off(); //turns off status led to indicate finished service cycle
  HSS.off();        //disconnecting power to sensors and SD card 

  Serial.println("--- Service cycle finished! ---\n");

  //End of loop function 
  
  } 