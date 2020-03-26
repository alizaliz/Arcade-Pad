
#include "HID-Project.h"
#include "GamepadButton.h"

#define NUMBER_OF_SHIFT_CHIPS 2              // How many shift register chips are daisy-chained.
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8 // Width of data (how many ext lines).
#define PULSE_WIDTH_USEC 5                   // Width of pulse to trigger the shift register to read and latch.
#define POLL_DELAY_MSEC 1                    // Optional delay between shift register reads.
#define DEFAULT_DEBOUNCE_TIME 25             // Switch actuation debouce time

int ploadPin = 6;       // Connects to Parallel load pin the 165
int clockEnablePin = 8; // Connects to Clock Enable pin the 165
int dataPin = 9;        // Connects to the Q7 pin the 165
int clockPin = 7;       // Connects to the Clock pin the 165

unsigned int pinValues;

GamepadButton gpButtons[16] =
    {
        GamepadButton(GENERIC, KEY_D, DEFAULT_DEBOUNCE_TIME), // D
        GamepadButton(GENERIC, KEY_B, DEFAULT_DEBOUNCE_TIME), // B
        GamepadButton(GENERIC, KEY_C, DEFAULT_DEBOUNCE_TIME), // C
        GamepadButton(GENERIC, KEY_A, DEFAULT_DEBOUNCE_TIME), // A
        GamepadButton(D_PAD, KEY_DOWN_ARROW, DEFAULT_DEBOUNCE_TIME), // Down
        GamepadButton(D_PAD, KEY_UP_ARROW, DEFAULT_DEBOUNCE_TIME), // Up
        GamepadButton(D_PAD, KEY_RIGHT_ARROW, DEFAULT_DEBOUNCE_TIME), // Right
        GamepadButton(D_PAD, KEY_LEFT_ARROW, DEFAULT_DEBOUNCE_TIME), // Left
        GamepadButton(GENERIC, KEY_ESC, DEFAULT_DEBOUNCE_TIME), // Empty
        GamepadButton(GENERIC, KEY_ENTER, DEFAULT_DEBOUNCE_TIME), // Empty
        GamepadButton(GENERIC, KEY_T, DEFAULT_DEBOUNCE_TIME), // Empty
        GamepadButton(GENERIC, KEY_R, DEFAULT_DEBOUNCE_TIME), // Empty
        GamepadButton(GENERIC, KEY_Z, DEFAULT_DEBOUNCE_TIME), // Z
        GamepadButton(GENERIC, KEY_X, DEFAULT_DEBOUNCE_TIME), // X
        GamepadButton(GENERIC, KEY_Y, DEFAULT_DEBOUNCE_TIME), // Y
        GamepadButton(GENERIC, KEY_W, DEFAULT_DEBOUNCE_TIME) // W
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

  Keyboard.begin();
}

void loop()
{
  pinValues = readShiftRegs();
  sendReports();

  delay(POLL_DELAY_MSEC);
}
