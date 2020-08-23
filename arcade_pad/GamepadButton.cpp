/*
 * GamepadButton.cpp
 * 
 * 
 */

#include "Arduino.h"
#include "GamepadButton.h"

GamepadButton::GamepadButton(int type, KeyboardKeycode button_value, uint16_t debounce_millis)
{
  this->type = type;
  this->button_value = button_value;
  this->debounce_millis = debounce_millis;
}

GamepadButton::GamepadButton(int type, KeyboardKeycode button_value, uint16_t debounce_millis, uint16_t retrigger_millis)
{
  this->type = type;
  this->button_value = button_value;
  this->debounce_millis = debounce_millis;
  this->retrigger_millis = retrigger_millis;
}

int GamepadButton::getType()
{
  return type;
}

void GamepadButton::setType(int type)
{
  this->type = type;
}

void GamepadButton::setValue(KeyboardKeycode button_value)
{
  this->button_value = button_value;
}

void GamepadButton::debounceTime(uint16_t debounce_millis)
{
  this->debounce_millis = debounce_millis;
}

void GamepadButton::retriggerTime(uint16_t retrigger_millis)
{
  this->retrigger_millis = retrigger_millis;
}

void GamepadButton::setTurbo(bool enable)
{
  this->turbo = enable;
}

bool GamepadButton::debounceTimeExpired()
{
  return (millis() - previous_millis >= debounce_millis);
}

bool GamepadButton::retriggerTimeExpired()
{
  return (millis() - previous_millis >= retrigger_millis);
}

void GamepadButton::invoke()
{
  if (!turbo && debounceTimeExpired())
  {
    Keyboard.press(button_value);
  }
  else if (type == GENERIC && turbo && retriggerTimeExpired())
  {
    Keyboard.write(button_value);
    previous_millis = millis();
  }
  active = true;
}

void GamepadButton::release()
{
  if (!active)
  {
    Keyboard.release(button_value);
    active = false;
    previous_millis = millis();
  }
}
