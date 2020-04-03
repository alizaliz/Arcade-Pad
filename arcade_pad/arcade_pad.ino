
#include "Bounce2.h"
#include "HID-Project.h"
#include "GamepadButton.h"

#define NUMBER_OF_SHIFT_CHIPS 2              // How many shift register chips are daisy-chained.
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8 // Width of data (how many ext lines).
#define PULSE_WIDTH_USEC 5                   // Width of pulse to trigger the shift register to read and latch.
#define POLL_DELAY_MSEC 1                    // Optional delay between shift register reads.
#define DEFAULT_DEBOUNCE_TIME 20             // Switch actuation debouce time
#define DEFAULT_RETRIGGER_TIME 500           // Hold retrigger time (Turbo)

int ploadPin = 6;       // Connects to Parallel load pin the 165
int clockEnablePin = 8; // Connects to Clock Enable pin the 165
int dataPin = 9;        // Connects to the Q7 pin the 165
int clockPin = 7;       // Connects to the Clock pin the 165

int potPin = A0;                      // Input pin for potentiometer
int turboButtonPin = 15;              // Turbo switch
const int potExtents[2] = {140, 950}; // Values related to analog read min/max (determined by resistance)

Bounce turboButton = Bounce(); // Latching button for turbo

unsigned int pinValues;
uint16_t oldPotValue = DEFAULT_RETRIGGER_TIME;
uint16_t curPotValue = DEFAULT_RETRIGGER_TIME;

GamepadButton gpButtons[DATA_WIDTH] =
    {
        GamepadButton(GENERIC, KEY_D, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // D
        GamepadButton(GENERIC, KEY_B, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // B
        GamepadButton(GENERIC, KEY_C, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // C
        GamepadButton(GENERIC, KEY_A, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // A
        GamepadButton(D_PAD, KEY_DOWN_ARROW, DEFAULT_DEBOUNCE_TIME),                  // Down
        GamepadButton(D_PAD, KEY_UP_ARROW, DEFAULT_DEBOUNCE_TIME),                    // Up
        GamepadButton(D_PAD, KEY_RIGHT_ARROW, DEFAULT_DEBOUNCE_TIME),                 // Right
        GamepadButton(D_PAD, KEY_LEFT_ARROW, DEFAULT_DEBOUNCE_TIME),                  // Left
        GamepadButton(CONTROL, KEY_ESC, DEFAULT_DEBOUNCE_TIME),                       // Escape
        GamepadButton(CONTROL, KEY_ENTER, DEFAULT_DEBOUNCE_TIME),                     // RETURN
        GamepadButton(GENERIC, KEY_V, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // V
        GamepadButton(GENERIC, KEY_U, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // U
        GamepadButton(GENERIC, KEY_Z, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // Z
        GamepadButton(GENERIC, KEY_X, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // X
        GamepadButton(GENERIC, KEY_Y, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME), // Y
        GamepadButton(GENERIC, KEY_W, DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME)  // W
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

// Send appropriate HID reports
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
  curPotValue = (uint16_t)map(analogRead(potPin), potExtents[0], potExtents[1], DEFAULT_DEBOUNCE_TIME, DEFAULT_RETRIGGER_TIME);
  if (abs(curPotValue - oldPotValue) < 2) // Remove noise
    return;

  oldPotValue = curPotValue;
  setButtonTriggerTime(curPotValue);
}

void setButtonTriggerTime(uint16_t time)
{
  for (int i = 0; i < DATA_WIDTH; i++)
  {
    if (gpButtons[i].getType() == GENERIC)
      gpButtons[i].retriggerTime(time);
  }
}

void enableTurbo(bool enable)
{
  for (int i = 0; i < DATA_WIDTH; i++)
  {
    if (gpButtons[i].getType() == GENERIC)
      gpButtons[i].setTurbo(enable);
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

  pinMode(potPin, INPUT_PULLUP);

  Keyboard.begin();
}

void loop()
{
  // Torbo! latch switch
  turboButton.update();
  if (turboButton.rose())
  {
    enableTurbo(true);
  }
  if (turboButton.read() == HIGH)
  {
    updateTurbo();
  }
  else if (turboButton.fell())
  {
    enableTurbo(false);
  }

  // Update shift register
  pinValues = readShiftRegs();
  sendReports();

  //delay(POLL_DELAY_MSEC);
}
