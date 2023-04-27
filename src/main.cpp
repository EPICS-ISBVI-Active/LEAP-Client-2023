/**
 * @author Nathan Fassnacht
 * Contact: foz@fozfuncs.com
*/

#include <iostream>
#include <functional>
#include <wiringPi.h>
#include <fstream>
#include <bitset>
#include <sstream>
#include <cstringt.h>

using namespace std;

#define BUTTON 35
#define ADVANCE 32
#define SPACE 23
#define BACKSPACE 36

// A class to store braille characters in an effort to handle better in the translation stage
class Braille {

    private:
        // A private boolean array containing the state of each Braille dot
        bool dots[6] = {false};

    public:
        Braille() {

        };

        // Resets the state of each dot to false
        void clear() {

            // Re-allocates each value to 0 using an assembly reference for maximum optimization
            memset(dots, 0, sizeof(dots));
        }

        // This sets the state of a dot to true, 
        void setDot(int index) {
            dots[index] = true;
        };

        // Converts the Braille character object to a "wide" character for use with the translation library and file output
        wchar_t toChar() {

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
// These pins are board numbers, not BCM
const int GPIO_PINS[] = {7, 11, 12, 13, 15, 16, 18, 22, 29, 31, 32, 33, 35, 36, 37, 38, 40};

// The key value pairs to convert from the pin id to the dot id
unordered_map<int, int> dotPins;

// The number of pins contained in the GPIO_PINS array
const int numPins = sizeof(GPIO_PINS) / sizeof(int);

// The current storage of braille characters to be sent to the translation library
wstringstream line;

// The destination file for the text data to be sent to
wofstream outfile("output.txt");

// Keeps a record of what pins are active or not
// This is necessary for determining when the space key should be activated
// as the space key descends when any dot is pressed
bitset<numPins> pinState;

// The braille character in object form
Braille currChar;

// Responsible for handling the action to take when a pin is activated
void hallEffectTriggered(int pin)
{

    cout << "Pin " << pin << " was pressed" << endl;

    switch (pin) {
        // Button Pressed
        case BUTTON:
            // TODO add logic for determining if the action to take is
            // switching the translation language or creating a new page
            break;

        // Space
        case SPACE:

            // make sure the following is only executed if there are no other keys pressed while pressing space
            // outputs the current braille character
            if (pinState.none()) {

                // Adds the current braille character to the output line
                line.put(currChar.toChar());

                // Puts the character to the console, only works if the console supports braille characters
                wcout << currChar.toChar();
                currChar.clear();

                // Adds a space to the line
                line << L" ";
            }

        // Advance Pressed
        case ADVANCE:
            // Adds the current braille character to the output line
            line.put(currChar.toChar());

            // Puts the character to the console, only works if the console supports braille characters
            wcout << currChar.toChar();
            currChar.clear();
            break;

        // Backspace
        case BACKSPACE:
            // If the character count of the wstringbuffer is zero, we cancel the action
            if (line.gcount() == 0) {
                break;
            }

            // Possible solution?
            // This line of code moves the current writing index back by one so the next character written will overwrite it
            line.seekp(-1, line.cur);
            break;
        
        default:
            // Sets the pinstate of the dot that corresponds with the activated pin as active
            // **This is necessary to filter out space key presses**
            pinState[dotPins[pin]] = 1;
            currChar.setDot(dotPins[pin]);
            break;
    }

}

// Responsible for clearing the bit containing 
void hallEffectDisabled(int pin)
{
    cout << "Pin " << pin << " was released" << endl;
    
    // Exits the method if the pressed pin was not a dot key
    if (dotPins.find(pin) == dotPins.end()) {
        return;
    }

    // Sets the hall effect sensor state to disabled
    // Only occurs for the dot hall effects
    pinState[dotPins[pin]] = 0;
}

int main()
{
    // Initializes the library   
    wiringPiSetup();
    
    // Set up key-value pairs for the dot pin unordered map (hashmap or dictionary equivalent in c++)
    dotPins[22] = 2;    // Dot 3
    dotPins[21] = 1;    // Dot 2
    dotPins[24] = 0;    // Dot 1

    dotPins[26] = 3;    // Dot 4
    dotPins[38] = 4;    // Dot 5
    dotPins[40] = 5; // Dot 6 Pin not working, need to fix and review documentation for correct pin


    // An array of functions that will be passed individually to each interrupt routine
    function<void()> isr_rising[numPins];
    function<void()> isr_falling[numPins];
    
    // Loops through all pins to set their respective properties
    for (int i = 0; i < numPins; i++) {

        // Creates an array of function pointers to store the dynamically generated function calls, passing in the pin number
        isr_rising[i] = [pin = GPIO_PINS[i]]() { hallEffectTriggered(pin); };
        isr_rising[i] = [pin = GPIO_PINS[i]]() { hallEffectDisabled(pin); };

        // Sets every pin as an input pin
        pinMode(GPIO_PINS[i], INPUT);

        // Assosciates the pin changing states with calling hallEffectTriggered
        // This is done by creating an array of functions, passing each a different pin
        // and then passing a pointer to the function to the ISR so that a new function doesn't need to be created for each pin.
        // It is not super optimized, as the switch case is 2 times per activation and may be better to change in the actual implementation
        wiringPiISR(GPIO_PINS[i], INT_EDGE_BOTH, *isr_rising[i].target<void (*)()>());

        // Only sets the interrupt on the dot pins so that an additional if statement can be avoided every execution
        if (dotPins.find(GPIO_PINS[i]) != dotPins.end()) {
            wiringPiISR(GPIO_PINS[i], INT_EDGE_FALLING, *isr_falling[i].target<void (*)()>());
        }
    }

    // Keeps the program running so that the interrupts function properly
    while (true) {
        // TODO:
        // Add logic to determine the active line via the scroll knob and append a newline character to the previous active line
    }

    return 0;
}