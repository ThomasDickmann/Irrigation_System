#include <Arduino.h>

//IDE internal libraries needed for the RTC & interrupt + communications  
#include <time.h>
#include <SPI.h> //SPI communication with SD card module
#include <avr/sleep.h>
#include <Wire.h> //I2C communication with BMP280 sensor 

//SD and RTC libraries
#include <SD.h>
#include <DS3232RTC.h>

//Temperature sensor library
#include <Adafruit_BMP280.h>

//Object creation 
Adafruit_BMP280 bmp; //Creation of BMP sensor object
//Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
//Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

void setup(){
    Serial.begin(115200);
    while ( !Serial ) delay(100);   // wait for native usb

    unsigned status;

    status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID); //adressing BMP with correct bus adress 
    
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    }


void loop(){
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" hPa");

    Serial.println();
    delay(2000);
 
}