/**********************************************************
 *      C++ class for High Side Switch control
 *      Controlled hardware: Anything needing VCC and unaltered system GND (peripherals, sensors...)
 *      Written by Thomas Dickmann 
 *      H File edited: 09.01.2022
 *
 * *********************************************************/

//TODO: Clarification, if HSS and LSS differentiation makes any sense. If not -> merge classes 

#ifndef HIGHSIDESWTICH_H
#define HIGHSIDESWTICH_H

#include <Arduino.h>

//define constants 
//TODO: Static variables, scopes, point of definition? 

class HighSideSwitch
{
    public: 
        HighSideSwitch(char pinnumber); //constructor, attachting the LSS to a drive pin 
        
        void init();
        void on(); 
        void off();
        bool status();

    private: 

    char _HSSpin;
    bool _status;
};

#endif //HIGHSIDESWTICH_H