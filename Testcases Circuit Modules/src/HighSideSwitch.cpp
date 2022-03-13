/**********************************************************
 *      C++ class for High Side Switch control
 *      Controlled hardware: Anything needing VCC and unaltered system GND (peripherals, sensors...)
 *      Written by Thomas Dickmann 
 *      CPP File edited: 09.01.2022
 *
 * *********************************************************/

#include "HighSideSwitch.h"

/* private variables to be used in class functions: 
    char _HSSpin
    bool _status
 */

//constructor creates object specifying the attached pin
HighSideSwitch::HighSideSwitch(char pinnumber)
{
    _HSSpin = pinnumber; 
}

//initializes the valve object
void HighSideSwitch::init()
{
    pinMode(_HSSpin, OUTPUT); 
    delay(100);
    /*initial on-switch after setting pin as output 
     yielded strage switch voltage 
     uneven step response w/ about 3.3V peak and breakdown to about 2V after 
     -> adding delay to define on time upon initialization
    */
    digitalWrite(_HSSpin, HIGH); //Status off
}

void HighSideSwitch::on()
{
    digitalWrite(_HSSpin,LOW);
    _status = 1 ; 
}

void HighSideSwitch::off()
{
    digitalWrite(_HSSpin,HIGH);
    _status = 0; 
}

//returns the valve status on or off when called
bool HighSideSwitch::status()
{
    return _status;
}