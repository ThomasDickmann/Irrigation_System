/**********************************************************
 *      C++ class for LED control in Arduino programs
 *      Written by Thomas Dickmann 
 *      Edited: 07.01.2022
 * 
 *  *********************************************************/

#include "LED.h"

//class members definition. All start with the name of the class, a double colon then the member name

//constructor
LED::LED(char pinnumber)
{
    _ledpin = pinnumber; //storing the pin number in the private variable _ledpin -> all other functions/methods interacting with the led pin should not be influenced by the main code!therefore: public function (constructor) sets/links the pin used in the actual program to the created object and every other function refers to the internal/private variable _pin which has been set only once (in the beginning)
}

//Functions 
void LED::init(){
    pinMode(_ledpin, OUTPUT); //setting the pin as an output
    digitalWrite(_ledpin, LOW); //switching the LED off as the default state
}

//commands to the led
void LED::on(){
    digitalWrite(_ledpin,HIGH);
    _status =1 ; //sets the status accordingly
}

void LED::off(){
    digitalWrite(_ledpin,LOW); 
    _status = 0; //sets the status accordingly
}

bool LED::status(){ //example of communication between/with objects
    return _status;
}