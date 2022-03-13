/**********************************************************
 *      C++ class for Low Side Switch control
 *      Controlled hardware: Motors, solenoid valves, lights, ... 
 *      Written by Thomas Dickmann 
 *      H File edited: 09.01.2022
 *
 * *********************************************************/

#ifndef LOWSIDESWTICH_H
#define LOWSIDESWTICH_H

#include <Arduino.h>

//define constants 
//TODO: Static variables, scopes, point of definition? 

class LowSideSwitch
{
    public: 
        LowSideSwitch(char pinnumber); //constructor, attachting the LSS to a drive pin 
        
        void init();
        void on(); 
        void off();
        bool status();

    private: 

    char _LSSpin;
    bool _status;
};

#endif //LOWSIDESWTICH_H