#include <Arduino.h>

#include <Wire.h> //I2C communication with BMP280 sensor and RTC

//SD libraries
#include <SPI.h> //SPI communication with SD card module
#include <SD.h>

//RTC libraries
#include <DS3232RTC.h>
#include <time.h>
//#include <avr/sleep.h>

//Temperature sensor library
#include <Adafruit_BMP280.h>


/*  -------------- Pinout for circuit board ------------- */ 
//RTC Pins 
//const unsigned char interruptPin = 2; //SQW pin connected to interrupt0 on pin D2 on Nano 
//A4 SDA
//A5 SCL 
//RST not connected
//BAT not connected 

//SD card pins 
int CS = 10; //chip select pin 
//D11 MOSI 
//D12 MISO 
//D13 CLK

//Pinout for the PCB/Prototype circuit: 
//Indicator/status LED pin
//unsigned char ledpin = A0;

//MOSFET driver pins low side switches
/*unsigned char valve_1 = 8; 
unsigned char valve_2 = 7; 
unsigned char valve_3 = 5; 
unsigned char valve_4 = 4;
 
unsigned char mot_1 = 3;  
unsigned char mot_2 = 6; */

//MOSFET driver pin high side switch 
const int hss_sig = 9; 

//input pins soil sensors
/*unsigned char sen_1 = A2;
unsigned char sen_2 = A3; 
unsigned char sen_3 = A6; 
unsigned char sen_4 = A7; */

//input pin battery voltage
//unsigned char v_bat = A1; 

/*  -------------- Pinout for circuit board ------------- */ 

//Object creation 
Adafruit_BMP280 bmp; //Creation of BMP sensor object
//Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
//Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

//Object creation 
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
    
    //Powering HSS (SD card + analog sensors)
    pinMode(hss_sig, OUTPUT); 
    digitalWrite(hss_sig, LOW); 

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

        // read from the file until there's nothing else in it:
        while (myFile.available()) {
        Serial.write(myFile.read());
        }
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

