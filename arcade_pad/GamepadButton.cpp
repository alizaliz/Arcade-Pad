/*
 * GamepadButton.cpp
 * 
 * 
 */

 #include "Arduino.h"
 #include "GamepadButton.h"
 #include "HID-Project.h"

 GamepadButton::GamepadButton(int type, uint8_t button_value, uint16_t interval_millis)
 {
    this->type = type;
    this->button_value = button_value;
    this->interval_millis = interval_millis;
 }

 void GamepadButton::setType(int type)
 {
    this->type = type;
 }

 void GamepadButton::setValue(uint8_t button_value)
 {
    this->button_value = button_value;
 }

 void GamepadButton::debounceTime(uint16_t interval_millis)
 {
    this->interval_millis = interval_millis;
 }

 bool GamepadButton::debounceTimeExpired()
 {
    if( millis() - previous_millis >= interval_millis)
    {
      previous_millis = millis();
      return true;
    }
    return false;
 }
 
 void GamepadButton::invoke()
 {
    if(!active | debounceTimeExpired())
    {
      switch(type)
      {
        case D_PAD:
          Gamepad.dPad1(button_value);
          break;
        case GENERIC:
          Gamepad.press(button_value);
          break;
      }
    }
    active = true;
 }

 void GamepadButton::release()
 {
    if(active)
    {
      switch(type)
      {
        case D_PAD:
          Gamepad.dPad1(GAMEPAD_DPAD_CENTERED);
          break;
        case GENERIC:
          Gamepad.release(button_value);
          break;
      }
      active = false;
    }
 }
