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

// Setup and Hardware classes includes
#include "Setup.h"          //including system constants and RTC + SD related routines
#include "LED.h"            //class for LEDS
#include "LowSideSwitch.h"  //class for LSS drivers
#include "HighSideSwitch.h" //class for HSS stages
#include "AnalogInput.h"    //analog sensor class

// Creating hardware objects
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

//Later: Creating a list of objects to loop over in the service cylce 
AnalogInput MySensors[] = {Sensor1, Sensor2, Sensor3, Sensor4};
LowSideSwitch MyValves[] = {Valve1, Valve2, Valve3, Valve4};

// Creation of BMP sensor object
Adafruit_BMP280 bmp; 
// Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor(); //TODO: Clarify what there are for 
// Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

// Creation of an Real time clock object 
DS3232RTC myRTC; 

// Creation of a file object for the SD card
File myFile;    

/* *** Move to Setup later on *** */
// Print Digits function used by RTC timestamp in loop
void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if (digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

/**********************
*   Setup function    *
**********************/
void setup()
{
    // Start serial communication
    Serial.begin(115200);
    while (!Serial)
        delay(100); // wait for native usb

    // Initializing the hardware objects (pins, default states)
    // Motors off, valves closed, HSS power on
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

    /********************************************
     *  Initial test of hardware and peripherals
     *********************************************/

    //  *** Valves and motors check ***
    Serial.println(F("Execute valve and motor performance check...")); 
    // Open the valves sequentially
    Valve1.on();
    delay(150);
    Valve2.on();
    delay(150);
    Valve3.on();
    delay(150);
    Valve4.on();
    delay(150);

    // start both motors, fill up reservoir and lines
    Motor1.on();
    delay(200); // reservoir buffer needs to be bigger to prevent dry run on pump 2
    Motor2.on();
    delay(200); // TODO: Adjust, so that lines will be filled but little to no water makes it through the valves
    // stop pumps
    Motor1.off();
    delay(150);
    Motor2.off();
    delay(150); 

    // close valves
    Valve1.off();
    delay(150);
    Valve2.off();
    delay(150);
    Valve3.off();
    delay(150);
    Valve4.off();
    delay(150);

    Serial.println(F("Valve and motor check finished. \n")); 

    //  *** Sensors and SD card check ***
    // Powering the HSS
    HSS.on();

    // Serial.print("Sensor 1 reading: ");
    value1 = Sensor1.read();
    value2 = Sensor2.read();
    value3 = Sensor3.read();
    value4 = Sensor4.read();
    Serial.print(F("Sensor readings 1 - 4: "));
    Serial.print(value1);
    Serial.print(' ');
    Serial.print(value2);
    Serial.print(' ');
    Serial.print(value3);
    Serial.print(' ');
    Serial.println(value4);

    // Initialize SD card
    SD_init(ledpin); 

    // Initialize BMP sensor
    unsigned status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID); // adressing BMP with correct bus adress

    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    // Initialize RTC
    myRTC.begin();

    // Set system time
    setSyncProvider(myRTC.get); // the function to get the time from the RTC
    if (timeStatus() != timeSet)
        Serial.println(F("Unable to sync with the RTC"));
    else
        Serial.println(F("RTC has set the system time"));

    // Prepare RTC for alarm interrupt signals
    RTC_alarm_init();

 //flashing the status LED to indicate to signal finish of setup function 
  for (int i=0; i < 5; i++){
    LED_Status.on();
    delay(150);
    LED_Status.off(); 
    delay(150);
  }

    // Cutting HSS power to SD card and sensors 
    HSS.off();

    Serial.println(F("System check and setup cycle finished. \n")); 
  //End of setup function 

}

/* Loop function */
void loop()
{

Serial.println(F("Set alarm and initialize SD card..."));  

//TODO RTC stuff 

  //Code to be executed after arduino has been woken up:
  LED_Status.on(); //turns on status led to indicate wakeup/begin of service cycle
  HSS.on();        //connecting power to sensors and SD card 

  //Initializing the SD after powering it down (error indication by status led, won't work on PCB V3.0)
  SD_init(ledpin); 

Serial.println(F("Service cycle begin... \n")); 

  /****************************************
  *** Code for reading all sensor data ****
  *****************************************/

     // Print BMP Data to serial monitor
    temperature_current = bmp.readTemperature();
    pressure_current = bmp.readPressure();
    Serial.print(F("Temperature = "));
    Serial.print(temperature_current); 
    Serial.println(F(" *C"));

    Serial.print(F("Pressure = "));
    Serial.print(pressure_current);
    Serial.println(F(" hPa"));

/*
    // Serial.print("Sensor 1 reading: ");
    value1 = Sensor1.read();
    value2 = Sensor2.read();
    value3 = Sensor3.read();
    value4 = Sensor4.read();
    Serial.println(F("Sensor readings 1 - 4: "));
    Serial.print(value1);
    Serial.println(' ');
    Serial.print(value2);
    Serial.println(' ');
    Serial.print(value3);
    Serial.println(' ');
    Serial.println(value4);
    Serial.println(' ');
*/

 Serial.print(F("RTC timestamp hh mm ss d m y: ")); 
    // Print timestamp of RTC to serial monitor
    Serial.print(hour());
    Serial.print(' ');
    Serial.print(minute());
    Serial.print(' ');
    Serial.print(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year());
    Serial.println();

    /****************************************
     ** Code for watering service routine ****
     *****************************************/

    // Fill secondary reservoir with water
    Motor1.on();
    delay(2000); // TODO pass fuction to give 15ml of water
    Motor1.off();

    // Comparision of sensor values and irrigation activity
    // Loop over sensors and compare values to threshold (needs to be defined in setup.cpp)
    Serial.println(F("Reading soil sensors, checking for water demand: "));
    for (int i = 0; i < 4; i++)
    {
      Serial.print(F("Sensor "));
      Serial.print(i + 1);
      Serial.print(F(": "));
      Serial.println(MySensors[i].read());

      if (MySensors[i].read() < soil_threshold)
      {
        Serial.print(F("Watering need detected. Watering plant "));
        Serial.println(i + 1);
        MyValves[i].on();  // open valve i
        Motor2.on();       // switch on pump secondary reservoir
        delay(1000);       // TODO pass function to give 5ml of water, optimize with plant specific value through var. list of amount constants
        Motor2.off();      // turn off pump secondary reservoir
        MyValves[i].off(); // close valve i
      }

      else
      {
        Serial.print(F("No watering need detected for plant "));
        Serial.println(i + 1);
      }
    }

  Serial.println(F("Soil moisture level check and watering routine finished. \n")); 

    // Write sensor data and timestamp to SD card
    myFile = SD.open("Data.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile)
    {
        Serial.print(F("Writing to Data.txt..."));
        // Data log entry to be written: 
        //myFile.println("testing 1, 2, 3.");
        // Print timestamp of RTC 
        myFile.print(hour());
        myFile.print(' ');
        myFile.print(minute());
        myFile.print(' ');
        myFile.print(second());
        myFile.print(' ');
        myFile.print(day());
        myFile.print(' ');
        myFile.print(month());
        myFile.print(' ');
        myFile.print(year());
        myFile.println();
        // Print analog sensor data 
        myFile.print(value1); 
        myFile.print(' ');
        myFile.print(value2); 
        myFile.print(' ');
        myFile.print(value3); 
        myFile.print(' ');
        myFile.print(value4); 
        myFile.print(' ');
        myFile.println();
        // Print ambient sensor data 
        myFile.print(temperature_current); 
        myFile.print(' ');
        myFile.print(pressure_current); 
        myFile.println(" \n");
        //TODO: Data on irrigation activity

        // close the file:
        myFile.close();
        Serial.println(F("done."));
    }
    else
    {
        // if the file didn't open, print an error:
        Serial.println(F("Error opening Data.txt"));
    }

    // re-open the file for reading:
    myFile = SD.open("Data.txt");
    if (myFile)
    {
        Serial.println(F("Data.txt:"));
        // read from the file until there's nothing else in it:
        //while (myFile.available()) {
        //Serial.write(myFile.read());
        //}
        // close the file:
        myFile.close();
    }
    else
    {
        // if the file didn't open, print an error:
        Serial.println(F("Error opening Data.txt"));
    }

    // Redundant security measure: Close all valves and swtich off both pumps again
    Motor1.off();
    Motor2.off();
    Valve1.off();
    Valve2.off();
    Valve3.off();
    Valve4.off();

    LED_Status.off(); // turns off status led to indicate finished service cycle
    
    // Cutting power to sensors and SD card
    HSS.off();        

    Serial.println(F("Service cycle finished!\n"));

    // End of loop function

    // Delay until repeat
    delay(5000);
}
