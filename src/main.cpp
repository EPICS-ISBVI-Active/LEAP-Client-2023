#include <iostream>
#include <functional>
#include <wiringPi.h>
#include <fstream>
#include <bitset>

using namespace std;

// A class to store braille characters in an effort to handle better in the translation stage
class Braille {

    private:
        // A private boolean array containing the state of each Braille dot
        bool dots[6] = {false, false, false, false, false, false};

    public:
        Braille() {};

        // This sets the state of a dot to true, 
        void setDot(int index) {
            dots[index] = true;
        };

        // Converts the Braille character object to a "wide" character for use with the translation library and file output
        wchar_t toChar(bool dots[6]) {

            // Creates the base value '00000000' byte to do operations on
            uint8_t brailleCode = 0;

            for (int i = 0; i < 6; i++) {

                // This statement does bitwise operations to construct an integer representation of the braille character
                // Also uses branchless programming to optimize the branch prediction 
                brailleCode |= dots[i] << i;
            }

            // Returns a 6bit casted to a character that represents a braille character
            return (wchar_t) (0x2800 + brailleCode);
        }
};

// Currently encapsulates all GPIO pins that are accessable as inputs for debugging reasons
const int GPIO_PINS[] = {7, 11, 12, 13, 15, 16, 18, 22, 29, 31, 32, 33, 35, 36, 37, 38, 40};

// The number of pins contained in the GPIO_PINS array
const int numPins = sizeof(GPIO_PINS)/sizeof(int);

// The current storage of braille characters to be sent to the translation library
stringstream line();

// The destination file for the text data to be sent to
wofstream outfile("output.txt");

// Keeps a record of what pins are active or not
// This is necessary for determining when the space key should be activated
// as the space key descends when any dot is pressed
bitset<numPins> pinState;

// Responsible for handling the action to take when a pin is activated
void hallEffectTriggered(int pin)
{
    // TODO:
    // Add hall effect handling
    cout << "Pin " << pin << " was pressed" << endl;
}

// Responsible for clearing the bit containing 
void hallEffectDisabled(int pin)
{
    // TODO:
    // Add hall effect handling
    cout << "Pin " << pin << " was released" << endl;
}

int main()
{
    // Initializes the library   
    wiringPiSetup();

    // An array of functions that will be passed individually to each interrupt routine
    function<void()> isr_rising[numPins];
    function<void()> isr_falling[numPins];
    
    // Loops through all pins to set their respective properties
    for (int i = 0; i < sizeof(GPIO_PINS)/sizeof(*GPIO_PINS); i++) {

        // Creates an array of function pointers to store the dynamically generated function calls, passing in the pin number
        isr_rising[i] = [pin = GPIO_PINS[i]]() { hallEffectTriggered(pin); };
        isr_rising[i] = [pin = GPIO_PINS[i]]() { hallEffectDisabled(pin); };

        // Sets every pin as an input pin
        pinMode(GPIO_PINS[i], INPUT);

        // Assosciates the pin changing states with calling hallEffectTriggered
        wiringPiISR(GPIO_PINS[i], INT_EDGE_BOTH, *isr_rising[i].target<void (*)()>());
        wiringPiISR(GPIO_PINS[i], INT_EDGE_FALLING, *isr_falling[i].target<void (*)()>());
    }

    // Keeps the program running so that the interrupts function properly
    while (true) {}

    return 0;
}
