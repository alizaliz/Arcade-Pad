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

class GamepadButton
{
  public:
  
    GamepadButton(int type, KeyboardKeycode button_value, uint16_t interval_millis);
    void setType(int type);
    void setValue(KeyboardKeycode button_value);
    void debounceTime(uint16_t interval_millis);
    void invoke();
    void release();

  protected:

    int type = GENERIC;
    int32_t previous_millis;
    int32_t interval_millis;
    KeyboardKeycode button_value;
    bool active;

  private:

    inline bool debounceTimeExpired();
};

#endif
