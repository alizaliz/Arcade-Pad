
#include "HID-Project.h"
#include "GamepadButton.h"

#define NUMBER_OF_SHIFT_CHIPS 2              // How many shift register chips are daisy-chained.
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8 // Width of data (how many ext lines).
#define PULSE_WIDTH_USEC 5                   // Width of pulse to trigger the shift register to read and latch.
#define POLL_DELAY_MSEC 1                    // Optional delay between shift register reads.

int ploadPin = 6;       // Connects to Parallel load pin the 165
int clockEnablePin = 8; // Connects to Clock Enable pin the 165
int dataPin = 9;        // Connects to the Q7 pin the 165
int clockPin = 7;       // Connects to the Clock pin the 165

unsigned int pinValues;

GamepadButton gpButtons[16] =
    {
        GamepadButton(GENERIC, 12, 500), // D
        GamepadButton(GENERIC, 11, 500), // B
        GamepadButton(GENERIC, 10, 500), // C
        GamepadButton(GENERIC, 9, 500), // A
        GamepadButton(GENERIC, 8, 500), // Down
        GamepadButton(GENERIC, 7, 500), // Up
        GamepadButton(GENERIC, 6, 500), // Right
        GamepadButton(GENERIC, 5, 500), // Left
        GamepadButton(GENERIC, 4, 500),
        GamepadButton(GENERIC, 3, 500),
        GamepadButton(GENERIC, 2, 500),
        GamepadButton(GENERIC, 1, 500),
        GamepadButton(D_PAD, GAMEPAD_DPAD_UP, 250), // Z
        GamepadButton(D_PAD, GAMEPAD_DPAD_RIGHT, 250), // X
        GamepadButton(D_PAD, GAMEPAD_DPAD_DOWN, 250), // Y
        GamepadButton(D_PAD, GAMEPAD_DPAD_LEFT, 250) // W
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

void setup()
{
  pinMode(ploadPin, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);

  Gamepad.begin();

  Serial.begin(9600);
}

void loop()
{
  pinValues = readShiftRegs();
  sendReports();

  delay(POLL_DELAY_MSEC);
}
