#include <Arduino.h>

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

Adafruit_BMP280 bmp; //Creation of BMP sensor object
//Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
//Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

DS3232RTC myRTC; //RTC object 
File myFile; //DC file object 

/* *** function declarations *** */

//Print Digits function used by RTC timestamp in loop 

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}


/******* OPL test project *********** 
* write timestamp in string + print 
* read sensors and write everything to SD 
* add sleep/wakeup functionality 
************************************/ 

/* Setup function */
void setup(){
    
Serial.begin(115200);
while ( !Serial ) delay(100);   // wait for native usb

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


/********************************************   
*   Initial test of hardware and peripherals
*********************************************/

  //  *** Valves and motors check *** 
  //Open the valves sequentially 
Valve1.on(); 
Valve2.on(); 
Valve3.on(); 
Valve4.on(); 

//start both motors, fill up reservoir and lines 
  Motor1.on();
  delay(2000); //reservoir buffer needs to be bigger to prevent dry run on pump 2
  Motor2.on();
  delay(500); //TODO: Adjust, so that lines will be filled but little to no water makes it through the valves
  //stop pumps
  Motor1.off(); 
  Motor2.off(); 
  //close valves 
Valve1.off(); 
Valve2.off(); 
Valve3.off(); 
Valve4.off(); 

    
   //  *** Sensors and SD card check *** 
  //Powering the HSS 
  HSS.on(); 

//Serial.print("Sensor 1 reading: "); 
int a = Sensor1.read(); 
int b = Sensor2.read(); 
int c = Sensor3.read(); 
int d = Sensor4.read(); 
Serial.print("Sensor readings 1 - 4: "); 
Serial.print(a); 
Serial.print(' ');
Serial.print(b); 
Serial.print(' ');
Serial.print(c); 
Serial.print(' ');
Serial.println(d); 

    //Initialize SD card
    Serial.print("Initializing SD card...");

    if (!SD.begin(CS)) {
        Serial.println("initialization failed!");
        while (1);
    }
    Serial.println("initialization done.");

    //Initialize RTC 
    myRTC.begin();

    //Set system time 
    setSyncProvider(myRTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");

    //Initialize BMP sensor 
    unsigned status;

    status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID); //adressing BMP with correct bus adress 
    
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}

/* Loop function */
void loop(){

    //Print timestamp of RTC to serial monitor 
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year());
    Serial.println();

    //Print BMP Data to serial monitor 
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" hPa");

    //Write sensor data and timestamp to SD card 
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("test.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");
        // close the file:
        myFile.close();
        Serial.println("done.");
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }

    // re-open the file for reading:
    myFile = SD.open("test.txt");
    if (myFile) {
        Serial.println("test.txt:");
        Serial.println(" -- File would be read here --");
        /*
        // read from the file until there's nothing else in it:
        while (myFile.available()) {
        //Serial.write(myFile.read());
        Serial.println(" -- File would be read here");
        }*/
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
    //Read SD card data file -> check

    //Later: Include Sleep/Wakeup functinality and see if it changes anything 

    //Delay until repeat
    Serial.println();
    delay(5000);
 
}

