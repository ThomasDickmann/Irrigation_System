/**********************************************************
 *      C++ class for LED control in Arduino programs
 *      Written by Thomas Dickmann 
 *      Edited: 07.01.2022
 *
 * *********************************************************/

//Include guards in each header file 
#ifndef LED_H
#define LED_H

#include <Arduino.h> //including arduino library to be able to use functions like digitalWrite etc. in class methods

//class definition (tells the compiler about the existence of functions/methods and variables/properties of the class)

class LED
{
    public: //access specifier, decides wheter or not members of the class are accessible from the outside
        //"constructor" -> allows creation of objects/intances of the class (in this case, LEDs to be used in programs)
        //name matches the class name, no return type, not even void 
        LED(char pinnumber);
        
        //public class members: 
        void init(); //setup the LED for use (input/output of the pin)
        void on();
        void off(); //switiching the LED state
        bool status(); //returns the state of "LED on"=1, "LED off"=0

    private: //these class members can only be accessed from inside the class. Usually, all variables/properties from the class are private and are accessed via public functions. 
            // this way the class can't be corrupted by the main program code
        char _ledpin; 
        bool _status; 
};

#endif //LED_H