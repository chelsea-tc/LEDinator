#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include "GFButton.h"

class Buttonhandler
{
  public:
    void read_buttons(volatile bool *buttons, byte amount);
    void read_held_buttons(volatile bool *buttons, byte amount);
    byte get_last_pressed_menu_button(bool *menu_buttons);
    void read_menu_buttons(volatile bool *buttons, byte amount);
    void read_held_menu_buttons(volatile bool *buttons, byte amount);
    void clear_buttons(volatile bool *buttons, byte amount);

  private:
    GFButton button_objects[6] = {GFButton(4), GFButton(5), GFButton(6), GFButton(7), GFButton(8), GFButton(9)};
    GFButton menu_button_objects[4] = {GFButton(A0), GFButton(A1), GFButton(A2), GFButton(A3)};
};

#endif