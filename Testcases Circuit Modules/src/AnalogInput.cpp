/**********************************************************
 *      C++ class for controlling analog inputs
 *      Controlled hardware: Analog sensors, voltage inputs etc. 
 *      Written by Thomas Dickmann 
 *      H File edited: 09.01.2022
 *
 * *********************************************************/

#include "AnalogInput.h"

//class members definition. All start with the name of the class, a double colon then the member name

//constructor
AnalogInput::AnalogInput(char pinnumber)
{
    _analogpin = pinnumber;
}

//Functions 
void AnalogInput::init()
{
    pinMode(_analogpin, INPUT); //setting the pin as an analog input 
}

int AnalogInput::read()
{
    int val = analogRead(_analogpin);
    return val;  
}
