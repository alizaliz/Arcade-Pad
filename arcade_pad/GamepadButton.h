/*
 * Gamepad button.h
 * 
 * 
 */

 #ifndef GamepadButton_h
 #define GamepadButton_h

 #include "Arduino.h"

// Type of button
 #define D_PAD 0
 #define GENERIC 1

class GamepadButton
{
  public:
  
    GamepadButton(int type, uint8_t button_value, uint16_t interval_millis);
    void setType(int type);
    void setValue(uint8_t button_value);
    void debounceTime(uint16_t interval_millis);
    void invoke();
    void release();

  protected:

    int type = GENERIC;
    int32_t previous_millis = 0;
    int32_t interval_millis = 0;
    uint8_t button_value = 0;
    bool active = false;

  private:

    inline bool debounceTimeExpired();
};

#endif
