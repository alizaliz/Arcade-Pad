/*
 * Gamepad button.h
 * 
 * 
 */

#ifndef GamepadButton_h
#define GamepadButton_h

#include "Arduino.h"
#include "HID-Project.h"

// Type of button
#define D_PAD 0
#define GENERIC 1
#define CONTROL 2

class GamepadButton
{
public:
  GamepadButton(int type, KeyboardKeycode button_value, uint16_t debounce_millis);
  GamepadButton(int type, KeyboardKeycode button_value, uint16_t debounce_millis, uint16_t retrigger_millis);
  int getType();
  void setType(int type);
  void setValue(KeyboardKeycode button_value);
  void debounceTime(uint16_t debounce_millis);
  void retriggerTime(uint16_t retrigger_millis);
  void setTurbo(bool enable);
  void invoke();
  void release();

protected:
  int type = GENERIC;
  int32_t previous_millis;
  int32_t debounce_millis;
  int32_t retrigger_millis;
  KeyboardKeycode button_value;
  bool active;
  bool triggerable;
  bool turbo;

private:
  inline bool debounceTimeExpired();
  inline bool retriggerTimeExpired();
};

#endif
