/**********************************************************
 *      C++ class for controlling analog inputs
 *      Controlled hardware: Analog sensors, voltage inputs etc. 
 *      Written by Thomas Dickmann 
 *      H File edited: 09.01.2022
 *
 * *********************************************************/

//Include guards in each header file 
#ifndef ANALOGINPUT_H
#define ANALOGINPUT_H

#include <Arduino.h> 

//TODO: Implement status function (check wether or not sensors are powered)

class AnalogInput
{
    public: 
        //constructor
        AnalogInput(char pinnumber);

        //public functions
        void init();
        unsigned int read(); 
      
    private: 
        char _analogpin; 
        bool _status; 
};

#endif //ANALOGINPUT_H