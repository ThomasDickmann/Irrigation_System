/**********************************************************
 *      C++ class for Low Side Switch control
 *      Controlled hardware: Motors, solenoid valves, lights, ... 
 *      Written by Thomas Dickmann 
 *      CPP File edited: 09.01.2022
 *
 * *********************************************************/

#include "LowSideSwitch.h"

/* private variables to be used in class functions: 
    char _LSSpin
    bool _status
 */

//constructor creates object specifying the attached pin
LowSideSwitch::LowSideSwitch(char pinnumber)
{
    _LSSpin = pinnumber; 
}

//initializes the valve object
void LowSideSwitch::init()
{
    pinMode(_LSSpin, OUTPUT); 
    digitalWrite(_LSSpin, LOW); 
}

void LowSideSwitch::on()
{
    digitalWrite(_LSSpin,HIGH);
    _status = 1 ; 
}

void LowSideSwitch::off()
{
    digitalWrite(_LSSpin,LOW);
    _status = 0; 
}

//returns the valve status on or off when called
bool LowSideSwitch::status()
{
    return _status;
}