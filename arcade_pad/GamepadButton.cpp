/*
 * GamepadButton.cpp
 * 
 * 
 */

 #include "Arduino.h"
 #include "GamepadButton.h"

 GamepadButton::GamepadButton(int type, KeyboardKeycode button_value, uint16_t interval_millis)
 {
    this->type = type;
    this->button_value = button_value;
    this->interval_millis = interval_millis;
 }

 void GamepadButton::setType(int type)
 {
    this->type = type;
 }

 void GamepadButton::setValue(KeyboardKeycode button_value)
 {
    this->button_value = button_value;
 }

 void GamepadButton::debounceTime(uint16_t interval_millis)
 {
    this->interval_millis = interval_millis;
 }

 bool GamepadButton::debounceTimeExpired()
 {
    return ( millis() - previous_millis >= interval_millis);
 }
 
 void GamepadButton::invoke()
 {
    if(!active && debounceTimeExpired())
    {
      switch(type)
      {
        case D_PAD:
          Keyboard.press(button_value);
          break;
        case GENERIC:
          Keyboard.press(button_value);
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
          Keyboard.release(button_value);
          break;
        case GENERIC:
          Keyboard.release(button_value);
          break;
      }
      active = false;
      previous_millis = millis();
    }
 }
