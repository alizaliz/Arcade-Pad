
#include "Bounce2.h"
#include "HID-Project.h"
#include "GamepadButton.h"

#define NUMBER_OF_SHIFT_CHIPS 2              // How many shift register chips are daisy-chained.
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8 // Width of data (how many ext lines).
#define PULSE_WIDTH_USEC 5                   // Width of pulse to trigger the shift register to read and latch.
#define POLL_DELAY_MSEC 1                    // Optional delay between shift register reads.
#define DEFAULT_DEBOUNCE_TIME 500

int ploadPin = 6;       // Connects to Parallel load pin the 165
int clockEnablePin = 8; // Connects to Clock Enable pin the 165
int dataPin = 9;        // Connects to the Q7 pin the 165
int clockPin = 7;       // Connects to the Clock pin the 165

int potPin = A0;   // Input pin for potentiometer
int turboButtonPin = 15; // Turbo switch

Bounce turboButton = Bounce(); // Latching button for turbo

unsigned int pinValues;
uint16_t oldPotValue = DEFAULT_DEBOUNCE_TIME;  
uint16_t curPotValue = DEFAULT_DEBOUNCE_TIME;


GamepadButton gpButtons[16] =
    {
        GamepadButton(D_PAD, GAMEPAD_DPAD_UP, 250),
        GamepadButton(D_PAD, GAMEPAD_DPAD_RIGHT, 250),
        GamepadButton(D_PAD, GAMEPAD_DPAD_DOWN, 250),
        GamepadButton(D_PAD, GAMEPAD_DPAD_LEFT, 250),
        GamepadButton(GENERIC, 1, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 2, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 3, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 4, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 5, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 6, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 7, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 8, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 9, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 10, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 11, DEFAULT_DEBOUNCE_TIME),
        GamepadButton(GENERIC, 12, DEFAULT_DEBOUNCE_TIME),
};

/* This function is essentially a "shift-in" routine reading the
 * serial Data from the shift register chips and representing
 * the state of those pins in an unsigned integer (or long).
*/
unsigned int readShiftRegs()
{
  long bitVal;
  unsigned int bytesVal = 0;

  // Trigger a parallel Load to latch the state of the data lines,
  digitalWrite(clockEnablePin, HIGH);
  digitalWrite(ploadPin, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(ploadPin, HIGH);
  digitalWrite(clockEnablePin, LOW);

  // Loop to read each bit value from the serial out line of the SN74HC165N.
  for (int i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(dataPin);

    // Set the corresponding bit in bytesVal.
    bytesVal |= (bitVal << ((DATA_WIDTH - 1) - i));

    // Pulse the Clock (rising edge shifts the next bit).
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(clockPin, LOW);
  }
  return (bytesVal);
}

void sendReports()
{
  for (int i = 0; i < DATA_WIDTH; i++)
  {
    if ((pinValues >> i) & 1)
    {
      gpButtons[i].invoke();
    }
    else
    {
      gpButtons[i].release();
    }
  }
}

void updateTurbo()
{
  // Read slider potentiometer
  curPotValue = (uint16_t)map(analogRead(potPin), 0 , 1020 , 0 , DEFAULT_DEBOUNCE_TIME);
  if(curPotValue != oldPotValue) return;
  
  oldPotValue = curPotValue;
  setButtonDebounce(curPotValue);
  
}

void setButtonDebounce(uint16_t time)
{
  for (int i = 4; i < 12 ; i++)
  {
    gpButtons[i].debounceTime(time);
  }
}

void setup()
{
  pinMode(ploadPin, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);

  turboButton.attach(turboButtonPin);
  turboButton.interval(DEFAULT_DEBOUNCE_TIME); 

  Gamepad.begin();
}

void loop()
{
  // Torbo! latch switch
  turboButton.update();
  if(turboButton.read() == HIGH)
  {
    updateTurbo();
  }
  else if(turboButton.fell())
  {
    setButtonDebounce(DEFAULT_DEBOUNCE_TIME);
  }
  // Update shift register
  pinValues = readShiftRegs();
  sendReports();

  delay(POLL_DELAY_MSEC);
}
