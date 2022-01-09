#include <Arduino.h>

#include "Setup.h"
#include "LED.h"
#include "LowSideSwitch.h"
#include "HighSideSwitch.h"

//TODO solve: When creating Setup.cpp file the build fails. Error: Multiple definitions of the pin variables. 

//instanciating objects 
LED LED1(ledpin); 

LowSideSwitch Valve1(valve_1); 
LowSideSwitch Valve2(valve_2); 
LowSideSwitch Valve3(valve_3); 
LowSideSwitch Valve4(valve_4); 

LowSideSwitch Motor1(mot_1); 
LowSideSwitch Motor2(mot_2);

HighSideSwitch HSS(hss_sig); 

void setup()
{
  //initializing the hardware objects (pins, default states):
  LED1.init(); 

  Valve1.init(); 
  Valve2.init();
  Valve3.init(); 
  Valve4.init(); 

  Motor1.init();
  Motor2.init();

  HSS.init(); 

  Serial.begin(9600); 

}

void loop()
{
  //Test: Toggling LSS and LED 
  LED1.on(); 
  Valve1.on();
  Valve2.on(); 
  Valve3.on();
  Valve4.on(); 
  Motor1.on(); 
  Motor2.on(); 
  HSS.on(); 
  delay(1000); 

  Serial.println("Test"); 
  
  LED1.off(); 
  Valve1.off();
  Valve2.off(); 
  Valve3.off();
  Valve4.off(); 
  Motor1.off(); 
  Motor2.off();  
  HSS.off(); 
  delay(1000); 

  } 